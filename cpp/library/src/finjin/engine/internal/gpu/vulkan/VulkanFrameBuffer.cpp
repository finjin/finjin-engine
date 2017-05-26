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
    this->commandBufferWaitIndex = 0;
    this->commandBufferCommitIndex = 0;

    this->renderingCompleteSemaphore = VK_NULL_HANDLE;

    this->graphicsCommandPool = VK_NULL_HANDLE;
}

void VulkanFrameBuffer::SetIndex(size_t index)
{
    this->index = index;
}

void VulkanFrameBuffer::CreateCommandBuffers(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, const VulkanQueueFamilyIndexes& queueFamilyIndexes, size_t maxGpuCommandListsPerStage, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Create command pool--------------------------
    VulkanCommandPoolCreateInfo commandPoolInfo(queueFamilyIndexes.graphics, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    auto result = vk.CreateCommandPool(vk.device, &commandPoolInfo, allocationCallbacks, &this->graphicsCommandPool);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create command pool. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Create command buffers---------------------
    if (!this->freeGraphicsCommandBuffers.Create(maxGpuCommandListsPerStage, allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create frame buffer graphics command buffers collection.");
        return;
    }

    if (!this->graphicsCommandBuffersToExecute.Create(maxGpuCommandListsPerStage, allocator, nullptr))
    {
        FINJIN_SET_ERROR(error, "Failed to create frame buffer 'all command buffers to execute' collection.");
        return;
    }

    if (!this->plainCommandBuffers.Create(maxGpuCommandListsPerStage, allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create frame buffer plain graphics command buffers collection.");
        return;
    }

    VulkanCommandBufferAllocateInfo commandBufferInfo;
    commandBufferInfo.commandPool = this->graphicsCommandPool;
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandBufferCount = static_cast<uint32_t>(this->plainCommandBuffers.size());
    result = vk.AllocateCommandBuffers(vk.device, &commandBufferInfo, this->plainCommandBuffers.data());
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create frame buffer command buffers. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Do some one time initialization on the command buffers
    for (size_t commandBufferIndex = 0; commandBufferIndex < this->freeGraphicsCommandBuffers.size(); commandBufferIndex++)
    {
        auto& commandBufferHandle = this->freeGraphicsCommandBuffers[commandBufferIndex];

        //Put the created command buffers into their corresponding "handle"
        commandBufferHandle.commandBuffer = this->plainCommandBuffers[commandBufferIndex];

        //Create the completed fence for the command buffer
        VulkanFenceCreateInfo fenceInfo;
        result = vk.CreateFence(vk.device, &fenceInfo, allocationCallbacks, &commandBufferHandle.fence);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create fence. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }
    this->plainCommandBuffers.clear();

    ResetCommandBuffers();

    //Create rendering complete semaphore--------------------
    VulkanSemaphoreCreateInfo semaphoreCreateInfo;
    result = vk.CreateSemaphore(vk.device, &semaphoreCreateInfo, allocationCallbacks, &this->renderingCompleteSemaphore);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create rendering complete semaphore. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }
}

void VulkanFrameBuffer::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    this->renderTarget.Destroy(vk, allocationCallbacks);

    if (this->renderingCompleteSemaphore != VK_NULL_HANDLE)
    {
        vk.DestroySemaphore(vk.device, this->renderingCompleteSemaphore, allocationCallbacks);
        this->renderingCompleteSemaphore = VK_NULL_HANDLE;
    }

    this->freeGraphicsCommandBuffers.maximize();
    for (auto& commandBufferHandle : this->freeGraphicsCommandBuffers)
    {
        if (commandBufferHandle.fence != VK_NULL_HANDLE)
            vk.DestroyFence(vk.device, commandBufferHandle.fence, allocationCallbacks);
    }
    this->freeGraphicsCommandBuffers.Destroy();

    this->graphicsCommandBuffersToExecute.Destroy();

    this->plainCommandBuffers.Destroy();

    if (this->graphicsCommandPool != VK_NULL_HANDLE)
    {
        vk.DestroyCommandPool(vk.device, this->graphicsCommandPool, allocationCallbacks);
        this->graphicsCommandPool = VK_NULL_HANDLE;
    }
}

void VulkanFrameBuffer::DestroyScreenSizeDependentResources(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    this->renderTarget.DestroyScreenSizeDependentResources(vk, allocationCallbacks);
}

VulkanFrameBuffer::GraphicsCommandBuffer* VulkanFrameBuffer::NewGraphicsCommandBuffer(VulkanDeviceFunctions& vk, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Clear command pool if it's the first command buffer
    if (this->freeGraphicsCommandBuffers.empty())
    {
        auto result = vk.ResetCommandPool(vk.device, this->graphicsCommandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to reset command buffer. Vulkan result: %1%", VulkanResult::ToString(result)));
            return nullptr;
        }
    }

    if (!this->freeGraphicsCommandBuffers.push_back())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to obtained a free graphics command buffer."));
        return nullptr;
    }

    auto& commandBuffer = this->freeGraphicsCommandBuffers.back();
    this->graphicsCommandBuffersToExecute.push_back(&commandBuffer);

    VulkanCommandBufferBeginInfo commandBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    auto result = vk.BeginCommandBuffer(commandBuffer.commandBuffer, &commandBeginInfo);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to start command buffer for population. Vulkan result: %1%", VulkanResult::ToString(result)));
        return nullptr;
    }

    return &commandBuffer;
}

