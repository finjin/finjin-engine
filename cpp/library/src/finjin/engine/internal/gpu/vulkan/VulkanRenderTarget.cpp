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

#include "VulkanRenderTarget.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//VulkanRenderTarget::Resource
VulkanRenderTarget::Resource::Resource()
{
    this->isImageOwned = true;
    this->image = VK_NULL_HANDLE;
    this->mem = VK_NULL_HANDLE;
    this->view = VK_NULL_HANDLE;
    this->isScreenSizeDependent = false;
}

void VulkanRenderTarget::Resource::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    if (this->view != VK_NULL_HANDLE)
    {
        vk.DestroyImageView(vk.device, this->view, allocationCallbacks);
        this->view = VK_NULL_HANDLE;
    }

    if (this->image != VK_NULL_HANDLE)
    {
        if (this->isImageOwned)
            vk.DestroyImage(vk.device, this->image, allocationCallbacks);
        this->image = VK_NULL_HANDLE;
    }

    if (this->mem != VK_NULL_HANDLE)
    {
        vk.FreeMemory(vk.device, this->mem, allocationCallbacks);
        this->mem = VK_NULL_HANDLE;
    }
}

void VulkanRenderTarget::Resource::DestroyScreenSizeDependentResources(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    if (this->isScreenSizeDependent)
    {
        if (this->view != VK_NULL_HANDLE)
        {
            vk.DestroyImageView(vk.device, this->view, allocationCallbacks);
            this->view = VK_NULL_HANDLE;
        }

        if (this->image != VK_NULL_HANDLE)
        {
            if (this->isImageOwned)
                vk.DestroyImage(vk.device, this->image, allocationCallbacks);
            this->image = VK_NULL_HANDLE;
        }

        if (this->mem != VK_NULL_HANDLE)
        {
            vk.FreeMemory(vk.device, this->mem, allocationCallbacks);
            this->mem = VK_NULL_HANDLE;
        }
    }
}

//VulkanRenderTarget
VulkanRenderTarget::VulkanRenderTarget() : clearColor(MathVector4(0, 0, 0, 0))
{
    this->vkframeBuffer = VK_NULL_HANDLE;
}

bool VulkanRenderTarget::IsScreenSizeDependent() const
{
    for (auto& colorOutput : this->colorOutputs)
    {
        if (colorOutput.isScreenSizeDependent)
            return true;
    }

    if (this->depthStencilResource.isScreenSizeDependent)
        return true;

    return false;
}

void VulkanRenderTarget::CreateColor
    (
    VkPhysicalDevice physicalDevice,
    const VulkanGpuDescription& physicalDeviceDescription,
    VulkanInstanceFunctions& vkInstance,
    VulkanDeviceFunctions& vk,
    VkAllocationCallbacks* allocationCallbacks,
    VkCommandBuffer graphicsCommandBuffer,
    size_t width,
    size_t height,
    VkFormat colorFormat,
    size_t multisampleCount,
    bool isScreenSizeDependent,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    for (auto& colorOutput : this->colorOutputs)
        colorOutput.Destroy(vk, allocationCallbacks);
    this->colorOutputs.resize(1);

    VulkanImageCreateInfo imageCreateInfo;

    VkFormatProperties formatProperties;
    vkInstance.GetPhysicalDeviceFormatProperties(physicalDevice, colorFormat, &formatProperties);
    if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    else if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT)
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

    //Fill in remaining image info
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = colorFormat;
    imageCreateInfo.extent.width = static_cast<uint32_t>(width);
    imageCreateInfo.extent.height = static_cast<uint32_t>(height);
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VulkanUtilities::GetSampleCountFlags(multisampleCount);
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    //Create
    auto result = vk.CreateImage(vk.device, &imageCreateInfo, allocationCallbacks, &this->colorOutputs[0].image);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create color image. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Allocate memory for the image----------------------------------
    VkMemoryRequirements memReqs;
    vk.GetImageMemoryRequirements(vk.device, this->colorOutputs[0].image, &memReqs);

    //Use the memory properties to determine the type of memory required
    VulkanMemoryAllocateInfo memAllocInfo;
    memAllocInfo.allocationSize = memReqs.size;
    if (!physicalDeviceDescription.GetMemoryTypeIndexFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memAllocInfo.memoryTypeIndex))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to determine color memory type. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    result = vk.AllocateMemory(vk.device, &memAllocInfo, allocationCallbacks, &this->colorOutputs[0].mem);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate color memory. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Bind image memory------------------------------------
    result = vk.BindImageMemory(vk.device, this->colorOutputs[0].image, this->colorOutputs[0].mem, 0);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to bind color memory. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Create color image view------------------------------
    VkImageAspectFlagBits colorImageAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    VulkanImageViewCreateInfo imageViewInfo(this->colorOutputs[0].image, colorFormat, colorImageAspectMask);
    VulkanUtilities::SetImageLayout(vk, graphicsCommandBuffer, this->colorOutputs[0].image, imageViewInfo.subresourceRange.aspectMask, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    result = vk.CreateImageView(vk.device, &imageViewInfo, allocationCallbacks, &this->colorOutputs[0].view);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create color image view. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    this->colorOutputs[0].isScreenSizeDependent = isScreenSizeDependent;
}

