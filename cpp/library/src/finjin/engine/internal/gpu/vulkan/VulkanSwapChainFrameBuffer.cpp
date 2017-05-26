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

#include "VulkanSwapChainFrameBuffer.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanSwapChainFrameBuffer::VulkanSwapChainFrameBuffer()
{
    this->image = VK_NULL_HANDLE;
    this->view = VK_NULL_HANDLE;
    this->vkframeBuffer = VK_NULL_HANDLE;
}

void VulkanSwapChainFrameBuffer::SetImageAndCreateView(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, VkImage image, VkFormat colorFormat, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->image = image;

    VulkanImageViewCreateInfo colorImageViewInfo(this->image, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    auto result = vk.CreateImageView(vk.device, &colorImageViewInfo, allocationCallbacks, &this->view);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create swap chain image view. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanSwapChainFrameBuffer::CreateFrameBuffer(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, VulkanFramebufferCreateInfo& frameBufferCreateInfo, size_t colorViewIndex, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //The pAttachments member is const, but in this case it's safe to modify it
    const_cast<VkImageView&>(frameBufferCreateInfo.pAttachments[colorViewIndex]) = this->view;

    auto result = vk.CreateFramebuffer(vk.device, &frameBufferCreateInfo, allocationCallbacks, &this->vkframeBuffer);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create frame buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanSwapChainFrameBuffer::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    if (this->vkframeBuffer != VK_NULL_HANDLE)
    {
        vk.DestroyFramebuffer(vk.device, this->vkframeBuffer, allocationCallbacks);
        this->vkframeBuffer = VK_NULL_HANDLE;
    }

    if (this->view != VK_NULL_HANDLE)
    {
        vk.DestroyImageView(vk.device, this->view, allocationCallbacks);
        this->view = VK_NULL_HANDLE;
    }

    this->image = VK_NULL_HANDLE; //Image should not be destroyed since it's owned by swap chain
}

#endif
