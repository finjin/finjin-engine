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

#include "VulkanMemoryResource.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanMemoryResource::VulkanMemoryResource()
{
    this->imageView = VK_NULL_HANDLE;
    this->image = VK_NULL_HANDLE;
    this->buffer = VK_NULL_HANDLE;
    this->deviceMemory = VK_NULL_HANDLE;
    this->mappedMemory = nullptr;
    this->bufferMemoryPropertyFlags = 0;
}

void VulkanMemoryResource::CreateBuffer
    (
    VulkanDeviceFunctions& vk,
    VkAllocationCallbacks* allocationCallbacks,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    void* data,
    VkDeviceSize size,
    VulkanGpuDescription& physicalDeviceDescription,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->buffer != VK_NULL_HANDLE)
    {
        FINJIN_SET_ERROR(error, "Buffer already in use.");
        return;
    }
    if (this->image != VK_NULL_HANDLE)
    {
        FINJIN_SET_ERROR(error, "Buffer unable to be created. Image already in use.");
        return;
    }

    //Create the buffer handle
    VulkanBufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usageFlags;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    auto result = vk.CreateBuffer(vk.device, &bufferCreateInfo, allocationCallbacks, &this->buffer);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create buffer. Vulkan error: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Create the memory backing up the buffer handle
    VkMemoryRequirements memReqs;
    vk.GetBufferMemoryRequirements(vk.device, this->buffer, &memReqs);
    VulkanMemoryAllocateInfo memAlloc;
    memAlloc.allocationSize = memReqs.size;

    //Find a memory type index that fits the properties of the buffer
    if (!physicalDeviceDescription.GetMemoryTypeIndexFromProperties(memReqs.memoryTypeBits, memoryPropertyFlags, memAlloc.memoryTypeIndex))
    {
        FINJIN_SET_ERROR(error, "Failed to find memory type index.");
        return;
    }
    result = vk.AllocateMemory(vk.device, &memAlloc, allocationCallbacks, &this->deviceMemory);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate buffer memory. Vulkan error: %1%", VulkanResult::ToString(result)));
        return;
    }

    //Copy the data if necessary
    if (data != nullptr)
    {
        result = Map(vk);
        if (result != VK_SUCCESS)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to map buffer memory. Vulkan error: %1%", VulkanResult::ToString(result)));
            return;
        }

        FINJIN_COPY_MEMORY(this->mappedMemory, data, size);

        //If host coherency hasn't been requested, do a manual flush to make writes visible
        if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
        {
            VulkanMappedMemoryRange mappedRange;
            mappedRange.memory = this->deviceMemory;
            mappedRange.offset = 0;
            mappedRange.size = size;
            vk.FlushMappedMemoryRanges(vk.device, 1, &mappedRange);
        }

        Unmap(vk);
    }

    //Attach the memory to the buffer object
    result = BindBuffer(vk);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to bind buffer memory. Vulkan error: %1%", VulkanResult::ToString(result)));
        return;
    }

    this->bufferMemoryPropertyFlags = memoryPropertyFlags;
}

void VulkanMemoryResource::CopyBufferTo(VulkanDeviceFunctions& vk, VkCommandBuffer commandBuffer, VulkanMemoryResource& destination, VkBufferCopy* copyRegion)
{
    assert(this->buffer && destination.buffer);

    VkBufferCopy bufferCopy = {};
    if (copyRegion != nullptr)
        bufferCopy = *copyRegion;
    else
    {
        VkMemoryRequirements memReqs;
        vk.GetBufferMemoryRequirements(vk.device, this->buffer, &memReqs);
        bufferCopy.size = memReqs.size;
    }

    vk.CmdCopyBuffer(commandBuffer, this->buffer, destination.buffer, 1, &bufferCopy);
}

void VulkanMemoryResource::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    Unmap(vk);

    if (this->imageView != VK_NULL_HANDLE)
    {
        vk.DestroyImageView(vk.device, this->imageView, allocationCallbacks);
        this->imageView = VK_NULL_HANDLE;
    }

    if (this->image != VK_NULL_HANDLE)
    {
        vk.DestroyImage(vk.device, this->image, allocationCallbacks);
        this->image = VK_NULL_HANDLE;
    }

    if (this->buffer != VK_NULL_HANDLE)
    {
        vk.DestroyBuffer(vk.device, this->buffer, allocationCallbacks);
        this->buffer = VK_NULL_HANDLE;
    }

    if (this->deviceMemory != VK_NULL_HANDLE)
    {
        vk.FreeMemory(vk.device, this->deviceMemory, allocationCallbacks);
        this->deviceMemory = VK_NULL_HANDLE;
    }

    this->bufferMemoryPropertyFlags = 0;
}

VkResult VulkanMemoryResource::Map(VulkanDeviceFunctions& vk, VkDeviceSize offset, VkDeviceSize size)
{
    return vk.MapMemory(vk.device, this->deviceMemory, offset, size, 0, &this->mappedMemory);
}

void VulkanMemoryResource::Unmap(VulkanDeviceFunctions& vk)
{
    if (this->mappedMemory != nullptr)
    {
        vk.UnmapMemory(vk.device, this->deviceMemory);
        this->mappedMemory = nullptr;
    }
}

VkResult VulkanMemoryResource::BindBuffer(VulkanDeviceFunctions& vk, VkDeviceSize offset)
{
    return vk.BindBufferMemory(vk.device, this->buffer, this->deviceMemory, offset);
}

VkResult VulkanMemoryResource::BindImage(VulkanDeviceFunctions& vk, VkDeviceSize offset)
{
    return vk.BindImageMemory(vk.device, this->image, this->deviceMemory, offset);
}

VkResult VulkanMemoryResource::Flush(VulkanDeviceFunctions& vk, VkDeviceSize offset, VkDeviceSize size)
{
    if (mappedMemory != nullptr)
    {
        VulkanMappedMemoryRange mappedRange(this->deviceMemory, offset, size);
        return vk.FlushMappedMemoryRanges(vk.device, 1, &mappedRange);
    }
    else
    {
        Map(vk, offset, size);
        VulkanMappedMemoryRange mappedRange(this->deviceMemory, offset, size);
        auto result = vk.FlushMappedMemoryRanges(vk.device, 1, &mappedRange);
        Unmap(vk);
        return result;
    }
}

VkResult VulkanMemoryResource::FlushChanges(VulkanDeviceFunctions& vk, VkDeviceSize offset, VkDeviceSize size)
{
    //Flush if VK_MEMORY_PROPERTY_HOST_COHERENT_BIT isn't set
    if ((this->bufferMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
        return Flush(vk, offset, size);

    return VK_SUCCESS;
}

VkResult VulkanMemoryResource::Invalidate(VulkanDeviceFunctions& vk, VkDeviceSize offset, VkDeviceSize size)
{
    VulkanMappedMemoryRange mappedRange(this->deviceMemory, offset, size);
    return vk.InvalidateMappedMemoryRanges(vk.device, 1, &mappedRange);
}

#endif
