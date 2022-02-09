#pragma once

#include "vt_window.h"
#include "vt_pipeline.h"
#include "vt_device.h"
#include "vt_swap_chain.h"
#include "vt_model.h"

#include <memory>
#include <vector>

namespace vt {

    class FirstApp {

    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp&) = delete;
        FirstApp& operator = (const FirstApp&) = delete;

        void run();

    private:
        void loadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void DrawFrame();
        void RecreateSwapChain();
        void RecordCommandBuffer(int imageIndex);

        void SierpinskiTriangle(std::vector<VtModel::Vertex>& _vertices, int _depth, glm::vec2 _top, glm::vec2 _left, glm::vec2 _right);

        VtWindow vtWindow{ WIDTH, HEIGHT, "Vulkan Tutorial" };
        VtDevice vtDevice{ vtWindow };
        std::unique_ptr<VtSwapChain> vtSwapChain;
        std::unique_ptr<VtPipeline> vtPipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<VtModel> vtModel;
    };
}