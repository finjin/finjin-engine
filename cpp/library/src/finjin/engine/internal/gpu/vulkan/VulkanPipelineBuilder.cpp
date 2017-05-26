//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Engine (finjin-engine).
//
//Finjin Engine is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN

#include "VulkanPipelineBuilder.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//VulkanGraphicsPipelineCreateInfoBuilder
VulkanGraphicsPipelineCreateInfoBuilder::VulkanGraphicsPipelineCreateInfoBuilder()
{
    Reset();
}

VulkanGraphicsPipelineCreateInfoBuilder::VulkanGraphicsPipelineCreateInfoBuilder(const VulkanGraphicsPipelineCreateInfoBuilder& other)
{
    operator = (other);
}

void VulkanGraphicsPipelineCreateInfoBuilder::operator = (const VulkanGraphicsPipelineCreateInfoBuilder& other)
{
    FINJIN_COPY_MEMORY(this, &other, sizeof(VulkanGraphicsPipelineCreateInfoBuilder));

    //Update pointers
    if (this->graphicsPipelineCreateInfo.pVertexInputState != nullptr)
        UseVertexInputState();
    if (this->graphicsPipelineCreateInfo.pInputAssemblyState != nullptr)
        UseInputAssemblyState();
    if (this->graphicsPipelineCreateInfo.pTessellationState != nullptr)
        UseTessellationState();
    if (this->graphicsPipelineCreateInfo.pViewportState != nullptr)
        UseViewportState();
    if (this->graphicsPipelineCreateInfo.pRasterizationState != nullptr)
        UseRasterizationState();
    if (this->graphicsPipelineCreateInfo.pMultisampleState != nullptr)
        UseMultisampleState();
    if (this->graphicsPipelineCreateInfo.pDepthStencilState != nullptr)
        UseDepthStencilState();
    if (this->graphicsPipelineCreateInfo.pColorBlendState != nullptr)
        UseColorBlendState();
    if (this->graphicsPipelineCreateInfo.pDynamicState != nullptr)
        UseDynamicState();

    if (this->multisampleState.pSampleMask != nullptr)
        this->multisampleState.pSampleMask = &this->multisampleMask;
    if (this->dynamicState.pDynamicStates != nullptr)
        this->dynamicState.pDynamicStates = this->dynamicStateValues.data();
    if (this->graphicsPipelineCreateInfo.pStages != nullptr)
        this->graphicsPipelineCreateInfo.pStages = this->stages.data();
}

void VulkanGraphicsPipelineCreateInfoBuilder::Reset()
{
    FINJIN_ZERO_ITEM(*this);

    this->graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    this->vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    this->inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    this->tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    this->viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    this->rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    this->multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    this->depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    this->colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    this->dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
}

void VulkanGraphicsPipelineCreateInfoBuilder::SetDefaults()
{
    Reset();

    {
        auto& inputAssemblyState = UseInputAssemblyState();
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    }

    {
        auto& rasterizationState = UseRasterizationState();
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationState.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationState.depthClampEnable = VK_FALSE;
        rasterizationState.rasterizerDiscardEnable = VK_FALSE;
        rasterizationState.depthBiasEnable = VK_FALSE;
        rasterizationState.lineWidth = 1.0f;
    }

    {
        auto& blendState = UseDefaultColorBlendState();
    }

    {
        auto& viewportState = UseViewportState();
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
    }

    {
        AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
        AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);
    }

    {
        auto& depthStencilState = UseDepthStencilState();
        depthStencilState.depthTestEnable = VK_TRUE;
        depthStencilState.depthWriteEnable = VK_TRUE;
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilState.depthBoundsTestEnable = VK_FALSE;
        depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
        depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
        depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
        depthStencilState.stencilTestEnable = VK_FALSE;
        depthStencilState.front = depthStencilState.back;
    }

    {
        auto& multisampleState = UseMultisampleState();
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    }
}

