//
// Created by Stanislav Svědiroh on 15.06.2022.
//
#pragma once

#include <rendering/Camera.h>
#include <platform/vulkan/VulkanGameObject.h>
#include <vulkan/vulkan.h>

# define MAX_LIGHTS 10

namespace VulkanEngine {
    struct PointLight {
        glm::vec3 position{};
        alignas(16) glm::vec4 color{};
    };

    struct GlobalUbo {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::mat4 inverseView{1.f};
        glm::vec4 ambientLightColor{0.609f, 0.18f, 0.207f, .08f}; // w is intensity
        PointLight pointLights[MAX_LIGHTS];
        int numLights;
    };

    struct FrameInfo {
        VkCommandBuffer commandBuffer;
        Camera &camera;
        VkDescriptorSet globalDescriptorSet;
        VulkanGameObject::Map &gameObjects;
    };
}
