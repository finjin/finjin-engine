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

#include "D3D12InputFormat.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
D3D12InputFormat::D3D12InputFormat()
{
    this->inputFormatHash = 0;
}

void D3D12InputFormat::Create(const GpuInputFormatStruct& inputFormat, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->inputFormatHash = inputFormat.GetElementHash();

    if (!this->elements.Create(inputFormat.elements.size(), allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create D3D input element collection.");
        return;
    }

    for (size_t elementIndex = 0; elementIndex < inputFormat.elements.size(); elementIndex++)
    {
        auto& element = inputFormat.elements[elementIndex];
        auto& d3d12Element = this->elements[elementIndex];

        d3d12Element.InputSlot = 0;
        d3d12Element.AlignedByteOffset = static_cast<UINT>(element.gpuPaddedOffset);
        d3d12Element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        d3d12Element.InstanceDataStepRate = 0;

        //Semantic names - https://msdn.microsoft.com/en-us/library/windows/desktop/bb509647(v=vs.85).aspx
        d3d12Element.SemanticIndex = 0;
        switch (element.elementID)
        {
            case GpuInputFormatStructMetadata::ElementID::BINORMAL: d3d12Element.SemanticName = "BINORMAL"; break;
            /*case GpuInputFormatStructMetadata::ElementID::BLEND_INDICES_0: d3d12Element.SemanticName = "BLENDINDICES"; break;
            case GpuInputFormatStructMetadata::ElementID::BLEND_INDICES_1: d3d12Element.SemanticIndex = 1; d3d12Element.SemanticName = "BLENDINDICES"; break;
            case GpuInputFormatStructMetadata::ElementID::BLEND_INDICES_2: d3d12Element.SemanticIndex = 2; d3d12Element.SemanticName = "BLENDINDICES"; break;
            case GpuInputFormatStructMetadata::ElementID::BLEND_INDICES_3: d3d12Element.SemanticIndex = 3; d3d12Element.SemanticName = "BLENDINDICES"; break;
            case GpuInputFormatStructMetadata::ElementID::BLEND_WEIGHT_0: d3d12Element.SemanticName = "BLENDWEIGHT"; break;
            case GpuInputFormatStructMetadata::ElementID::BLEND_WEIGHT_1: d3d12Element.SemanticIndex = 1; d3d12Element.SemanticName = "BLENDWEIGHT"; break;
            case GpuInputFormatStructMetadata::ElementID::BLEND_WEIGHT_2: d3d12Element.SemanticIndex = 2; d3d12Element.SemanticName = "BLENDWEIGHT"; break;
            case GpuInputFormatStructMetadata::ElementID::BLEND_WEIGHT_3: d3d12Element.SemanticIndex = 3; d3d12Element.SemanticName = "BLENDWEIGHT"; break;*/
            case GpuInputFormatStructMetadata::ElementID::COLOR_0: d3d12Element.SemanticName = "COLOR"; break;
            case GpuInputFormatStructMetadata::ElementID::COLOR_1: d3d12Element.SemanticIndex = 1; d3d12Element.SemanticName = "COLOR"; break;
            case GpuInputFormatStructMetadata::ElementID::COLOR_2: d3d12Element.SemanticIndex = 2; d3d12Element.SemanticName = "COLOR"; break;
            case GpuInputFormatStructMetadata::ElementID::COLOR_3: d3d12Element.SemanticIndex = 3; d3d12Element.SemanticName = "COLOR"; break;
            case GpuInputFormatStructMetadata::ElementID::NORMAL: d3d12Element.SemanticName = "NORMAL"; break;
            case GpuInputFormatStructMetadata::ElementID::POSITION: d3d12Element.SemanticName = "POSITION"; break;
            case GpuInputFormatStructMetadata::ElementID::POSITION_TRANSFORMED: d3d12Element.SemanticName = "POSITIONT"; break;
            case GpuInputFormatStructMetadata::ElementID::POINT_SIZE: d3d12Element.SemanticName = "PSIZE"; break;
            case GpuInputFormatStructMetadata::ElementID::TANGENT: d3d12Element.SemanticName = "TANGENT"; break;
            case GpuInputFormatStructMetadata::ElementID::TEX_COORD_0: d3d12Element.SemanticName = "TEXCOORD"; break;
            case GpuInputFormatStructMetadata::ElementID::TEX_COORD_1: d3d12Element.SemanticIndex = 1; d3d12Element.SemanticName = "TEXCOORD"; break;
            case GpuInputFormatStructMetadata::ElementID::TEX_COORD_2: d3d12Element.SemanticIndex = 2; d3d12Element.SemanticName = "TEXCOORD"; break;
            case GpuInputFormatStructMetadata::ElementID::TEX_COORD_3: d3d12Element.SemanticIndex = 3; d3d12Element.SemanticName = "TEXCOORD"; break;
            case GpuInputFormatStructMetadata::ElementID::TEX_COORD_4: d3d12Element.SemanticIndex = 4; d3d12Element.SemanticName = "TEXCOORD"; break;
            case GpuInputFormatStructMetadata::ElementID::TEX_COORD_5: d3d12Element.SemanticIndex = 5; d3d12Element.SemanticName = "TEXCOORD"; break;
            case GpuInputFormatStructMetadata::ElementID::TEX_COORD_6: d3d12Element.SemanticIndex = 6; d3d12Element.SemanticName = "TEXCOORD"; break;
            case GpuInputFormatStructMetadata::ElementID::TEX_COORD_7: d3d12Element.SemanticIndex = 7; d3d12Element.SemanticName = "TEXCOORD"; break;
        }

        switch (element.type)
        {
            case NumericStructElementType::NONE: break;
            case NumericStructElementType::INT1: d3d12Element.Format = DXGI_FORMAT_R32_SINT; break;
            case NumericStructElementType::INT2: d3d12Element.Format = DXGI_FORMAT_R32G32_SINT; break;
            case NumericStructElementType::INT3: d3d12Element.Format = DXGI_FORMAT_R32G32B32_SINT; break;
            case NumericStructElementType::INT4: d3d12Element.Format = DXGI_FORMAT_R32G32B32A32_SINT; break;
            case NumericStructElementType::UINT1: d3d12Element.Format = DXGI_FORMAT_R32_UINT; break;
            case NumericStructElementType::UINT2: d3d12Element.Format = DXGI_FORMAT_R32G32_UINT; break;
            case NumericStructElementType::UINT3: d3d12Element.Format = DXGI_FORMAT_R32G32B32_UINT; break;
            case NumericStructElementType::UINT4: d3d12Element.Format = DXGI_FORMAT_R32G32B32A32_UINT; break;
            case NumericStructElementType::FLOAT1: d3d12Element.Format = DXGI_FORMAT_R32_FLOAT; break;
            case NumericStructElementType::FLOAT2: d3d12Element.Format = DXGI_FORMAT_R32G32_FLOAT; break;
            case NumericStructElementType::FLOAT3: d3d12Element.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
            case NumericStructElementType::FLOAT4: d3d12Element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
            case NumericStructElementType::FLOAT3x3: FINJIN_SET_ERROR(error, "Failed to use 'float3x3' as input element."); return;
            case NumericStructElementType::FLOAT4x4: FINJIN_SET_ERROR(error, "Failed to use 'float4x4' as input element."); return;
            case NumericStructElementType::NESTED_STRUCT: FINJIN_SET_ERROR(error, "Failed to use 'nested struct' as input element."); return;
            default: FINJIN_SET_ERROR(error, "Unrecognized input element."); return;
        }
    }
}

#endif
