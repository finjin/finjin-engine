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

#include "MetalInputFormat.hpp"
#include "MetalIncludes.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define DEFAULT_BUFFER_INDEX 0


//Implementation----------------------------------------------------------------
MetalInputFormat::MetalInputFormat()
{
    this->inputFormatHash = 0;
    this->inputFormatElementCount = 0;
    this->vertexDescriptor = nullptr;
}

void MetalInputFormat::Create(const GpuInputFormatStruct& inputFormat, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->inputFormatHash = inputFormat.GetElementHash();
    this->inputFormatElementCount = inputFormat.elements.size();

    this->vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];

    auto layout = this->vertexDescriptor.layouts[0];
    layout.stride = inputFormat.totalSize;
    layout.stepFunction = MTLVertexStepFunctionPerVertex;
    layout.stepRate = 1;

    for (size_t elementIndex = 0; elementIndex < inputFormat.elements.size(); elementIndex++)
    {
        auto& element = inputFormat.elements[elementIndex];
        auto metalAttribute = this->vertexDescriptor.attributes[elementIndex];

        metalAttribute.offset = element.gpuPaddedOffset;
        metalAttribute.bufferIndex = DEFAULT_BUFFER_INDEX;

        switch (element.type)
        {
            case NumericStructElementType::NONE: break;
            case NumericStructElementType::INT1: metalAttribute.format = MTLVertexFormatInt; break;
            case NumericStructElementType::INT2: metalAttribute.format = MTLVertexFormatInt2; break;
            case NumericStructElementType::INT3: metalAttribute.format = MTLVertexFormatInt3; break;
            case NumericStructElementType::INT4: metalAttribute.format = MTLVertexFormatInt4; break;
            case NumericStructElementType::UINT1: metalAttribute.format = MTLVertexFormatUInt; break;
            case NumericStructElementType::UINT2: metalAttribute.format = MTLVertexFormatUInt2; break;
            case NumericStructElementType::UINT3: metalAttribute.format = MTLVertexFormatUInt3; break;
            case NumericStructElementType::UINT4: metalAttribute.format = MTLVertexFormatUInt4; break;
            case NumericStructElementType::FLOAT1: metalAttribute.format = MTLVertexFormatFloat; break;
            case NumericStructElementType::FLOAT2: metalAttribute.format = MTLVertexFormatFloat2; break;
            case NumericStructElementType::FLOAT3: metalAttribute.format = MTLVertexFormatFloat3; break;
            case NumericStructElementType::FLOAT4: metalAttribute.format = MTLVertexFormatFloat4; break;
            case NumericStructElementType::FLOAT3x3: FINJIN_SET_ERROR(error, "Failed to use 'float3x3' as input element."); return;
            case NumericStructElementType::FLOAT4x4: FINJIN_SET_ERROR(error, "Failed to use 'float4x4' as input element."); return;
            case NumericStructElementType::NESTED_STRUCT: FINJIN_SET_ERROR(error, "Failed to use 'nested struct' as input element."); return;
            default: FINJIN_SET_ERROR(error, "Unrecognized input element."); return;
        }
    }
}

void MetalInputFormat::SetBufferIndex(size_t index)
{
    for (size_t elementIndex = 0; elementIndex < this->inputFormatElementCount; elementIndex++)
        this->vertexDescriptor.attributes[elementIndex].bufferIndex = static_cast<NSUInteger>(index);
}

#endif
