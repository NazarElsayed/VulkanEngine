#pragma once

#include "vt_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vt {

    class VtModel {
    public:

        struct Vertex {
            glm::vec2 position;
            glm::vec3 colour;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        VtModel(VtDevice& _device, const std::vector<Vertex>& _vertices);
        ~VtModel();

        VtModel(const VtModel&) = delete;
        VtModel& operator=(const VtModel&) = delete;

        void bind(VkCommandBuffer _commandBuffer);
        void draw(VkCommandBuffer _commandBuffer);

    private:
        void createVertexBuffers(const std::vector<Vertex>& _vertices);

        VtDevice& vtDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
    };
}