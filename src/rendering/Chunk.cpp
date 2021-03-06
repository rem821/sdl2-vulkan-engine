#include "Chunk.h"

GameObject Chunk::getChunkBorders(VulkanEngineDevice &_device, glm::vec2 chunk_pos) {
    VulkanEngineModel::Builder bordersBuilder{};

    glm::vec3 size = {CHUNK_SIZE, CHUNK_SIZE, CHUNK_DEPTH};
    glm::vec3 pos = {chunk_pos, 0};

    VulkanEngineModel::Builder faces = Block::getCubeFaces(pos, size, true, true, false, false, true, true);

    for (auto vertex: faces.vertices) {
        bordersBuilder.vertices.emplace_back(vertex);
    }
    for (auto index: faces.indices) {
        bordersBuilder.indices.emplace_back(index);
    }

    GameObject obj = GameObject::createGameObject();
    obj.model = std::make_unique<VulkanEngineModel>(_device, bordersBuilder);
    obj.color = glm::vec3(1.0f, 0.0f, 0.0f);
    obj.transform.translation = {0.0f, 0.0f, 0.0f};
    obj.isWireFrame = true;
    obj.isActive = false;

    return obj;
}
