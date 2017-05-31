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

#include "MetalResources.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//MetalAssetResources
MetalAssetResources::MetalAssetResources()
{
}

void MetalAssetResources::Destroy()
{
    this->inputFormatsByNameHash.Destroy();

    for (auto& texture : this->texturesByNameHash)
        texture.second.Destroy();
    this->texturesByNameHash.Destroy();

    for (auto& count : this->textureOffsetsByDimension)
        count.Reset();

    for (auto& shaders : this->shadersByShaderTypeAndNameHash)
        shaders.Destroy();

    this->materialsByNameHash.Destroy();

    for (auto& mesh : this->meshesByNameHash)
        mesh.second.Destroy();
    this->meshesByNameHash.Destroy();
}

//MetalFullScreenQuadMesh
MetalFullScreenQuadMesh::MetalFullScreenQuadMesh()
{
    this->vertexDescriptor = nullptr;
    this->vertexBuffer = nullptr;
    this->vertexCount = 0;
}

void MetalFullScreenQuadMesh::Create(id<MTLDevice> device, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
    if (this->vertexDescriptor == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to allocate vertex descriptor.");
        return;
    }

    auto layout = this->vertexDescriptor.layouts[0];
    layout.stride = 0;
    layout.stepFunction = MTLVertexStepFunctionPerVertex;
    layout.stepRate = 1;

    {
        auto metalAttribute = this->vertexDescriptor.attributes[0];
        metalAttribute.offset = layout.stride;
        metalAttribute.bufferIndex = DEFAULT_BUFFER_INDEX;
        metalAttribute.format = MTLVertexFormatFloat3;

        layout.stride += sizeof(float) * 3;
    }

    {
        auto metalAttribute = this->vertexDescriptor.attributes[1];
        metalAttribute.offset = layout.stride;
        metalAttribute.bufferIndex = DEFAULT_BUFFER_INDEX;
        metalAttribute.format = MTLVertexFormatFloat2;

        layout.stride += sizeof(float) * 2;
    }

    this->vertexCount = 4;
    this->primitiveType = MTLPrimitiveTypeTriangleStrip;

    const float vertices[] =
    {
        -1,  1, 0,  0, 0, //Upper left
         1,  1, 0,  1, 0, //Upper right
        -1, -1, 0,  0, 1, //Lower left
         1, -1, 0,  1, 1  //Lowwer right
    };

    auto vertexBufferByteSize = layout.stride * this->vertexCount;

    this->vertexBuffer = [device newBufferWithBytes:vertices length:vertexBufferByteSize options:MTLResourceCPUCacheModeDefaultCache];
}

void MetalFullScreenQuadMesh::Destroy()
{
    this->vertexDescriptor = nullptr;
    this->vertexBuffer = nullptr;
    this->vertexCount = 0;
}

//MetalCommonResources
MetalCommonResources::MetalCommonResources(Allocator* allocator) : fullScreenQuadShaders(allocator)
{
}

void MetalCommonResources::Destroy()
{
    this->fullScreenQuadGraphicsPipelineState = nullptr;
    this->fullScreenQuadGraphicsPipelineStateDesc = nullptr;
    this->fullScreenQuadMesh.Destroy();
    this->fullScreenQuadShaders.Destroy();
    this->commonShaderLibrary.Destroy();

    this->defaultSamplerDescriptor = nullptr;
    this->defaultSampler = nullptr;
}

#endif
