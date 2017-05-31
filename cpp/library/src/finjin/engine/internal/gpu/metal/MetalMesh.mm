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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_METAL

#include "MetalMesh.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//MetalMesh::Submesh
MetalMesh::Submesh::Submesh(MetalMesh* owner)
{
    this->mesh = owner;
    this->vertexBufferIndex = 0;
    this->startIndexLocation = 0;
    this->indexCount = 0;
    this->baseVertexLocation = 0;
    this->vertexCount = 0;
    this->primitiveType = MTLPrimitiveTypeTriangle;
}

MetalVertexBufferView MetalMesh::Submesh::GetVertexBufferView() const
{
    if (this->vertexBuffer.resource != nullptr)
    {
        MetalVertexBufferView vbv;
        vbv.buffer = this->vertexBuffer.resource;
        vbv.bufferOffset = 0;
        vbv.byteSize = this->vertexBuffer.byteSize;
        vbv.strideInBytes = this->vertexBuffer.elementStride;
        return vbv;
    }
    else
        return this->mesh->GetSharedVertexBufferView(this->vertexBufferIndex);
}

MetalIndexBufferView MetalMesh::Submesh::GetIndexBufferView() const
{
    if (this->indexBuffer.resource != nullptr)
    {
        MetalIndexBufferView ibv;
        ibv.buffer = this->indexBuffer.resource;
        ibv.bufferOffset = 0;
        ibv.byteSize = this->indexBuffer.byteSize;
        ibv.strideInBytes = this->indexBuffer.elementStride;
        ibv.indexType = this->indexBuffer.elementStride == 2 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
        return ibv;
    }
    else
        return this->mesh->GetSharedIndexBufferView();
}

GpuInputFormatStruct* MetalMesh::Submesh::GetInputFormat()
{
    if (this->vertexBuffer.inputFormat != nullptr)
        return this->vertexBuffer.inputFormat;
    else
    {
        assert(vertexBufferIndex < this->mesh->sharedVertexBuffers.size());
        return this->mesh->sharedVertexBuffers[vertexBufferIndex].inputFormat;
    }
}

//MetalMesh
MetalMesh::MetalMesh(Allocator* allocator)
{
}

void MetalMesh::Destroy()
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

void MetalMesh::HandleCreationFailure()
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

MetalVertexBufferView MetalMesh::GetSharedVertexBufferView(size_t vertexBufferIndex) const
{
    MetalVertexBufferView vbv;
    vbv.buffer = this->sharedVertexBuffers[vertexBufferIndex].resource;
    vbv.bufferOffset = 0;
    vbv.byteSize = this->sharedVertexBuffers[vertexBufferIndex].byteSize;
    vbv.strideInBytes = this->sharedVertexBuffers[vertexBufferIndex].elementStride;
    return vbv;
}

MetalIndexBufferView MetalMesh::GetSharedIndexBufferView() const
{
    MetalIndexBufferView ibv;
    ibv.buffer = this->sharedIndexBuffer.resource;
    ibv.bufferOffset = 0;
    ibv.byteSize = this->sharedIndexBuffer.byteSize;
    ibv.strideInBytes = this->sharedIndexBuffer.elementStride;
    ibv.indexType = this->sharedIndexBuffer.elementStride == 2 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
    return ibv;
}

#endif