VulkanFrameBuffer::GraphicsCommandBuffer* VulkanFrameBuffer::GetCurrentGraphicsCommandBuffer()
{
    return !this->graphicsCommandBuffersToExecute.empty() ? this->graphicsCommandBuffersToExecute.back() : nullptr;
}

void VulkanFrameBuffer::SubmitCommandBuffers(VulkanDeviceFunctions& vk, VkQueue queue, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->commandBufferCommitIndex < this->graphicsCommandBuffersToExecute.size())
    {
        GraphicsCommandBuffer* lastCommandBuffer = nullptr;

        this->plainCommandBuffers.clear();
        for (; this->commandBufferCommitIndex < this->graphicsCommandBuffersToExecute.size(); this->commandBufferCommitIndex++)
        {
            auto commandBufferHandle = this->graphicsCommandBuffersToExecute[this->commandBufferCommitIndex];
            this->plainCommandBuffers.push_back(commandBufferHandle->commandBuffer);

            commandBufferHandle->waitForFence = this->commandBufferCommitIndex == this->graphicsCommandBuffersToExecute.size() - 1;
            if (commandBufferHandle->waitForFence)
            {
                vk.ResetFences(vk.device, 1, &commandBufferHandle->fence);
                lastCommandBuffer = commandBufferHandle;
            }
        }

        VulkanSubmitInfo submitInfo;
        VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfo.pWaitDstStageMask = &pipelineStageFlags;
        submitInfo.commandBufferCount = static_cast<uint32_t>(this->plainCommandBuffers.size());
        submitInfo.pCommandBuffers = this->plainCommandBuffers.data();
        if (waitSemaphore != VK_NULL_HANDLE)
        {
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &waitSemaphore;
        }
        if (signalSemaphore != VK_NULL_HANDLE)
        {
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &signalSemaphore;
        }

        auto result = vk.QueueSubmit(queue, 1, &submitInfo, lastCommandBuffer->fence);
        if (FINJIN_CHECK_VKRESULT_FAILED(result))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to submit queue. Vulkan result: %1%", VulkanResult::ToString(result)));
            return;
        }
    }
}

void VulkanFrameBuffer::WaitForCommandBuffers(VulkanDeviceFunctions& vk)
{
    for (; this->commandBufferWaitIndex < this->commandBufferCommitIndex; this->commandBufferWaitIndex++)
    {
        auto commandBuffer = this->graphicsCommandBuffersToExecute[this->commandBufferWaitIndex];
        if (commandBuffer->waitForFence)
        {
            auto result = vk.WaitForFences(vk.device, 1, &commandBuffer->fence, VK_TRUE, UINT64_MAX);
            if (FINJIN_CHECK_VKRESULT_FAILED(result))
            {
            }
        }
    }
}

void VulkanFrameBuffer::ResetCommandBuffers()
{
    this->freeGraphicsCommandBuffers.clear();
    this->graphicsCommandBuffersToExecute.clear();

    this->commandBufferWaitIndex = 0;
    this->commandBufferCommitIndex = 0;
}

void VulkanFrameBuffer::WaitForNotInUse()
{
    while (!this->graphicsCommandBuffersToExecute.empty())
    {
        //Spin
    }
}

#endif
