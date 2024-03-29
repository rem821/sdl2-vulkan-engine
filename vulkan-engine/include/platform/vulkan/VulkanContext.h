//
// Created by standa on 5.3.23.
//
#pragma once

#include <precompiled_headers/PCH.h>
#include <renderer/GraphicsContext.h>
#include <platform/vulkan/VulkanDevice.h>
#include <platform/vulkan/VulkanRenderer.h>
#include <platform/vulkan/VulkanDescriptors.h>
#include <platform/vulkan/VulkanBuffer.h>
#include <platform/vulkan/VulkanRenderSystem.h>
#include <platform/vulkan/VulkanPointLightSystem.h>
#include <platform/vulkan/VulkanFrameInfo.h>
#include <platform/vulkan/VulkanGameObject.h>

namespace VulkanEngine {

    class VulkanContext : public GraphicsContext {
    public:
        explicit VulkanContext(const Window& window);

        void Init() override;
        void InitImGuiVulkan() override;

        VkCommandBuffer BeginFrame() override;
        void EndFrame(VkCommandBuffer commandBuffer) override;
    private:
        const Window& window_;

        std::unique_ptr<VulkanDevice> vulkanDevice_;
        std::unique_ptr<VulkanRenderer> vulkanRenderer_;
        std::unique_ptr<VulkanDescriptorPool> globalPool_;
        std::unique_ptr<VulkanDescriptorSetLayout> globalSetLayout_;
        std::unique_ptr<VulkanRenderSystem> renderSystem_;
        std::unique_ptr<VulkanPointLightSystem> pointLightSystem_;
        std::vector<std::unique_ptr<VulkanBuffer>> uboBuffers{VulkanSwapChain::MAX_FRAMES_IN_FLIGHT};
        std::vector<VkDescriptorSet> globalDescriptorSets{VulkanSwapChain::MAX_FRAMES_IN_FLIGHT};

        Camera camera{};
        VulkanGameObject::Map gameObjects{};
    };
}