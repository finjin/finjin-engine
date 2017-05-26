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
#include "finjin/common/RequestedValue.hpp"
#include "VulkanShaderType.hpp"
#include "VulkanTexture.hpp"
#include "VulkanGpuDescription.hpp"


//Macros------------------------------------------------------------------------
#if FINJIN_DEBUG
    #define FINJIN_CHECK_VKRESULT_FAILED(res) VulkanUtilities::CheckVkResultFailed(res)
#else
    #define FINJIN_CHECK_VKRESULT_FAILED(res) (res) != VK_SUCCESS
#endif

#define FINJIN_VULKAN_DEFAULT_SHADER_ENTRY_POINT_NAME "main"


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

        static VkSampleCountFlagBits GetSampleCountFlags(size_t sampleCount);

        static bool GetBestDepthStencilFormat(RequestedValue<VkFormat>& depthStencilFormat, bool stencilRequired, VulkanInstanceFunctions& vk, VkPhysicalDevice physicalDevice);
    };

    struct VulkanFenceCreateInfo : VkFenceCreateInfo
    {
        VulkanFenceCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        }

        VulkanFenceCreateInfo(VkFenceCreateFlags flags)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

            this->flags = flags;
        }
    };

    struct VulkanCommandPoolCreateInfo : VkCommandPoolCreateInfo
    {
        VulkanCommandPoolCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        }

        VulkanCommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

            this->queueFamilyIndex = queueFamilyIndex;
            this->flags = flags;
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

        VulkanImageViewCreateInfo(VkImage image, VkFormat format, VkImageAspectFlagBits aspectMask)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

            this->format = format;
            this->subresourceRange.aspectMask = aspectMask;
            this->subresourceRange.baseMipLevel = 0;
            this->subresourceRange.levelCount = 1;
            this->subresourceRange.baseArrayLayer = 0;
            this->subresourceRange.layerCount = 1;
            this->viewType = VK_IMAGE_VIEW_TYPE_2D;
            this->image = image;
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

    struct VulkanSamplerCreateInfo : VkSamplerCreateInfo
    {
        VulkanSamplerCreateInfo()
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        }

        VulkanSamplerCreateInfo(VkFilter filter, VkSamplerAddressMode addressMode)
        {
            FINJIN_ZERO_ITEM(*this);
            this->sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

            this->magFilter = filter;
            this->minFilter = filter;
            this->mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            this->addressModeU = addressMode;
            this->addressModeV = addressMode;
            this->addressModeW = addressMode;
            this->mipLodBias = 0.0;
            this->anisotropyEnable = VK_FALSE;
            this->maxAnisotropy = 1.0f;
            this->compareOp = VK_COMPARE_OP_NEVER;
            this->minLod = 0.0;
            this->maxLod = 0.0; //FLT_MAX
            this->compareEnable = VK_FALSE;
        }
    };

} }
