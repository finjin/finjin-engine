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
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "VulkanQueueFamilyIndexes.hpp"
#include "VulkanRenderTarget.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanGpuContext;
    class VulkanGpuContextImpl;

    class VulkanFrameBuffer
    {
    public:
        struct GraphicsCommandBuffer
        {
            GraphicsCommandBuffer()
            {
                this->waitForFence = false;
                this->fence = VK_NULL_HANDLE;
                this->commandBuffer = VK_NULL_HANDLE;
            }

            bool waitForFence;
            VkFence fence;
            VkCommandBuffer commandBuffer;
        };

    public:
        VulkanFrameBuffer();

        void SetIndex(size_t index);

        void CreateCommandBuffers(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, const VulkanQueueFamilyIndexes& queueFamilyIndexes, size_t maxGpuCommandListsPerStage, Allocator* allocator, Error& error);
        void CreateScreenCaptureBuffer(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, const VulkanGpuDescription& physicalDeviceDescription, size_t byteCount, bool isScreenSizeDependent, Error& error);

        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);
        void DestroyScreenSizeDependentResources(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        GraphicsCommandBuffer* NewGraphicsCommandBuffer(VulkanDeviceFunctions& vk, Error& error);
        GraphicsCommandBuffer* GetCurrentGraphicsCommandBuffer();
        void SubmitCommandBuffers(VulkanDeviceFunctions& vk, VkQueue queue, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, Error& error);
        void WaitForCommandBuffers(VulkanDeviceFunctions& vk);
        void ResetCommandBuffers();

        void WaitForNotInUse();

    public:
        size_t index;
        std::atomic_size_t commandBufferWaitIndex;
        std::atomic_size_t commandBufferCommitIndex;

        VulkanRenderTarget renderTarget;

        VkSemaphore renderingCompleteSemaphore;

        VkCommandPool graphicsCommandPool;
        DynamicVector<GraphicsCommandBuffer> freeGraphicsCommandBuffers;
        DynamicVector<GraphicsCommandBuffer*> graphicsCommandBuffersToExecute;
        DynamicVector<VkCommandBuffer> plainCommandBuffers;

        VulkanMemoryResource screenCaptureBuffer;
        std::array<uint32_t, 2> screenCaptureSize;
        bool screenCaptureRequested;
        bool isScreenCaptureScreenSizeDependent;
    };

} }
