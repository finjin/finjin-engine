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


//Includes---------------------------------------------------------------------
#include "D3D12Includes.hpp"
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/GenericGpuNumericStructs.hpp"
#include "D3D12Utilities.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    using namespace Finjin::Common;

    class D3D12GpuBuffer
    {
    public:
        D3D12GpuBuffer();

        bool CreateIndexBuffer(FinjinMesh::IndexBuffer& meshAssetIndexBuffer, Allocator* allocator, ID3D12Device* device, Error& error);
        bool CreateVertexBuffer(FinjinMesh::VertexBuffer& meshAssetVertexBuffer, GpuInputFormatStruct* inputFormat, Allocator* allocator, ID3D12Device* device, Error& error);
        void Destroy();

        bool Upload(ID3D12GraphicsCommandList* cmdList);

    public:
        uint8_t* cpuPointer;
        AllocatedVector<uint8_t> cpu;
        Microsoft::WRL::ComPtr<ID3D12Resource> gpu;
        Microsoft::WRL::ComPtr<ID3D12Resource> uploader;
        size_t elementStride; //Bytes from one element to the next
        size_t byteSize; //cpu.size()
        GpuInputFormatStruct* inputFormat; //Pointer to input format struct. Will be null for non-vertex buffers
    };

    class D3D12GpuRenderingConstantBuffer : public GpuRenderingConstantBuffer
    {
    public:
        D3D12GpuRenderingConstantBuffer();
        ~D3D12GpuRenderingConstantBuffer();

        void Create(ID3D12Device* device, const GpuRenderingConstantBufferStruct& bufferStruct, size_t instanceCount, Error& error);
        void Destroy();

        ID3D12Resource* GetResource() const;
        
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    };

    class D3D12GpuRenderingStructuredBufferResource : public GpuRenderingStructuredBuffer
    {
    public:
        D3D12GpuRenderingStructuredBufferResource();
        ~D3D12GpuRenderingStructuredBufferResource();

        void Create(ID3D12Device* device, const GpuRenderingStructuredBufferStruct& bufferStruct, size_t instanceCount, Error& error);
        void Destroy();

        ID3D12Resource* GetResource() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    };

} }
