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

#include "VulkanSimpleWork.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanSimpleWork::VulkanSimpleWork()
{
    this->commandPool = VK_NULL_HANDLE;
    this->commandBuffer = VK_NULL_HANDLE;
    this->fence = VK_NULL_HANDLE;
    this->queue = VK_NULL_HANDLE;
}

void VulkanSimpleWork::Create(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, VkQueue queue, uint32_t queueFamilyIndex, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    {
        //Pool
        VulkanCommandPoolCreateInfo commandPoolInfo;
        commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        auto result = vk.CreateCommandPool(vk.device, &commandPoolInfo, allocationCallbacks, &this->commandPool);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create command pool. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }
    {
        //Buffer
        VulkanCommandBufferAllocateInfo commandBufferInfo;
        commandBufferInfo.commandPool = this->commandPool;
        commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferInfo.commandBufferCount = 1;
        auto result = vk.AllocateCommandBuffers(vk.device, &commandBufferInfo, &this->commandBuffer);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }
    {
        //Fence
        VulkanFenceCreateInfo tempFenceInfo;
        auto result = vk.CreateFence(vk.device, &tempFenceInfo, allocationCallbacks, &this->fence);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create fence. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }

    this->queue = queue;
}

void VulkanSimpleWork::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    this->commandBuffer = VK_NULL_HANDLE;

    if (this->commandPool != VK_NULL_HANDLE)
    {
        vk.DestroyCommandPool(vk.device, this->commandPool, allocationCallbacks);
        this->commandPool = VK_NULL_HANDLE;
    }

    if (this->fence != VK_NULL_HANDLE)
    {
        vk.DestroyFence(vk.device, this->fence, allocationCallbacks);
        this->fence = VK_NULL_HANDLE;
    }

    this->queue = VK_NULL_HANDLE;
}

void VulkanSimpleWork::Start(VulkanDeviceFunctions& vk, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Clear command pool
    auto result = vk.ResetCommandPool(vk.device, this->commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to reset command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Execute begin
    VulkanCommandBufferBeginInfo commandBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    result = vk.BeginCommandBuffer(this->commandBuffer, &commandBeginInfo);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to execute command buffer begin. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanSimpleWork::Finish(VulkanDeviceFunctions& vk, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = vk.EndCommandBuffer(this->commandBuffer);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to execute end command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    {
        vk.ResetFences(vk.device, 1, &this->fence);

        //Submit command buffer for execution
        VulkanSubmitInfo submitInfo;
        VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        submitInfo.pWaitDstStageMask = &pipelineStageFlags;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &this->commandBuffer;
        result = vk.QueueSubmit(this->queue, 1, &submitInfo, this->fence);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to submit queue for submit/wait. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }

        //Wait for submitted command buffer to finish
        result = vk.WaitForFences(vk.device, 1, &this->fence, VK_TRUE, UINT64_MAX);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to wait for fence for submit/wait. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }
}

#endif
