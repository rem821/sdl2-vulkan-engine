//
// Created by standa on 5.3.23.
//
#include <platform/vulkan/VulkanContext.h>
#include <imgui_impl_vulkan.h>

namespace VulkanEngine {

    VulkanContext::VulkanContext(const Window &window) : window_(window) {
        CORE_INFO("Creating Vulkan Context!");

    }

    void VulkanContext::Init() {
        vulkanDevice_ = std::make_unique<VulkanDevice>((GLFWwindow *) window_.GetNativeWindow(), true);
        vulkanRenderer_ = std::make_unique<VulkanRenderer>(window_, *vulkanDevice_);
        globalPool_ = VulkanDescriptorPool::Builder(*vulkanDevice_)
                .SetMaxSets(1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
                .AddPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
                .Build();

        globalSetLayout_ = VulkanDescriptorSetLayout::Builder(*vulkanDevice_)
                .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
                .Build();

        renderSystem_ = std::make_unique<VulkanRenderSystem>(*vulkanDevice_, vulkanRenderer_->GetSwapChainRenderPass(), globalSetLayout_->GetDescriptorSetLayout(),
                                                             VK_POLYGON_MODE_FILL,
                                                             VK_CULL_MODE_BACK_BIT);
        pointLightSystem_ = std::make_unique<VulkanPointLightSystem>(*vulkanDevice_, vulkanRenderer_->GetSwapChainRenderPass(), globalSetLayout_->GetDescriptorSetLayout());

        for (auto &uboBuffer: uboBuffers) {
            uboBuffer = std::make_unique<VulkanBuffer>(
                    *vulkanDevice_,
                    sizeof(GlobalUbo),
                    1,
                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );
            uboBuffer->Map();
        }

        for (unsigned long i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->DescriptorInfo();
            VulkanDescriptorWriter(*globalSetLayout_, *globalPool_)
                    .WriteBuffer(0, &bufferInfo)
                    .Build(globalDescriptorSets[i]);
        }

        camera.SetViewYXZ({0.f, 0.f, 0.f}, {0.f, 0.f, 0.f});

        auto gameObject = VulkanGameObject::CreateGameObject();
        gameObject.model = VulkanModel::CreateModelFromFile(*vulkanDevice_, "./Rover3.obj");
        gameObject.transform = {{0.f,  -100.f, 250.0f},
                                {1.0f, 1.0f,   1.0f},
                                {0.0f, 0.0f,   0.0f}};
        gameObjects.emplace(gameObject.GetId(), std::move(gameObject));

        auto pointLight = VulkanGameObject::MakePointLight(500000.0f);
        pointLight.color = {0.8f, 0.8f, 0.8f};
        pointLight.transform.scale = {10.0f, 10.f, 10.f};
        pointLight.transform.translation.y = -50.0f;
        pointLight.transform.translation.z = 150.0f;

        gameObjects.emplace(pointLight.GetId(), std::move(pointLight));
    }

    void VulkanContext::InitImGuiVulkan() {
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = vulkanDevice_->GetInstance();
        init_info.PhysicalDevice = vulkanDevice_->GetPhysicalDevice();
        init_info.Device = vulkanDevice_->GetDevice();
        init_info.QueueFamily = vulkanDevice_->FindPhysicalQueueFamilies().graphicsFamily;
        init_info.Queue = vulkanDevice_->GraphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = globalPool_->GetPool();
        init_info.Subpass = 0;
        init_info.MinImageCount = VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
        init_info.ImageCount = VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = nullptr;
        ImGui_ImplVulkan_Init(&init_info, vulkanRenderer_->GetSwapChainRenderPass());

        // Upload fonts
        VkCommandPool command_pool = vulkanDevice_->GetCommandPool();
        VkCommandBuffer command_buffer;

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = vulkanDevice_->GetCommandPool();
        allocInfo.commandBufferCount = 1;
        CORE_ASSERT(vkAllocateCommandBuffers(vulkanDevice_->GetDevice(), &allocInfo, &command_buffer) == VK_SUCCESS, "Failed to allocate command buffer!")

        CORE_ASSERT(vkResetCommandPool(vulkanDevice_->GetDevice(), command_pool, 0) == VK_SUCCESS, "Couldn't reset command pool")
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        CORE_ASSERT(vkBeginCommandBuffer(command_buffer, &begin_info) == VK_SUCCESS, "Couldn't begin command buffer")

        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &command_buffer;
        CORE_ASSERT(vkEndCommandBuffer(command_buffer) == VK_SUCCESS, "Couldn't end command buffer")
        CORE_ASSERT(vkQueueSubmit(vulkanDevice_->GraphicsQueue(), 1, &end_info, VK_NULL_HANDLE) == VK_SUCCESS, "Couldn't submit queue!")

        CORE_ASSERT(vkDeviceWaitIdle(vulkanDevice_->GetDevice()) == VK_SUCCESS, "VkDeviceWaitIdle error!")
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    VkCommandBuffer VulkanContext::BeginFrame() {
        auto commandBuffer = vulkanRenderer_->BeginFrame();

        camera.SetPerspectiveProjection(glm::radians(60.f), vulkanRenderer_->GetAspectRatio(), 0.1f, 1000.f);

        int frameIndex = vulkanRenderer_->GetFrameIndex();
        FrameInfo frameInfo{commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};

        GlobalUbo ubo{};
        ubo.projection = camera.GetProjection();
        ubo.view = camera.GetView();
        ubo.inverseView = camera.GetInverseView();
        pointLightSystem_->Update(frameInfo, ubo);
        uboBuffers[frameIndex]->WriteToBuffer(&ubo);
        uboBuffers[frameIndex]->Flush();

        vulkanRenderer_->BeginSwapChainRenderPass(commandBuffer);

        renderSystem_->RenderGameObjects(frameInfo);
        pointLightSystem_->Render(frameInfo);

        return commandBuffer;
    }

    void VulkanContext::EndFrame(VkCommandBuffer commandBuffer) {
        vulkanRenderer_->EndSwapChainRenderPass(commandBuffer);
        vulkanRenderer_->EndFrame();
    }
}