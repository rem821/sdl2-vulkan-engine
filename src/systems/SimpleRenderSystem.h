//
// Created by Stanislav Svědiroh on 14.06.2022.
//
#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "../rendering/VulkanEngineDevice.h"
#include "../rendering/VulkanEnginePipeline.h"
#include "../rendering/GameObject.h"
#include "../rendering/Camera.h"
#include "../rendering/VulkanEngineFrameInfo.h"

#include "../CustomTypes.h"
#include "../TextureManager.h"
#include "../Map.h"
#include "fmt/core.h"
#include <memory>
#include <vector>
#include <stdexcept>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "../../libs/glm/glm.hpp"
#include "../../libs/glm/gtc/constants.hpp"

struct SimplePushConstantData {
    glm::mat4 modelMatrix{1.f};
    glm::mat4 normalMatrix{1.f};
};

class SimpleRenderSystem {

public:
    SimpleRenderSystem(VulkanEngineDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();
    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(FrameInfo &frameInfo);

private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    VulkanEngineDevice &engineDevice;

    std::unique_ptr<VulkanEnginePipeline> enginePipeline;
    VkPipelineLayout pipelineLayout;
};