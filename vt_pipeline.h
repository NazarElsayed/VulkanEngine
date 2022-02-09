#pragma once

#include "vt_device.h"

//std
#include <string>
#include <vector>

namespace vt {

    struct PipelineConfigInfo {

        PipelineConfigInfo() = default;

        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class VtPipeline {
    public:
        VtPipeline(
            VtDevice& _device,
            const std::string& _vertFilepath,
            const std::string& _fragFilepath,
            const PipelineConfigInfo& _configInfo
        );
        ~VtPipeline();

        VtPipeline(const VtPipeline&) = delete;
        void operator=(const VtPipeline&) = delete;

        void bind(VkCommandBuffer _commandBuffer);

        static void defaultPipelineConfigInfo(PipelineConfigInfo& _configInfo);

    private:
        static std::vector<char> readFile(const std::string& _filepath);

        void createGraphicsPipeline(const std::string& _vertFilePath, const std::string& _fragFilepath, const PipelineConfigInfo& _configInfo);

        void createShaderModule(const std::vector<char>& _code, VkShaderModule* _shaderModule);

        VtDevice& vtDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}