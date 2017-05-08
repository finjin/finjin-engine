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

#include "VulkanGpuBuffer.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define CONSERVATIVE_MEMORY_MAPPING 0


//Implementation----------------------------------------------------------------

//VulkanGpuBuffer
VulkanGpuBuffer::VulkanGpuBuffer()
{
    this->cpuPointer = nullptr;
    this->elementStride = 0;
    this->byteSize = 0;
    this->inputFormat = nullptr;
}

bool VulkanGpuBuffer::CreateIndexBuffer(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, FinjinMesh::IndexBuffer& meshAssetIndexBuffer, VulkanGpuDescription& gpuDescription, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (meshAssetIndexBuffer.empty())
        return false;

    this->elementStride = meshAssetIndexBuffer.GetElementSize();
    this->byteSize = this->elementStride * meshAssetIndexBuffer.size();
    this->cpuPointer = meshAssetIndexBuffer.GetBytes();

    this->uploader.CreateBuffer(vk, allocationCallbacks, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, this->cpuPointer, this->byteSize, gpuDescription, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create index uploader buffer.");
        return false;
    }

    this->gpu.CreateBuffer(vk, allocationCallbacks, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr, this->byteSize, gpuDescription, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create index buffer.");
        return false;
    }

    return true;
}

bool VulkanGpuBuffer::CreateVertexBuffer(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, FinjinMesh::VertexBuffer& meshAssetVertexBuffer, GpuInputFormatStruct* inputFormat, VulkanGpuDescription& gpuDescription, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (meshAssetVertexBuffer.empty())
        return false;

    //Create interleaved vertex buffer
    this->inputFormat = inputFormat;
    this->elementStride = meshAssetVertexBuffer.GetVertexSize();
    this->byteSize = this->elementStride * meshAssetVertexBuffer.vertexCount;
    if (!this->cpu.Create(this->byteSize, allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create vertex buffer CPU buffer.");
        return false;
    }
    this->cpuPointer = this->cpu.data();
    if (!meshAssetVertexBuffer.Interleave(this->cpu))
    {
        FINJIN_SET_ERROR(error, "Failed to interleave vertex data.");
        return false;
    }

    //Create uploader
    this->uploader.CreateBuffer(vk, allocationCallbacks, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, this->cpuPointer, this->byteSize, gpuDescription, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create vertex uploader buffer.");
        return false;
    }

    //Create vertex buffer
    this->gpu.CreateBuffer(vk, allocationCallbacks, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr, this->byteSize, gpuDescription, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create vertex buffer.");
        return false;
    }

    return true;
}

void VulkanGpuBuffer::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    this->cpuPointer = nullptr;
    this->cpu.Destroy();
    this->gpu.Destroy(vk, allocationCallbacks);
    this->uploader.Destroy(vk, allocationCallbacks);
    this->elementStride = 0;
    this->byteSize = 0;
    this->inputFormat = nullptr;
}

bool VulkanGpuBuffer::Upload(VulkanDeviceFunctions& vk, VkCommandBuffer commandBuffer)
{
    if (this->byteSize == 0)
        return false;

    this->uploader.CopyBufferTo(vk, commandBuffer, this->gpu);

    return true;
}

//VulkanGpuRenderingUniformBuffer
VulkanGpuRenderingUniformBuffer::VulkanGpuRenderingUniformBuffer()
{
}

VulkanGpuRenderingUniformBuffer::~VulkanGpuRenderingUniformBuffer()
{
    assert(this->resource.deviceMemory == VK_NULL_HANDLE);
}

void VulkanGpuRenderingUniformBuffer::Create(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, const GpuRenderingConstantBufferStruct& bufferStruct, size_t instanceCount, VulkanGpuDescription& gpuDescription, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->numericStruct = &bufferStruct;
    this->instanceCount = instanceCount;

    if (instanceCount == 0)
        return;

    auto byteSize = bufferStruct.paddedTotalSize * (instanceCount - 1) + bufferStruct.totalSize;
    this->resource.CreateBuffer(vk, allocationCallbacks, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, nullptr, byteSize, gpuDescription, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create uniform buffer.");
        return;
    }

#if CONSERVATIVE_MEMORY_MAPPING
    //Map the buffer only when necessary
    StartWrites(vk);
    SetDefaults();
    FinishWrites(vk);
#else
    //Map the buffer and leave it mapped
    if (this->resource.Map(vk) != VK_SUCCESS)
    {
        assert(0 && "Failed to map uniform buffer.");
        return;
    }

    this->structInstanceBuffer = static_cast<uint8_t*>(this->resource.mappedMemory);

    SetDefaults();
#endif
}

void VulkanGpuRenderingUniformBuffer::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    this->resource.Destroy(vk, allocationCallbacks);

    this->structInstanceBuffer = nullptr;
}

void VulkanGpuRenderingUniformBuffer::StartWrites(VulkanDeviceFunctions& vk)
{
#if CONSERVATIVE_MEMORY_MAPPING
    assert(this->structInstanceBuffer == nullptr);

    if (this->resource.Map(vk) != VK_SUCCESS)
    {
        assert(0 && "Failed to map uniform buffer.");
        return;
    }

    this->structInstanceBuffer = static_cast<uint8_t*>(this->resource.mappedMemory);
#endif
}

void VulkanGpuRenderingUniformBuffer::FinishWrites(VulkanDeviceFunctions& vk, VkDeviceSize offset, VkDeviceSize size)
{
    if (this->structInstanceBuffer != nullptr)
    {
        this->resource.FlushChanges(vk, offset, size);

    #if CONSERVATIVE_MEMORY_MAPPING
        this->resource.Unmap(vk);

        this->structInstanceBuffer = nullptr;
    #endif
    }
}

VulkanMemoryResource& VulkanGpuRenderingUniformBuffer::GetResource()
{
    return this->resource;
}

#endif
