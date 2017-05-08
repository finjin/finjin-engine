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
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/GenericGpuNumericStructs.hpp"
#include "VulkanMemoryResource.hpp"
#include "VulkanUtilities.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanGpuBuffer
    {
    public:
        VulkanGpuBuffer();

        bool CreateIndexBuffer(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, FinjinMesh::IndexBuffer& meshAssetIndexBuffer, VulkanGpuDescription& gpuDescription, Allocator* allocator, Error& error);
        bool CreateVertexBuffer(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, FinjinMesh::VertexBuffer& meshAssetVertexBuffer, GpuInputFormatStruct* inputFormat, VulkanGpuDescription& gpuDescription, Allocator* allocator, Error& error);
        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        bool Upload(VulkanDeviceFunctions& vk, VkCommandBuffer commandBuffer);

    public:
        uint8_t* cpuPointer;
        DynamicVector<uint8_t> cpu;
        VulkanMemoryResource gpu;
        VulkanMemoryResource uploader;
        size_t elementStride; //Bytes from one element to the next
        size_t byteSize; //cpu.size()
        GpuInputFormatStruct* inputFormat; //Pointer to input format struct. Will be null for non-vertex buffers
    };

    class VulkanGpuRenderingUniformBuffer : public GpuRenderingConstantBuffer
    {
    public:
        VulkanGpuRenderingUniformBuffer();
        ~VulkanGpuRenderingUniformBuffer();

        void Create(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, const GpuRenderingConstantBufferStruct& bufferStruct, size_t instanceCount, VulkanGpuDescription& gpuDescription, Error& error);
        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        void StartWrites(VulkanDeviceFunctions& vk);
        void FinishWrites(VulkanDeviceFunctions& vk, VkDeviceSize offset = 0, VkDeviceSize size = VK_WHOLE_SIZE);

        VulkanMemoryResource& GetResource();

    private:
        VulkanMemoryResource resource;
    };

} }
