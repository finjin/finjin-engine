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


#pragma once


//Includes----------------------------------------------------------------------
#include "finjin/common/ValueOrError.hpp"
#include "VulkanUtilities.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanGraphicsPipelineCreateInfoBuilder
    {
    public:
        VulkanGraphicsPipelineCreateInfoBuilder();
        VulkanGraphicsPipelineCreateInfoBuilder(const VulkanGraphicsPipelineCreateInfoBuilder& other);

        void operator = (const VulkanGraphicsPipelineCreateInfoBuilder& other);

        void Reset();

        void SetDefaults();

        void CreatePipeline(VkPipeline* result, VkPipelineCache cache, VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, Error& error);

        VkPipelineVertexInputStateCreateInfo& UseVertexInputState();
        VkPipelineInputAssemblyStateCreateInfo& UseInputAssemblyState();
        VkPipelineTessellationStateCreateInfo& UseTessellationState();
        VkPipelineViewportStateCreateInfo& UseViewportState();
        VkPipelineRasterizationStateCreateInfo& UseRasterizationState();
        VkPipelineMultisampleStateCreateInfo& UseMultisampleState();
        void SetMultisampleMask(uint32_t mask);
        VkPipelineDepthStencilStateCreateInfo& UseDepthStencilState();

        static VkPipelineColorBlendAttachmentState& GetDefaultBlendAttachmentState();

        VkPipelineColorBlendStateCreateInfo& UseDefaultColorBlendState();
        VkPipelineColorBlendStateCreateInfo& UseColorBlendState();
        VkPipelineDynamicStateCreateInfo& UseDynamicState();

        void AddDynamicState(VkDynamicState value);

        VkPipelineShaderStageCreateInfo& AddShaderStage();

    public:
        VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;

        VkPipelineVertexInputStateCreateInfo vertexInputState;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
        VkPipelineTessellationStateCreateInfo tessellationState;
        VkPipelineViewportStateCreateInfo viewportState;
        VkPipelineRasterizationStateCreateInfo rasterizationState;
        VkPipelineMultisampleStateCreateInfo multisampleState;
        VkPipelineDepthStencilStateCreateInfo depthStencilState;
        VkPipelineColorBlendStateCreateInfo colorBlendState;
        VkPipelineDynamicStateCreateInfo dynamicState;

        StaticVector<VkDynamicState, 16> dynamicStateValues;
        StaticVector<VkPipelineShaderStageCreateInfo, (size_t)VulkanShaderType::COUNT> stages;
        VkSampleMask multisampleMask; //For multisampleState.pSampleMask

        VkPipelineColorBlendAttachmentState defaultBlendAttachmentState;
    };

    class VulkanDescriptorSetLayoutBuilder
    {
    public:
        template <typename BindingIndex>
        ValueOrError<void> AddBinding(BindingIndex bindingIndex, uint32_t bindingID, VkDescriptorType descriptorType, size_t descriptorCount, VkShaderStageFlagBits stageFlags)
        {
            assert(static_cast<size_t>(bindingIndex) == this->bindings.size());

            if (static_cast<size_t>(bindingIndex) != this->bindings.size())
                return ValueOrError<void>::CreateError();

            if (!this->bindings.push_back())
                return ValueOrError<void>::CreateError();

            auto& binding = this->bindings.back();
            FINJIN_ZERO_MEMORY(&binding, sizeof(VkDescriptorSetLayoutBinding));

            binding.binding = bindingID;
            binding.descriptorType = descriptorType;
            binding.descriptorCount = static_cast<uint32_t>(descriptorCount);
            binding.stageFlags = stageFlags;
            //binding.pImmutableSamplers = nullptr;

            return ValueOrError<void>();
        }

        void CreateDescriptorSetLayout(VkDescriptorSetLayout* result, VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, Error& error);
        void CreateDescriptorPool(VkDescriptorPool* result, size_t maxSets, VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, Error& error);

    public:
        enum { MAX_BINDINGS = 16 };

        StaticVector<VkDescriptorSetLayoutBinding, MAX_BINDINGS> bindings;
    };

} }