void VulkanRenderTarget::CreateDepthStencil
    (
    VkPhysicalDevice physicalDevice,
    const VulkanGpuDescription& physicalDeviceDescription,
    VulkanInstanceFunctions& vkInstance,
    VulkanDeviceFunctions& vk,
    VkAllocationCallbacks* allocationCallbacks,
    VkCommandBuffer graphicsCommandBuffer,
    size_t width,
    size_t height,
    VkFormat depthStencilFormat,
    size_t multisampleCount,
    bool isScreenSizeDependent,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    this->depthStencilResource.isScreenSizeDependent = isScreenSizeDependent;

    VulkanImageCreateInfo imageCreateInfo;

    VkFormatProperties formatProperties;
    vkInstance.GetPhysicalDeviceFormatProperties(physicalDevice, depthStencilFormat, &formatProperties);
    if (formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        imageCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    else if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
        imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

    //Fill in remaining image info
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = depthStencilFormat;
    imageCreateInfo.extent.width = static_cast<uint32_t>(width);
    imageCreateInfo.extent.height = static_cast<uint32_t>(height);
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VulkanUtilities::GetSampleCountFlags(multisampleCount);
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    //Create
    auto result = vk.CreateImage(vk.device, &imageCreateInfo, allocationCallbacks, &this->depthStencilResource.image);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create depth image. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Allocate memory for the image----------------------------------
    VkMemoryRequirements memReqs;
    vk.GetImageMemoryRequirements(vk.device, this->depthStencilResource.image, &memReqs);

    //Use the memory properties to determine the type of memory required
    VulkanMemoryAllocateInfo memAllocInfo;
    memAllocInfo.allocationSize = memReqs.size;
    if (!physicalDeviceDescription.GetMemoryTypeIndexFromProperties(memReqs.memoryTypeBits, 0, memAllocInfo.memoryTypeIndex))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to determine depth memory type. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    result = vk.AllocateMemory(vk.device, &memAllocInfo, allocationCallbacks, &this->depthStencilResource.mem);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate depth memory. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Bind image memory------------------------------------
    result = vk.BindImageMemory(vk.device, this->depthStencilResource.image, this->depthStencilResource.mem, 0);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to bind depth memory. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Create depth image view------------------------------
    VkImageAspectFlagBits depthImageAspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    if (VulkanUtilities::IsDepthStencilFormat(depthStencilFormat))
        depthImageAspectMask = VkImageAspectFlagBits(depthImageAspectMask | VK_IMAGE_ASPECT_STENCIL_BIT);
    VulkanImageViewCreateInfo imageViewInfo(this->depthStencilResource.image, depthStencilFormat, depthImageAspectMask);
    VulkanUtilities::SetImageLayout(vk, graphicsCommandBuffer, this->depthStencilResource.image, imageViewInfo.subresourceRange.aspectMask, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    result = vk.CreateImageView(vk.device, &imageViewInfo, allocationCallbacks, &this->depthStencilResource.view);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create depth image view. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanRenderTarget::CreateFrameBuffer(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, VulkanFramebufferCreateInfo& frameBufferCreateInfo, size_t colorViewIndex, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //The pAttachments member is const, but in this case it's safe to modify it
    const_cast<VkImageView&>(frameBufferCreateInfo.pAttachments[colorViewIndex]) = this->colorOutputs[0].view;

    auto result = vk.CreateFramebuffer(vk.device, &frameBufferCreateInfo, allocationCallbacks, &this->vkframeBuffer);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create frame buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanRenderTarget::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    if (this->vkframeBuffer != VK_NULL_HANDLE)
    {
        vk.DestroyFramebuffer(vk.device, this->vkframeBuffer, allocationCallbacks);
        this->vkframeBuffer = VK_NULL_HANDLE;
    }

    for (auto& colorOutput : this->colorOutputs)
        colorOutput.Destroy(vk, allocationCallbacks);
    this->colorOutputs.clear();

    this->depthStencilResource.Destroy(vk, allocationCallbacks);
}

void VulkanRenderTarget::DestroyScreenSizeDependentResources(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    if (IsScreenSizeDependent())
    {
        if (this->vkframeBuffer != VK_NULL_HANDLE)
        {
            vk.DestroyFramebuffer(vk.device, this->vkframeBuffer, allocationCallbacks);
            this->vkframeBuffer = VK_NULL_HANDLE;
        }
    }

    for (auto& colorOutput : this->colorOutputs)
        colorOutput.DestroyScreenSizeDependentResources(vk, allocationCallbacks);

    this->depthStencilResource.DestroyScreenSizeDependentResources(vk, allocationCallbacks);
}

#endif
