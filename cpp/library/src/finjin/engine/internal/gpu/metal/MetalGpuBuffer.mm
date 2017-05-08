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

#include "MetalGpuBuffer.hpp"
#include "MetalUtilities.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define RESOURCE_STORAGE_MANAGED 0 //This never needs to be enabled, and only works on macOS


//Implementation----------------------------------------------------------------

//MetalGpuBuffer
MetalGpuBuffer::MetalGpuBuffer()
{
    this->resource = nullptr;
    this->elementStride = 0;
    this->byteSize = 0;
    this->inputFormat = nullptr;
}

bool MetalGpuBuffer::CreateIndexBuffer(FinjinMesh::IndexBuffer& meshAssetIndexBuffer, Allocator* allocator, id<MTLDevice> device, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (meshAssetIndexBuffer.empty())
        return false;

    this->elementStride = meshAssetIndexBuffer.GetElementSize();
    this->byteSize = this->elementStride * meshAssetIndexBuffer.size();

    this->resource = [device newBufferWithBytes:meshAssetIndexBuffer.GetBytes() length:this->byteSize options:MTLResourceCPUCacheModeDefaultCache];

    return true;
}

bool MetalGpuBuffer::CreateVertexBuffer(FinjinMesh::VertexBuffer& meshAssetVertexBuffer, GpuInputFormatStruct* inputFormat, Allocator* allocator, id<MTLDevice> device, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (meshAssetVertexBuffer.empty())
        return false;

    //Create interleaved vertex buffer
    this->inputFormat = inputFormat;
    this->elementStride = meshAssetVertexBuffer.GetVertexSize();
    this->byteSize = this->elementStride * meshAssetVertexBuffer.vertexCount;
    ByteBuffer interleaved;
    if (!interleaved.Create(this->byteSize, allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create interleaved vertex buffer.");
        return false;
    }
    if (!meshAssetVertexBuffer.Interleave(interleaved))
    {
        FINJIN_SET_ERROR(error, "Failed to interleave vertex data.");
        return false;
    }

    this->resource = [device newBufferWithBytes:interleaved.data() length:this->byteSize options:MTLResourceCPUCacheModeDefaultCache];

    return true;
}

void MetalGpuBuffer::Destroy()
{
    this->resource = nullptr;
    this->elementStride = 0;
    this->byteSize = 0;
    this->inputFormat = nullptr;
}

//MetalGpuRenderingUniformBuffer
MetalGpuRenderingUniformBuffer::MetalGpuRenderingUniformBuffer()
{
    this->resource = nullptr;
}

MetalGpuRenderingUniformBuffer::~MetalGpuRenderingUniformBuffer()
{
    Destroy();
}

void MetalGpuRenderingUniformBuffer::Create(id<MTLDevice> device, const MetalGpuDescription& deviceDescription, const GpuRenderingConstantBufferStruct& bufferStruct, size_t instanceCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->numericStruct = &bufferStruct;
    this->instanceCount = instanceCount;

    if (instanceCount == 0)
        return;

    auto paddedTotalSize = MetalUtilities::GetUniformBufferAlignedSize(bufferStruct.paddedTotalSize, bufferStruct.totalSize, instanceCount, deviceDescription.featureSet.limits.minBufferOffsetAlignment);

    MTLResourceOptions resourceOptions = MTLResourceCPUCacheModeDefaultCache;
#if RESOURCE_STORAGE_MANAGED
    resourceOptions = MTLResourceStorageModeManaged;
#endif
    this->resource = [device newBufferWithLength:paddedTotalSize options:resourceOptions];
    if (this->resource == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to create uniform buffer.");
        return;
    }

#if RESOURCE_STORAGE_MANAGED
    StartWrites();
    SetDefaults();
    FinishWrites();
#else
    this->structInstanceBuffer = static_cast<uint8_t*>(this->resource.contents);
    if (this->structInstanceBuffer == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to access uniform buffer.");
        return;
    }

    SetDefaults();
#endif
}

void MetalGpuRenderingUniformBuffer::Destroy()
{
    this->resource = nullptr;

    this->structInstanceBuffer = nullptr;
}

void MetalGpuRenderingUniformBuffer::StartWrites()
{
#if RESOURCE_STORAGE_MANAGED
    assert(this->structInstanceBuffer == nullptr);

    this->structInstanceBuffer = static_cast<uint8_t*>(this->resource.contents);
    if (this->structInstanceBuffer == nullptr)
    {
        assert(0 && "Failed to access uniform buffer.");
        return;
    }
#endif
}

void MetalGpuRenderingUniformBuffer::FinishWrites()
{
#if RESOURCE_STORAGE_MANAGED
    if (this->structInstanceBuffer != nullptr)
    {
        [this->resource didModifyRange:NSMakeRange(0, this->resource.length)];

        this->structInstanceBuffer = nullptr;
    }
#endif
}

#endif
