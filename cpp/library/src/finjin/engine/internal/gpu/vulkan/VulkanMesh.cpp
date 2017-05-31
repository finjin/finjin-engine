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

#include "VulkanMesh.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//VulkanMesh::Submesh
VulkanMesh::Submesh::Submesh(VulkanMesh* owner)
{
    this->mesh = owner;
    this->vertexBufferIndex = 0;
    this->startIndexLocation = 0;
    this->indexCount = 0;
    this->baseVertexLocation = 0;
    this->vertexCount = 0;
    this->primitiveType = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

VulkanVertexBufferView VulkanMesh::Submesh::GetVertexBufferView() const
{
    if (this->vertexBuffer.gpu.buffer != VK_NULL_HANDLE)
    {
        VulkanVertexBufferView vbv;
        vbv.buffer = this->vertexBuffer.gpu.buffer;
        vbv.bufferOffset = 0;
        vbv.byteSize = this->vertexBuffer.byteSize;
        vbv.strideInBytes = this->vertexBuffer.elementStride;
        return vbv;
    }
    else
        return this->mesh->GetSharedVertexBufferView(this->vertexBufferIndex);
}

VulkanIndexBufferView VulkanMesh::Submesh::GetIndexBufferView() const
{
    if (this->vertexBuffer.gpu.buffer != VK_NULL_HANDLE)
    {
        VulkanIndexBufferView ibv;
        ibv.buffer = this->indexBuffer.gpu.buffer;
        ibv.bufferOffset = 0;
        ibv.byteSize = this->indexBuffer.byteSize;
        ibv.indexType = this->indexBuffer.elementStride == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
        return ibv;
    }
    else
        return this->mesh->GetSharedIndexBufferView();
}

GpuInputFormatStruct* VulkanMesh::Submesh::GetInputFormat()
{
    if (this->vertexBuffer.inputFormat != nullptr)
        return this->vertexBuffer.inputFormat;
    else
    {
        assert(vertexBufferIndex < this->mesh->sharedVertexBuffers.size());
        return this->mesh->sharedVertexBuffers[vertexBufferIndex].inputFormat;
    }
}

//VulkanMesh
VulkanMesh::VulkanMesh(Allocator* allocator)
{    
    this->isResidentCountdown = 0;
    this->waitingToBeResidentNext = nullptr;
}

void VulkanMesh::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    for (auto& vb : this->sharedVertexBuffers)
        vb.Destroy(vk, allocationCallbacks);
    this->sharedVertexBuffers.Destroy();

    this->sharedIndexBuffer.Destroy(vk, allocationCallbacks);

    for (auto& submesh : this->submeshes)
    {
        submesh.vertexBuffer.Destroy(vk, allocationCallbacks);
        submesh.indexBuffer.Destroy(vk, allocationCallbacks);
    }
    this->submeshes.Destroy();
}

void VulkanMesh::HandleCreationFailure(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    Destroy(vk, allocationCallbacks);
}

void VulkanMesh::ReleaseUploaders(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    for (auto& vb : this->sharedVertexBuffers)
        vb.uploader.Destroy(vk, allocationCallbacks);

    this->sharedIndexBuffer.uploader.Destroy(vk, allocationCallbacks);

    for (auto& submesh : this->submeshes)
    {
        submesh.vertexBuffer.uploader.Destroy(vk, allocationCallbacks);
        submesh.indexBuffer.uploader.Destroy(vk, allocationCallbacks);
    }
}

VulkanVertexBufferView VulkanMesh::GetSharedVertexBufferView(uint32_t vertexBufferIndex) const
{
    VulkanVertexBufferView vbv;
    vbv.buffer = this->sharedVertexBuffers[vertexBufferIndex].gpu.buffer;
    vbv.bufferOffset = 0;
    vbv.byteSize = this->sharedVertexBuffers[vertexBufferIndex].byteSize;
    vbv.strideInBytes = this->sharedVertexBuffers[vertexBufferIndex].elementStride;
    return vbv;
}

VulkanIndexBufferView VulkanMesh::GetSharedIndexBufferView() const
{
    VulkanIndexBufferView ibv;
    ibv.buffer = this->sharedIndexBuffer.gpu.buffer;
    ibv.bufferOffset = 0;
    ibv.byteSize = this->sharedIndexBuffer.byteSize;
    ibv.indexType = this->sharedIndexBuffer.elementStride == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32;
    return ibv;
}

bool VulkanMesh::IsResidentOnGpu() const
{
    return this->isResidentCountdown == 0;
}

void VulkanMesh::UpdateResidentOnGpuStatus(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    if (this->isResidentCountdown > 0)
    {
        this->isResidentCountdown--;

        if (IsResidentOnGpu())
            ReleaseUploaders(vk, allocationCallbacks);
    }
}

#endif
