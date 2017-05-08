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

#include "VulkanFrameBuffer.hpp"
#include "VulkanGpuContext.hpp"
#include "VulkanGpuContextImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanFrameBuffer::VulkanFrameBuffer()
{
    this->index = 0;
    this->frameBuffer = VK_NULL_HANDLE;

    this->renderingCompleteFence = VK_NULL_HANDLE;

    this->renderingCompleteSemaphore = VK_NULL_HANDLE;

    this->graphicsCommandPool = VK_NULL_HANDLE;
}

void VulkanFrameBuffer::SetIndex(size_t index)
{
    this->index = index;

    this->staticMeshRendererFrameState.index = index;
}

VkCommandBuffer VulkanFrameBuffer::NewGraphicsCommandBuffer()
{
    if (!this->graphicsCommandBuffers.push_back())
        return VK_NULL_HANDLE;

    auto result = this->graphicsCommandBuffers.back();
    this->commandBuffersToExecute.push_back(result);
    return result;
}

VkCommandBuffer VulkanFrameBuffer::GetCurrentGraphicsCommandBuffer()
{
    return !this->graphicsCommandBuffers.empty() ? this->graphicsCommandBuffers.back() : VK_NULL_HANDLE;
}

void VulkanFrameBuffer::ResetCommandBuffers()
{
    this->graphicsCommandBuffers.clear();
    this->commandBuffersToExecute.clear();
}

void VulkanFrameBuffer::ResetFences(VulkanGpuContextImpl* contextImpl)
{
    auto result = contextImpl->vk.ResetFences(contextImpl->vk.device, 1, &this->renderingCompleteFence);
    assert(result == VK_SUCCESS);
}

#endif
