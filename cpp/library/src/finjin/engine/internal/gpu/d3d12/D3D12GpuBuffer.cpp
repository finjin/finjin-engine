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

#include "D3D12GpuBuffer.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define CONSERVATIVE_MEMORY_MAPPING 0


//Implementation----------------------------------------------------------------

//D3D12GpuBuffer
D3D12GpuBuffer::D3D12GpuBuffer()
{
    this->cpuPointer = nullptr;
    this->gpu = nullptr;
    this->uploader = nullptr;
    this->elementStride = 0;
    this->byteSize = 0;
    this->inputFormat = nullptr;
}

bool D3D12GpuBuffer::CreateIndexBuffer(FinjinMesh::IndexBuffer& meshAssetIndexBuffer, Allocator* allocator, ID3D12Device* device, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (meshAssetIndexBuffer.empty())
        return false;

    this->elementStride = meshAssetIndexBuffer.GetElementSize();
    this->byteSize = this->elementStride * meshAssetIndexBuffer.size();
    this->cpuPointer = meshAssetIndexBuffer.GetBytes();

    this->gpu = D3D12Utilities::CreateDefaultBufferAndUploader(device, this->cpuPointer, this->byteSize, this->uploader, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create default index buffer.");
        return false;
    }

    return true;
}

bool D3D12GpuBuffer::CreateVertexBuffer(FinjinMesh::VertexBuffer& meshAssetVertexBuffer, GpuInputFormatStruct* inputFormat, Allocator* allocator, ID3D12Device* device, Error& error)
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

    //Create uploader and vertex buffer
    this->gpu = D3D12Utilities::CreateDefaultBufferAndUploader(device, this->cpuPointer, this->byteSize, this->uploader, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create vertex buffer GPU buffer.");
        return false;
    }

    return true;
}

void D3D12GpuBuffer::Destroy()
{
    this->cpuPointer = nullptr;
    this->cpu.Destroy();
    this->gpu = nullptr;
    this->uploader = nullptr;
    this->elementStride = 0;
    this->byteSize = 0;
    this->inputFormat = nullptr;
}

bool D3D12GpuBuffer::Upload(ID3D12GraphicsCommandList* commandList)
{
    if (this->byteSize == 0)
        return false;

    D3D12_SUBRESOURCE_DATA subResourceData;
    subResourceData.pData = this->cpuPointer;
    subResourceData.RowPitch = this->byteSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(this->gpu.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

    UpdateSubresources<1>(commandList, this->gpu.Get(), this->uploader.Get(), 0, 0, 1, &subResourceData);

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(this->gpu.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

    return true;
}

//D3D12GpuRenderingConstantBuffer
D3D12GpuRenderingConstantBuffer::D3D12GpuRenderingConstantBuffer()
{
    this->resource = nullptr;
}

D3D12GpuRenderingConstantBuffer::~D3D12GpuRenderingConstantBuffer()
{
    Destroy();
}

void D3D12GpuRenderingConstantBuffer::Create(ID3D12Device* device, const GpuRenderingConstantBufferStruct& bufferStruct, size_t instanceCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->numericStruct = &bufferStruct;
    this->instanceCount = instanceCount;

    if (instanceCount == 0)
        return;

    auto paddedTotalSize = D3D12Utilities::GetConstantBufferAlignedSize(bufferStruct.paddedTotalSize, bufferStruct.totalSize, instanceCount);
    //std::cout << "Creating constant buffer " << bufferStruct.typeName << " with size " << paddedTotalSize << std::endl;

    auto result = device->CreateCommittedResource
        (
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(paddedTotalSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&this->resource)
        );
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, "Failed to create constant buffer.");
        return;
    }

#if CONSERVATIVE_MEMORY_MAPPING
    //Map the buffer only when necessary
    StartWrites();
    SetDefaults();
    FinishWrites();
#else
    //Map the buffer and leave it mapped
    result = this->resource->Map(0, nullptr, reinterpret_cast<void**>(&this->structInstanceBuffer));
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, "Failed to map constant buffer.");
        return;
    }

    SetDefaults();
#endif
}

void D3D12GpuRenderingConstantBuffer::Destroy()
{
    if (this->resource != nullptr && this->structInstanceBuffer != nullptr)
        this->resource->Unmap(0, nullptr);

    this->resource = nullptr;

    this->structInstanceBuffer = nullptr;
}

void D3D12GpuRenderingConstantBuffer::StartWrites()
{
#if CONSERVATIVE_MEMORY_MAPPING
    assert(this->structInstanceBuffer == nullptr);

    auto result = this->resource->Map(0, nullptr, reinterpret_cast<void**>(&this->structInstanceBuffer));
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        assert(0 && "Failed to map constant buffer.");
        return;
    }
#endif
}

void D3D12GpuRenderingConstantBuffer::FinishWrites()
{
#if CONSERVATIVE_MEMORY_MAPPING
    if (this->structInstanceBuffer != nullptr)
    {
        this->resource->Unmap(0, nullptr);

        this->structInstanceBuffer = nullptr;
    }
#endif
}

ID3D12Resource* D3D12GpuRenderingConstantBuffer::GetResource() const
{
    return this->resource.Get();
}

/*class D3D12GpuRenderingStructuredBufferResource : public GpuRenderingStructuredBuffer
{
public:
    D3D12GpuRenderingStructuredBufferResource();
    ~D3D12GpuRenderingStructuredBufferResource();

    void Create(ID3D12Device* device, const GpuRenderingStructuredBufferStruct& bufferStruct, size_t instanceCount, Error& error);
    void Destroy();

    ID3D12Resource* GetResource() const;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
};*/

/*//D3D12GpuRenderingStructuredBufferResource
D3D12GpuRenderingStructuredBufferResource::D3D12GpuRenderingStructuredBufferResource()
{
    this->resource = nullptr;
}

D3D12GpuRenderingStructuredBufferResource::~D3D12GpuRenderingStructuredBufferResource()
{
    Destroy();
}

void D3D12GpuRenderingStructuredBufferResource::Create(ID3D12Device* device, const GpuRenderingStructuredBufferStruct& bufferStruct, size_t instanceCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->numericStruct = &bufferStruct;
    this->instanceCount = instanceCount;

    if (instanceCount == 0)
        return;

    auto paddedTotalSize = bufferStruct.paddedTotalSize * instanceCount;

    auto result = device->CreateCommittedResource
        (
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(paddedTotalSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&this->resource)
        );
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, "Failed to create upload resource.");
        return;
    }

    result = this->resource->Map(0, nullptr, reinterpret_cast<void**>(&this->structInstanceBuffer));
    if (FINJIN_CHECK_HRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, "Failed to map upload resource.");
        return;
    }

    SetDefaults();
}

void D3D12GpuRenderingStructuredBufferResource::Destroy()
{
    if (this->resource != nullptr && this->structInstanceBuffer != nullptr)
        this->resource->Unmap(0, nullptr);

    this->resource = nullptr;

    this->structInstanceBuffer = nullptr;
}

ID3D12Resource* D3D12GpuRenderingStructuredBufferResource::GetResource() const
{
    return this->resource.Get();
}
*/

#endif
