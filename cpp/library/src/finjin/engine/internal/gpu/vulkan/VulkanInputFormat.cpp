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

#include "VulkanInputFormat.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define DEFAULT_BINDING_ID 0


//Implementation----------------------------------------------------------------
VulkanInputFormat::VulkanInputFormat()
{
    this->inputFormatHash = 0;
    this->vertexBinding.binding = DEFAULT_BINDING_ID;
}

void VulkanInputFormat::Create(const GpuInputFormatStruct& inputFormat, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->inputFormatHash = inputFormat.GetElementHash();

    if (!this->attributes.Create(inputFormat.elements.size(), allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create Vulkan input attributes collection.");
        return;
    }

    this->vertexBinding.stride = static_cast<uint32_t>(inputFormat.totalSize);
    this->vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    for (size_t elementIndex = 0; elementIndex < inputFormat.elements.size(); elementIndex++)
    {
        auto& element = inputFormat.elements[elementIndex];
        auto& vulkanAttribute = this->attributes[elementIndex];

        vulkanAttribute.location = static_cast<uint32_t>(elementIndex);
        vulkanAttribute.binding = this->vertexBinding.binding;
        vulkanAttribute.offset = static_cast<uint32_t>(element.gpuPaddedOffset);

        switch (element.type)
        {
            case NumericStructElementType::NONE: break;
            case NumericStructElementType::INT1: vulkanAttribute.format = VK_FORMAT_R32_SINT; break;
            case NumericStructElementType::INT2: vulkanAttribute.format = VK_FORMAT_R32G32_SINT; break;
            case NumericStructElementType::INT3: vulkanAttribute.format = VK_FORMAT_R32G32B32_SINT; break;
            case NumericStructElementType::INT4: vulkanAttribute.format = VK_FORMAT_R32G32B32A32_SINT; break;
            case NumericStructElementType::UINT1: vulkanAttribute.format = VK_FORMAT_R32_UINT; break;
            case NumericStructElementType::UINT2: vulkanAttribute.format = VK_FORMAT_R32G32_UINT; break;
            case NumericStructElementType::UINT3: vulkanAttribute.format = VK_FORMAT_R32G32B32_UINT; break;
            case NumericStructElementType::UINT4: vulkanAttribute.format = VK_FORMAT_R32G32B32A32_UINT; break;
            case NumericStructElementType::FLOAT1: vulkanAttribute.format = VK_FORMAT_R32_SFLOAT; break;
            case NumericStructElementType::FLOAT2: vulkanAttribute.format = VK_FORMAT_R32G32_SFLOAT; break;
            case NumericStructElementType::FLOAT3: vulkanAttribute.format = VK_FORMAT_R32G32B32_SFLOAT; break;
            case NumericStructElementType::FLOAT4: vulkanAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT; break;
            case NumericStructElementType::FLOAT3x3: FINJIN_SET_ERROR(error, "Failed to use 'float3x3' as input element."); return;
            case NumericStructElementType::FLOAT4x4: FINJIN_SET_ERROR(error, "Failed to use 'float4x4' as input element."); return;
            case NumericStructElementType::NESTED_STRUCT: FINJIN_SET_ERROR(error, "Failed to use 'nested struct' as input element."); return;
            default: FINJIN_SET_ERROR(error, "Unrecognized input element."); return;
        }
    }
}

void VulkanInputFormat::SetBinding(uint32_t binding)
{
    this->vertexBinding.binding = binding;

    for (auto& attribute : this->attributes)
        attribute.binding = binding;
}

#endif