void VulkanGraphicsPipelineCreateInfoBuilder::CreatePipeline(VkPipeline* pipeline, VkPipelineCache cache, VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = vk.CreateGraphicsPipelines(vk.device, cache, 1, &this->graphicsPipelineCreateInfo, allocationCallbacks, pipeline);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create pipeline. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

VkPipelineVertexInputStateCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::UseVertexInputState()
{
    this->graphicsPipelineCreateInfo.pVertexInputState = &this->vertexInputState;
    return this->vertexInputState;
}

VkPipelineInputAssemblyStateCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::UseInputAssemblyState()
{
    this->graphicsPipelineCreateInfo.pInputAssemblyState = &this->inputAssemblyState;
    return this->inputAssemblyState;
}

VkPipelineTessellationStateCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::UseTessellationState()
{
    this->graphicsPipelineCreateInfo.pTessellationState = &this->tessellationState;
    return this->tessellationState;
}

VkPipelineViewportStateCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::UseViewportState()
{
    this->graphicsPipelineCreateInfo.pViewportState = &this->viewportState;
    return this->viewportState;
}

VkPipelineRasterizationStateCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::UseRasterizationState()
{
    this->graphicsPipelineCreateInfo.pRasterizationState = &this->rasterizationState;
    return this->rasterizationState;
}

VkPipelineMultisampleStateCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::UseMultisampleState()
{
    this->graphicsPipelineCreateInfo.pMultisampleState = &this->multisampleState;
    return this->multisampleState;
}

void VulkanGraphicsPipelineCreateInfoBuilder::SetMultisampleMask(uint32_t mask)
{
    this->multisampleMask = mask;
    this->multisampleState.pSampleMask = &this->multisampleMask;
}

VkPipelineDepthStencilStateCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::UseDepthStencilState()
{
    this->graphicsPipelineCreateInfo.pDepthStencilState = &this->depthStencilState;
    return this->depthStencilState;
}

VkPipelineColorBlendAttachmentState& VulkanGraphicsPipelineCreateInfoBuilder::GetDefaultBlendAttachmentState()
{
    static VkPipelineColorBlendAttachmentState defaultBlendAttachmentState = {};
    defaultBlendAttachmentState.colorWriteMask = 0xf;
    return defaultBlendAttachmentState;
}

VkPipelineColorBlendStateCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::UseDefaultColorBlendState()
{
    this->colorBlendState.attachmentCount = 1;
    this->colorBlendState.pAttachments = &GetDefaultBlendAttachmentState();

    this->graphicsPipelineCreateInfo.pColorBlendState = &this->colorBlendState;
    return this->colorBlendState;
}

VkPipelineColorBlendStateCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::UseColorBlendState()
{
    this->graphicsPipelineCreateInfo.pColorBlendState = &this->colorBlendState;
    return this->colorBlendState;
}

VkPipelineDynamicStateCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::UseDynamicState()
{
    this->graphicsPipelineCreateInfo.pDynamicState = &this->dynamicState;
    return this->dynamicState;
}

void VulkanGraphicsPipelineCreateInfoBuilder::AddDynamicState(VkDynamicState value)
{
    if (!this->dynamicStateValues.contains(value))
        this->dynamicStateValues.push_back(value);

    this->dynamicState.pDynamicStates = this->dynamicStateValues.data();
    this->dynamicState.dynamicStateCount = static_cast<uint32_t>(this->dynamicStateValues.size());

    this->graphicsPipelineCreateInfo.pDynamicState = &this->dynamicState;
}

VkPipelineShaderStageCreateInfo& VulkanGraphicsPipelineCreateInfoBuilder::AddShaderStage()
{
    this->stages.push_back();
    auto& stage = this->stages.back();

    stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

    this->graphicsPipelineCreateInfo.pStages = this->stages.data();
    this->graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(this->stages.size());

    return stage;
}

//VulkanDescriptorSetLayoutBuilder
void VulkanDescriptorSetLayoutBuilder::CreateDescriptorSetLayout(VkDescriptorSetLayout* descriptorSetLayout, VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    VulkanDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(this->bindings.data(), this->bindings.size());
    auto result = vk.CreateDescriptorSetLayout(vk.device, &descriptorSetLayoutCreateInfo, allocationCallbacks, descriptorSetLayout);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create descriptor set layout. Vulkan error: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanDescriptorSetLayoutBuilder::CreateDescriptorPool(VkDescriptorPool* descriptorPool, size_t maxSets, VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    StaticVector<VkDescriptorPoolSize, MAX_BINDINGS> poolSizes;
    poolSizes.resize(this->bindings.size());
    for (size_t bindingIndex = 0; bindingIndex < this->bindings.size(); bindingIndex++)
    {
        poolSizes[bindingIndex].type = this->bindings[bindingIndex].descriptorType;
        poolSizes[bindingIndex].descriptorCount = this->bindings[bindingIndex].descriptorCount;
    }

    VulkanDescriptorPoolCreateInfo descriptorPoolCreateInfo(poolSizes.data(), poolSizes.size());
    descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(maxSets);
    auto result = vk.CreateDescriptorPool(vk.device, &descriptorPoolCreateInfo, allocationCallbacks, descriptorPool);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create descriptor pool. Vulkan error: %1%", VulkanResult::ToString(result)));
        return;
    }
}

#endif
