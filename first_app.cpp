#include "first_app.h"

#include <stdexcept>
#include <cassert>
#include <array>

namespace vt {

    FirstApp::FirstApp() {
        loadModels();
        CreatePipelineLayout();
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(vtDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run() {
        while (!vtWindow.shouldClose()) {
            glfwPollEvents();
            DrawFrame();
        }

        vkDeviceWaitIdle(vtDevice.device());
    }

    void FirstApp::loadModels() {
        std::vector<VtModel::Vertex> vertices{
            {{ 0.0f,-0.5f}, {1.0f, 0.0f, 0.0f}},
            {{ 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };

        //std::vector<VtModel::Vertex> vertices{};
        //SierpinskiTriangle(vertices, 7, { 0.0f, -0.9f }, { 0.9f, 0.9f }, { -0.9f, 0.9f });

        vtModel = std::make_unique<VtModel>(vtDevice, vertices);
    }

    void FirstApp::CreatePipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if (vkCreatePipelineLayout(vtDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::CreatePipeline() {
        assert(vtSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        VtPipeline::defaultPipelineConfigInfo(pipelineConfig);

        pipelineConfig.renderPass = vtSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        vtPipeline = std::make_unique<VtPipeline>(
            vtDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig
            );
    }

    void FirstApp::CreateCommandBuffers() {
        commandBuffers.resize(vtSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = vtDevice.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(vtDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }
    }

    void FirstApp::FreeCommandBuffers() {
        vkFreeCommandBuffers(vtDevice.device(), vtDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void FirstApp::DrawFrame() {
        uint32_t imageIndex;
        auto result = vtSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swap chain image!");
        }

        RecordCommandBuffer(imageIndex);
        result = vtSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vtWindow.wasWindowResized()) {
            vtWindow.resetWindowResizedFlag();
            RecreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image!");
        }
    }

    void FirstApp::RecreateSwapChain() {
        auto extent = vtWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = vtWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(vtDevice.device());

        if (vtSwapChain == nullptr) {
            vtSwapChain = std::make_unique<VtSwapChain>(vtDevice, extent);
        }
        else {
            vtSwapChain = std::make_unique<VtSwapChain>(vtDevice, extent, std::move(vtSwapChain));
            if (vtSwapChain->imageCount() != commandBuffers.size()) {
                FreeCommandBuffers();
                CreateCommandBuffers();
            }
        }

        CreatePipeline();
    }

    void FirstApp::RecordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vtSwapChain->getRenderPass();
        renderPassInfo.framebuffer = vtSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = vtSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(vtSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(vtSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, vtSwapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        vtPipeline->bind(commandBuffers[imageIndex]);
        vtModel->bind(commandBuffers[imageIndex]);
        vtModel->draw(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }
    }

    void FirstApp::SierpinskiTriangle(std::vector<VtModel::Vertex>& _vertices, int _depth, glm::vec2 _top, glm::vec2 _right, glm::vec2 _left) {
        glm::vec3 colour{ 1.0f, 0.3f, 0.0f };
        if (_depth <= 0) {
            _vertices.push_back({ _top , {1.0f, 0.0f, 0.0f} });
            _vertices.push_back({ _right, {0.0f, 1.0f, 0.0f} });
            _vertices.push_back({ _left , {0.0f, 0.0f, 1.0f} });
        }
        else {
            auto topRight = 0.5f * (_top + _right);
            auto leftTop = 0.5f * (_left + _top);
            auto rightLeft = 0.5f * (_right + _left);
            SierpinskiTriangle(_vertices, _depth - 1, _top, topRight, leftTop);
            SierpinskiTriangle(_vertices, _depth - 1, _right, rightLeft, topRight);
            SierpinskiTriangle(_vertices, _depth - 1, _left, leftTop, rightLeft);
        }
    }
}