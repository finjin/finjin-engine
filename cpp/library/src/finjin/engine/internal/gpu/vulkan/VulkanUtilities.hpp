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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Math.hpp"
#include "VulkanShaderType.hpp"
#include "VulkanTexture.hpp"
#include "VulkanGpuDescription.hpp"


//Macros------------------------------------------------------------------------
#if FINJIN_DEBUG
    #define FINJIN_CHECK_VKRESULT_FAILED(res) VulkanUtilities::CheckVkResultFailed(res)
#else
    #define FINJIN_CHECK_VKRESULT_FAILED(res) (res) != VK_SUCCESS
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class ASTCCompressionSubmode
    {
        SRGB,
        LINEAR
    };

    class VulkanUtilities
    {
    public:
        static bool CheckVkResultFailed(VkResult result);

        static bool IsDepthStencilFormat(VkFormat format);

        static bool IsBCFormat(VkFormat format);
        static bool IsETC2Format(VkFormat format);
        static bool IsASTCFormat(VkFormat format);

        static VkFormat ASTCBlockSizeToVulkanFormat(uint8_t xDim, uint8_t yDim, uint8_t zDim, ASTCCompressionSubmode submode);

        static VkFormat GLInternalFormatToVulkanFormat(uint32_t glInternalFormat);

        static VkShaderStageFlagBits ParseShaderStages(const Utf8StringView& value, Error& error);

        static void ParsePushConstantRangeOffsetAndSize(VkPushConstantRange& result, const Utf8StringView& value, Error& error);

        static VkImageViewType GetBestImageViewType(VkImageType imageType, bool isArray);

        static void SetImageLayout
            (
            VulkanDeviceFunctions& vk,
            VkCommandBuffer commandBuffer,
            VkImage image,
            VkImageLayout oldImageLayout,
            VkImageLayout newImageLayout,
            VkImageSubresourceRange subresourceRange,
            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
            );

        static void SetImageLayout
            (
            VulkanDeviceFunctions& vk,
            VkCommandBuffer commandBuffer,
            VkImage image,
            VkImageAspectFlags aspectMask,
            VkImageLayout oldImageLayout,
            VkImageLayout newImageLayout,
            VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT
            );
    };

    struct VulkanFenceCreateInfo : VkFenceCreateInfo
    {
        VulkanFenceCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        }
    };

    struct VulkanCommandPoolCreateInfo : VkCommandPoolCreateInfo
    {
        VulkanCommandPoolCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        }
    };

    struct VulkanCommandBufferAllocateInfo : VkCommandBufferAllocateInfo
    {
        VulkanCommandBufferAllocateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        }
    };

    struct VulkanSemaphoreCreateInfo : VkSemaphoreCreateInfo
    {
        VulkanSemaphoreCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        }
    };

    struct VulkanSubmitInfo : VkSubmitInfo
    {
        VulkanSubmitInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        }
    };

    struct VulkanRenderPassCreateInfo : VkRenderPassCreateInfo
    {
        VulkanRenderPassCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        }
    };

    struct VulkanFramebufferCreateInfo : VkFramebufferCreateInfo
    {
        VulkanFramebufferCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        }
    };

    struct VulkanSwapchainCreateInfoKHR : VkSwapchainCreateInfoKHR
    {
        VulkanSwapchainCreateInfoKHR()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        }
    };

    struct VulkanImageViewCreateInfo : VkImageViewCreateInfo
    {
        VulkanImageViewCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        }
    };

    struct VulkanImageCreateInfo : VkImageCreateInfo
    {
        VulkanImageCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        }
    };

    struct VulkanMemoryAllocateInfo : VkMemoryAllocateInfo
    {
        VulkanMemoryAllocateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        }
    };

    struct VulkanRenderPassBeginInfo : VkRenderPassBeginInfo
    {
        VulkanRenderPassBeginInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        }
    };

    struct VulkanDeviceCreateInfo : VkDeviceCreateInfo
    {
        VulkanDeviceCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        }
    };

    struct VulkanCommandBufferBeginInfo : VkCommandBufferBeginInfo
    {
        VulkanCommandBufferBeginInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        }

        VulkanCommandBufferBeginInfo(VkCommandBufferUsageFlags flags)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            this->flags = flags;
        }
    };

    struct VulkanPresentInfoKHR : VkPresentInfoKHR
    {
        VulkanPresentInfoKHR()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        }
    };

    struct VulkanImageMemoryBarrier : VkImageMemoryBarrier
    {
        VulkanImageMemoryBarrier()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            this->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            this->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        }
    };

    struct VulkanBufferMemoryBarrier : VkBufferMemoryBarrier
    {
        VulkanBufferMemoryBarrier()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
            this->srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            this->dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        }
    };

    struct VulkanMemoryBarrier : VkMemoryBarrier
    {
        VulkanMemoryBarrier()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        }
    };

    struct VulkanDeviceQueueCreateInfo : VkDeviceQueueCreateInfo
    {
        VulkanDeviceQueueCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        }

        VulkanDeviceQueueCreateInfo(uint32_t queueFamilyIndex, uint32_t queueCount, const float* queuePriorities)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            this->queueFamilyIndex = queueFamilyIndex;
            this->queueCount = queueCount;
            this->pQueuePriorities = queuePriorities;
        }

        operator VkDeviceQueueCreateInfo& ()
        {
            return *this;
        }
    };

    struct VulkanDescriptorImageInfo : VkDescriptorImageInfo
    {
        VulkanDescriptorImageInfo()
        {
            FINJIN_ZERO_ITEM(*this);
        }

        VulkanDescriptorImageInfo(VkSampler sampler, VkImageView imageView, VkImageLayout imageLayout)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sampler = sampler;
            this->imageView = imageView;
            this->imageLayout = imageLayout;
        }
    };

    struct VulkanWriteDescriptorSet : VkWriteDescriptorSet
    {
        VulkanWriteDescriptorSet()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        }
    };

    struct VulkanCopyDescriptorSet : VkCopyDescriptorSet
    {
        VulkanCopyDescriptorSet()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
        }
    };

    struct VulkanDescriptorSetAllocateInfo : VkDescriptorSetAllocateInfo
    {
        VulkanDescriptorSetAllocateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        }

        VulkanDescriptorSetAllocateInfo(VkDescriptorPool pool)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            this->descriptorPool = pool;
        }
    };

    struct VulkanPipelineLayoutCreateInfo : VkPipelineLayoutCreateInfo
    {
        VulkanPipelineLayoutCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        }

        VulkanPipelineLayoutCreateInfo(size_t setLayoutCount)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            this->setLayoutCount = static_cast<uint32_t>(setLayoutCount);
        }

        VulkanPipelineLayoutCreateInfo(const VkDescriptorSetLayout* pSetLayouts, size_t setLayoutCount)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            this->setLayoutCount = static_cast<uint32_t>(setLayoutCount);
            this->pSetLayouts = pSetLayouts;
        }
    };

    struct VulkanDescriptorSetLayoutCreateInfo : VkDescriptorSetLayoutCreateInfo
    {
        VulkanDescriptorSetLayoutCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        }

        VulkanDescriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutBinding* pBindings, size_t bindingCount)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            this->pBindings = pBindings;
            this->bindingCount = static_cast<uint32_t>(bindingCount);
        }
    };

    struct VulkanApplicationInfo : VkApplicationInfo
    {
        VulkanApplicationInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            this->pEngineName = FINJIN_ENGINE_NAME;
            this->engineVersion = VK_MAKE_VERSION(FINJIN_ENGINE_VERSION_MAJOR, FINJIN_ENGINE_VERSION_MINOR, FINJIN_ENGINE_VERSION_PATCH);
            this->apiVersion = VK_API_VERSION_1_0;
        }
    };

    struct VulkanInstanceCreateInfo : VkInstanceCreateInfo
    {
        VulkanInstanceCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        }
    };

    struct VulkanAttachmentReference : VkAttachmentReference
    {
        VulkanAttachmentReference()
        {
            FINJIN_ZERO_ITEM(*this);
        }

        VulkanAttachmentReference(uint32_t attachment, VkImageLayout layout)
        {
            this->attachment = attachment;
            this->layout = layout;
        }
    };

    struct VulkanComponentMapping : VkComponentMapping
    {
        VulkanComponentMapping()
        {
            FINJIN_ZERO_ITEM(*this);
        }
    };

    struct VulkanMappedMemoryRange : VkMappedMemoryRange
    {
        VulkanMappedMemoryRange()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        }

        VulkanMappedMemoryRange(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size)
        {
            FINJIN_ZERO_ITEM(*this);
            this->memory = memory;
            this->offset = offset;
            this->size = size;
        }
    };

    struct VulkanBufferCreateInfo : VkBufferCreateInfo
    {
        VulkanBufferCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        }
    };

    struct VulkanShaderModuleCreateInfo : VkShaderModuleCreateInfo
    {
        VulkanShaderModuleCreateInfo(const void* bytes, size_t byteCount)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            this->codeSize = static_cast<uint32_t>(byteCount); //This is specified in bytes
            this->pCode = static_cast<const uint32_t*>(bytes); //This is specified as a pointer to uint32_t
        }
    };

    struct VulkanPipelineVertexInputStateCreateInfo : VkPipelineVertexInputStateCreateInfo
    {
        VulkanPipelineVertexInputStateCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        }
    };

    struct VulkanGraphicsPipelineCreateInfo : VkGraphicsPipelineCreateInfo
    {
        VulkanGraphicsPipelineCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        }
    };

    struct VulkanPipelineShaderStageCreateInfo : VkPipelineShaderStageCreateInfo
    {
        VulkanPipelineShaderStageCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        }
    };

    struct VulkanDescriptorPoolSize : VkDescriptorPoolSize
    {
        VulkanDescriptorPoolSize(VkDescriptorType type, size_t descriptorCount)
        {
            FINJIN_ZERO_ITEM(*this);
            this->type = type;
            this->descriptorCount = static_cast<uint32_t>(descriptorCount);
        }
    };

    struct VulkanDescriptorPoolCreateInfo : VkDescriptorPoolCreateInfo
    {
        VulkanDescriptorPoolCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        }

        VulkanDescriptorPoolCreateInfo(const VkDescriptorPoolSize* pools, size_t count)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            this->poolSizeCount = static_cast<uint32_t>(count);
            this->pPoolSizes = pools;
        }
    };

    struct VulkanPipelineCacheCreateInfo : VkPipelineCacheCreateInfo
    {
        VulkanPipelineCacheCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        }
    };

    struct VulkanDescriptorBufferInfo : VkDescriptorBufferInfo
    {
        VulkanDescriptorBufferInfo()
        {
            FINJIN_ZERO_ITEM(*this);
        }

        VulkanDescriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE)
        {
            FINJIN_ZERO_ITEM(*this);
            this->buffer = buffer;
            this->offset = offset;
            this->range = range;
        }
    };

    struct VulkanGraphicsPipelineCreateInfoBuilder
    {
        VulkanGraphicsPipelineCreateInfoBuilder()
        {
            Reset();
        }

        VulkanGraphicsPipelineCreateInfoBuilder(const VulkanGraphicsPipelineCreateInfoBuilder& other)
        {
            operator = (other);
        }

        void operator = (const VulkanGraphicsPipelineCreateInfoBuilder& other)
        {
            //Custom assignment operator to update pointers as necessary
            FINJIN_COPY_MEMORY(this, &other, sizeof(VulkanGraphicsPipelineCreateInfoBuilder));
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

        void Reset()
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

        void SetDefault()
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

        VkPipelineVertexInputStateCreateInfo& UseVertexInputState()
        {
            this->graphicsPipelineCreateInfo.pVertexInputState = &this->vertexInputState;
            return this->vertexInputState;
        }

        VkPipelineInputAssemblyStateCreateInfo& UseInputAssemblyState()
        {
            this->graphicsPipelineCreateInfo.pInputAssemblyState = &this->inputAssemblyState;
            return this->inputAssemblyState;
        }

        VkPipelineTessellationStateCreateInfo& UseTessellationState()
        {
            this->graphicsPipelineCreateInfo.pTessellationState = &this->tessellationState;
            return this->tessellationState;
        }

        VkPipelineViewportStateCreateInfo& UseViewportState()
        {
            this->graphicsPipelineCreateInfo.pViewportState = &this->viewportState;
            return this->viewportState;
        }

        VkPipelineRasterizationStateCreateInfo& UseRasterizationState()
        {
            this->graphicsPipelineCreateInfo.pRasterizationState = &this->rasterizationState;
            return this->rasterizationState;
        }

        VkPipelineMultisampleStateCreateInfo& UseMultisampleState()
        {
            this->graphicsPipelineCreateInfo.pMultisampleState = &this->multisampleState;
            return this->multisampleState;
        }

        void SetMultisampleMask(uint32_t mask)
        {
            this->multisampleMask = mask;
            this->multisampleState.pSampleMask = &this->multisampleMask;
        }

        VkPipelineDepthStencilStateCreateInfo& UseDepthStencilState()
        {
            this->graphicsPipelineCreateInfo.pDepthStencilState = &this->depthStencilState;
            return this->depthStencilState;
        }

        static VkPipelineColorBlendAttachmentState& GetDefaultBlendAttachmentState()
        {
            static VkPipelineColorBlendAttachmentState defaultBlendAttachmentState = {};
            defaultBlendAttachmentState.colorWriteMask = 0xf;
            return defaultBlendAttachmentState;
        }

        VkPipelineColorBlendStateCreateInfo& UseDefaultColorBlendState()
        {
            this->colorBlendState.attachmentCount = 1;
            this->colorBlendState.pAttachments = &GetDefaultBlendAttachmentState();

            this->graphicsPipelineCreateInfo.pColorBlendState = &this->colorBlendState;
            return this->colorBlendState;
        }

        VkPipelineColorBlendStateCreateInfo& UseColorBlendState()
        {
            this->graphicsPipelineCreateInfo.pColorBlendState = &this->colorBlendState;
            return this->colorBlendState;
        }

        VkPipelineDynamicStateCreateInfo& UseDynamicState()
        {
            this->graphicsPipelineCreateInfo.pDynamicState = &this->dynamicState;
            return this->dynamicState;
        }

        void AddDynamicState(VkDynamicState value)
        {
            if (!this->dynamicStateValues.contains(value))
                this->dynamicStateValues.push_back(value);

            this->dynamicState.pDynamicStates = this->dynamicStateValues.data();
            this->dynamicState.dynamicStateCount = static_cast<uint32_t>(this->dynamicStateValues.size());

            this->graphicsPipelineCreateInfo.pDynamicState = &this->dynamicState;
        }

        VkPipelineShaderStageCreateInfo& AddShaderStage()
        {
            this->stages.push_back();
            auto& stage = this->stages.back();

            stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

            this->graphicsPipelineCreateInfo.pStages = this->stages.data();
            this->graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(this->stages.size());

            return stage;
        }

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

} }
