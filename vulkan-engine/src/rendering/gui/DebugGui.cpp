#include "rendering/gui/DebugGui.h"

DebugGui::DebugGui(VulkanEngineDevice &engineDevice, VulkanEngineRenderer &renderer, GLFWwindow *window) {
    imguiPool = VulkanEngineDescriptorPool::Builder(engineDevice)
            .setMaxSets(1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
            .addPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
            .setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
            .build();

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = engineDevice.getInstance();
    init_info.PhysicalDevice = engineDevice.getPhysicalDevice();
    init_info.Device = engineDevice.getDevice();
    init_info.QueueFamily = engineDevice.findPhysicalQueueFamilies().graphicsFamily;
    init_info.Queue = engineDevice.graphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = imguiPool->getPool();
    init_info.Subpass = 0;
    init_info.MinImageCount = VulkanEngineSwapChain::MAX_FRAMES_IN_FLIGHT;
    init_info.ImageCount = VulkanEngineSwapChain::MAX_FRAMES_IN_FLIGHT;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = nullptr;
    init_info.CheckVkResultFn = nullptr;
    ImGui_ImplVulkan_Init(&init_info, renderer.getSwapChainRenderPass());

    VkCommandBuffer commandBuffer = engineDevice.beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    engineDevice.endSingleTimeCommands(commandBuffer);

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

DebugGui::~DebugGui() {
    ImGui_ImplVulkan_Shutdown();
}

void DebugGui::showWindow(const FrameInfo &frameInfo, GLFWwindow *window, GameObject::Map &gameObjects, const std::vector<uint32_t> &chunkBorderIds) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame(window);

    ImGui::NewFrame();

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_MenuBar;
    window_flags |= ImGuiWindowFlags_NoNav;
    window_flags |= ImGuiWindowFlags_NoBackground;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);


    if (!ImGui::Begin("Runtime info", nullptr, window_flags)) {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }


    // Frame time
    ImGui::Text("FrameTime: %f ms", frameInfo.frameTime * 1000);

    // Frames per second - moving average
    movingFPSAverage = (0.08f * (1.0f / frameInfo.frameTime)) + (1.0f - 0.08f) * movingFPSAverage;
    ImGui::Text("FPS: %f", movingFPSAverage);

    ImGui::Text("GameObjects: %lu", frameInfo.gameObjects.size());
    ImGui::Text("Camera position: x:%f, y:%f, z:%f", fromCameraToWorld(frameInfo.camera.getPosition()).x, fromCameraToWorld(frameInfo.camera.getPosition()).y,
                fromCameraToWorld(frameInfo.camera.getPosition()).z);
    ImGui::Text("Camera rotation: yaw:%f, pitch:%f, roll:%f", frameInfo.camera.getRotation().x, frameInfo.camera.getRotation().y,
                frameInfo.camera.getRotation().z);


    if (ImGui::Button("Render chunk borders")) {
        chunkBordersShown = !chunkBordersShown;
        for (auto id: chunkBorderIds) {
            gameObjects.at(id).isActive = chunkBordersShown;
        }
    }

    if (ImGui::Button("Render wireframes borders")) {
        renderWireframes = !renderWireframes;
        for (auto &obj: frameInfo.gameObjects) {
            GameObject &gameObj = obj.second;
            if (!gameObj.isActive || gameObj.renderMode == GameObject::RENDER_MODE_WIREFRAME) continue;

            if (renderWireframes) {
                gameObj.renderMode = GameObject::RENDER_MODE_BOTH;
            } else {
                gameObj.renderMode = GameObject::RENDER_MODE_FILLED;
            }
        }
    }

    if (ImGui::Button("Disable chunk loading")) {
        frameInfo.chunkLoadingDisabled = !frameInfo.chunkLoadingDisabled;
    }

    uint32_t vertices = 0;
    for (auto &kv: gameObjects) {
        auto &obj = kv.second;
        if (obj.model == nullptr) continue;
        if (!obj.isActive) continue;

        vertices += obj.model->getVertexCount();
    }

    ImGui::Text("Vertices: %u", vertices);

    ImGui::End();

    ImGui::Render();
}

void DebugGui::render(VkCommandBuffer &commandBuffer) {
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

void DebugGui::processEvent(SDL_Event event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}
