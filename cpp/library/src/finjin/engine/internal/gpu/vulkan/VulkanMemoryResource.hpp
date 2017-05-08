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
#include "VulkanGpuDescription.hpp"
#include "VulkanIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanMemoryResource
    {
    public:
        VulkanMemoryResource();

        void CreateBuffer
            (
            VulkanDeviceFunctions& vk,
            VkAllocationCallbacks* allocationCallbacks,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            void* data,
            VkDeviceSize size,
            VulkanGpuDescription& physicalDeviceDescription,
            Error& error
            );
        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        VkResult Map(VulkanDeviceFunctions& vk, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
        void Unmap(VulkanDeviceFunctions& vk);

        void CopyBufferTo(VulkanDeviceFunctions& vk, VkCommandBuffer commandBuffer, VulkanMemoryResource& destination, VkBufferCopy* copyRegion = nullptr);

        VkResult BindBuffer(VulkanDeviceFunctions& vk, VkDeviceSize offset = 0);
        VkResult BindImage(VulkanDeviceFunctions& vk, VkDeviceSize offset = 0);

        VkResult Flush(VulkanDeviceFunctions& vk, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
        VkResult FlushChanges(VulkanDeviceFunctions& vk, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);
        VkResult Invalidate(VulkanDeviceFunctions& vk, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

    public:
        VkImageView imageView; //Depending on scenario, might not be used
        VkImage image; //Depending on scenario, might not be used
        VkBuffer buffer; //Depending on scenario, might not be used
        VkDeviceMemory deviceMemory;
        void* mappedMemory;
        VkMemoryPropertyFlags bufferMemoryPropertyFlags; //Values passed to CreateBuffer()
    };

} }
