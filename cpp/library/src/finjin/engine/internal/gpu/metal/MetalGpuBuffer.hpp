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
#include "MetalGpuDescription.hpp"
#include "MetalIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalGpuBuffer
    {
    public:
        MetalGpuBuffer();

        bool CreateIndexBuffer(FinjinMesh::IndexBuffer& meshAssetIndexBuffer, Allocator* allocator, id<MTLDevice> device, Error& error);
        bool CreateVertexBuffer(FinjinMesh::VertexBuffer& meshAssetVertexBuffer, GpuInputFormatStruct* inputFormat, Allocator* allocator, id<MTLDevice> device, Error& error);
        void Destroy();

    public:
        id<MTLBuffer> resource;
        size_t elementStride; //Bytes from one element to the next
        size_t byteSize;
        GpuInputFormatStruct* inputFormat; //Pointer to input format struct. Will be null for non-vertex buffers
    };

    class MetalGpuRenderingUniformBuffer : public GpuRenderingConstantBuffer
    {
    public:
        MetalGpuRenderingUniformBuffer();
        ~MetalGpuRenderingUniformBuffer();

        void Create(id<MTLDevice> device, const MetalGpuDescription& deviceDescription, const GpuRenderingConstantBufferStruct& bufferStruct, size_t instanceCount, Error& error);
        void Destroy();

        void StartWrites();
        void FinishWrites();

    public:
        id<MTLBuffer> resource;
    };

} }
