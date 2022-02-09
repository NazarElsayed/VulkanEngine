#include "vt_pipeline.h"

#include "vt_model.h"

//std
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

namespace vt {

    VtPipeline::VtPipeline(
        VtDevice& _device,
        const std::string& _vertFilepath,
        const std::string& _fragFilepath,
        const PipelineConfigInfo& _configInfo) : vtDevice{ _device } {
        createGraphicsPipeline(_vertFilepath, _fragFilepath, _configInfo);
    }

    VtPipeline::~VtPipeline() {
        vkDestroyShaderModule(vtDevice.device(), vertShaderModule, nullptr);
        vkDestroyShaderModule(vtDevice.device(), fragShaderModule, nullptr);
        vkDestroyPipeline(vtDevice.device(), graphicsPipeline, nullptr);
    }

    std::vector<char> VtPipeline::readFile(const std::string& _filepath) {

        std::ifstream file{ _filepath, std::ios::ate | std::ios::binary };

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + _filepath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void VtPipeline::createGraphicsPipeline(const std::string& _vertFilePath, const std::string& _fragFilepath, const PipelineConfigInfo& _configInfo) {

        assert(_configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline:: no pipelineLayout Provided in configInfo");
        assert(_configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline:: no renderPass in configInfo");

        auto vertCode = readFile(_vertFilePath);
        auto fragCode = readFile(_fragFilepath);

        createShaderModule(vertCode, &vertShaderModule);
        createShaderModule(fragCode, &fragShaderModule);

        VkPipelineShaderStageCreateInfo shaderStages[2];

        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = fragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        auto bindingDescriptions = VtModel::Vertex::getBindingDescriptions();
        auto attributeDescriptions = VtModel::Vertex::getAttributeDescriptions();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &_configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &_configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &_configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &_configInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &_configInfo.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &_configInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = &_configInfo.dynamicStateInfo;

        pipelineInfo.layout = _configInfo.pipelineLayout;
        pipelineInfo.renderPass = _configInfo.renderPass;
        pipelineInfo.subpass = _configInfo.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(vtDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline");
        }
    }

    void VtPipeline::createShaderModule(const std::vector<char>& _code, VkShaderModule* _shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = _code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());

        if (vkCreateShaderModule(vtDevice.device(), &createInfo, nullptr, _shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }
    }

    void VtPipeline::bind(VkCommandBuffer _commandBuffer) {
        vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    }


    void VtPipeline::defaultPipelineConfigInfo(PipelineConfigInfo& _configInfo) {

        _configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        _configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        _configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        _configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        _configInfo.viewportInfo.viewportCount = 1;
        _configInfo.viewportInfo.pViewports = nullptr;
        _configInfo.viewportInfo.scissorCount = 1;
        _configInfo.viewportInfo.pScissors = nullptr;

        _configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        _configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
        _configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        _configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        _configInfo.rasterizationInfo.lineWidth = 1.0f;
        _configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        _configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        _configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
        _configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
        _configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
        _configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

        _configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        _configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
        _configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        _configInfo.multisampleInfo.minSampleShading = 1.0f;
        _configInfo.multisampleInfo.pSampleMask = nullptr;
        _configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
        _configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;

        _configInfo.colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        _configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
        _configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        _configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        _configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        _configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        _configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        _configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        _configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        _configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
        _configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
        _configInfo.colorBlendInfo.attachmentCount = 1;
        _configInfo.colorBlendInfo.pAttachments = &_configInfo.colorBlendAttachment;
        _configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
        _configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
        _configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
        _configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

        _configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        _configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
        _configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
        _configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        _configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        _configInfo.depthStencilInfo.minDepthBounds = 0.0f;
        _configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
        _configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
        _configInfo.depthStencilInfo.front = {};
        _configInfo.depthStencilInfo.back = {};

        _configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        _configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        _configInfo.dynamicStateInfo.pDynamicStates = _configInfo.dynamicStateEnables.data();
        _configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(_configInfo.dynamicStateEnables.size());
        _configInfo.dynamicStateInfo.flags = 0;
    }
}