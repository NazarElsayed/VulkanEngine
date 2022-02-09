#include "vt_model.h"

#include <cassert>
#include <cstring>

namespace vt {

    VtModel::VtModel(VtDevice& _device, const std::vector<Vertex>& _vertices) : vtDevice{ _device } {
        createVertexBuffers(_vertices);
    }

    VtModel::~VtModel() {
        vkDestroyBuffer(vtDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(vtDevice.device(), vertexBufferMemory, nullptr);
    }

    std::vector<VkVertexInputBindingDescription> VtModel::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> VtModel::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, colour);
        return attributeDescriptions;
    }

    void VtModel::bind(VkCommandBuffer _commandBuffer) {
        VkBuffer buffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(_commandBuffer, 0, 1, buffers, offsets);
    }

    void VtModel::draw(VkCommandBuffer _commandBuffer) {
        vkCmdDraw(_commandBuffer, vertexCount, 1, 0, 0);
    }

    void VtModel::createVertexBuffers(const std::vector<Vertex>& _vertices) {
        vertexCount = static_cast<uint32_t>(_vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize BufferSize = sizeof(_vertices[0]) * vertexCount;

        vtDevice.createBuffer(
            BufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer,
            vertexBufferMemory);

        void* data;
        vkMapMemory(vtDevice.device(), vertexBufferMemory, 0, BufferSize, 0, &data);
        memcpy(data, _vertices.data(), static_cast<size_t>(BufferSize));
        vkUnmapMemory(vtDevice.device(), vertexBufferMemory);
    }
}