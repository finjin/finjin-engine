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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12

#include "D3D12Mesh.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//D3D12Mesh::Submesh
D3D12Mesh::Submesh::Submesh(D3D12Mesh* owner)
{
    this->mesh = owner;
    this->vertexBufferIndex = 0;
    this->startIndexLocation = 0;
    this->indexCount = 0;
    this->baseVertexLocation = 0;
    this->vertexCount = 0;
    this->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

D3D12_VERTEX_BUFFER_VIEW D3D12Mesh::Submesh::GetVertexBufferView() const
{
    if (this->vertexBuffer.gpu != nullptr)
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = this->vertexBuffer.gpu->GetGPUVirtualAddress();
        vbv.StrideInBytes = static_cast<UINT>(this->vertexBuffer.elementStride);
        vbv.SizeInBytes = static_cast<UINT>(this->vertexBuffer.byteSize);
        return vbv;
    }
    else
        return this->mesh->GetSharedVertexBufferView(this->vertexBufferIndex);
}

D3D12_INDEX_BUFFER_VIEW D3D12Mesh::Submesh::GetIndexBufferView() const
{
    if (this->indexBuffer.gpu != nullptr)
    {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = this->indexBuffer.gpu->GetGPUVirtualAddress();
        ibv.Format = this->indexBuffer.elementStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        ibv.SizeInBytes = static_cast<UINT>(this->indexBuffer.byteSize);
        return ibv;
    }
    else
        return this->mesh->GetSharedIndexBufferView();
}

GpuInputFormatStruct* D3D12Mesh::Submesh::GetInputFormat()
{
    if (this->vertexBuffer.inputFormat != nullptr)
        return this->vertexBuffer.inputFormat;
    else
    {
        assert(vertexBufferIndex < this->mesh->sharedVertexBuffers.size());
        return this->mesh->sharedVertexBuffers[vertexBufferIndex].inputFormat;
    }
}

//D3D12Mesh
D3D12Mesh::D3D12Mesh(Allocator* allocator)
{
    this->finjinMesh = nullptr;
    this->isResidentCountdown = 0;
    this->waitingToBeResidentNext = nullptr;
}

void D3D12Mesh::Destroy()
{
    for (auto& vb : this->sharedVertexBuffers)
        vb.Destroy();
    this->sharedVertexBuffers.Destroy();

    this->sharedIndexBuffer.Destroy();

    for (auto& submesh : this->submeshes)
    {
        submesh.vertexBuffer.Destroy();
        submesh.indexBuffer.Destroy();
    }
    this->submeshes.Destroy();
}

void D3D12Mesh::HandleCreationFailure()
{
    for (auto& vb : this->sharedVertexBuffers)
        vb.Destroy();
    this->sharedVertexBuffers.Destroy();

    this->sharedIndexBuffer.Destroy();

    for (auto& submesh : this->submeshes)
    {
        submesh.vertexBuffer.Destroy();
        submesh.indexBuffer.Destroy();
    }
    this->submeshes.Destroy();
}

void D3D12Mesh::ReleaseUploaders()
{
    for (auto& vb : this->sharedVertexBuffers)
        vb.uploader = nullptr;

    this->sharedIndexBuffer.uploader = nullptr;

    for (auto& submesh : this->submeshes)
    {
        submesh.vertexBuffer.uploader = nullptr;
        submesh.indexBuffer.uploader = nullptr;
    }
}

D3D12_VERTEX_BUFFER_VIEW D3D12Mesh::GetSharedVertexBufferView(UINT vertexBufferIndex) const
{
    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.BufferLocation = this->sharedVertexBuffers[vertexBufferIndex].gpu->GetGPUVirtualAddress();
    vbv.StrideInBytes = static_cast<UINT>(this->sharedVertexBuffers[vertexBufferIndex].elementStride);
    vbv.SizeInBytes = static_cast<UINT>(this->sharedVertexBuffers[vertexBufferIndex].byteSize);
    return vbv;
}

D3D12_INDEX_BUFFER_VIEW D3D12Mesh::GetSharedIndexBufferView() const
{
    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = this->sharedIndexBuffer.gpu->GetGPUVirtualAddress();
    ibv.Format = this->sharedIndexBuffer.elementStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    ibv.SizeInBytes = static_cast<UINT>(this->sharedIndexBuffer.byteSize);
    return ibv;
}

bool D3D12Mesh::IsResidentOnGpu() const
{
    return this->isResidentCountdown == 0;
}

void D3D12Mesh::UpdateResidentOnGpuStatus()
{
    if (this->isResidentCountdown > 0)
    {
        this->isResidentCountdown--;

        if (IsResidentOnGpu())
            ReleaseUploaders();
    }
}

#endif
