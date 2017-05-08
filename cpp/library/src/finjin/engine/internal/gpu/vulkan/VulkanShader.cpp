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

#include "VulkanShader.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanShader::VulkanShader(Allocator* allocator) : name(allocator)
{
    this->shaderModule = VK_NULL_HANDLE;
}

void VulkanShader::Create(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, Allocator* allocator, const Utf8String& name, const void* bytes, size_t byteCount, bool makeLocalCopy, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->name.assign(name).HasError())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign shader name for '%1%'.", name));
        return;
    }

    if (makeLocalCopy)
    {
        if (!this->localBytes.Create(byteCount, allocator))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate shader buffer for '%1%'.", name));
            return;
        }
        FINJIN_COPY_MEMORY(this->localBytes.data(), bytes, byteCount);
        this->bytes = ByteBufferReader(localBytes);
    }
    else
        this->bytes = ByteBufferReader(bytes, byteCount);

    VulkanShaderModuleCreateInfo createInfo(bytes, byteCount);
    auto result = vk.CreateShaderModule(vk.device, &createInfo, allocationCallbacks, &this->shaderModule);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create shader module for '%1%'.", name));
        return;
    }
}

void VulkanShader::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    this->name.clear();

    this->bytes = ByteBufferReader();

    this->localBytes.clear();

    if (this->shaderModule != VK_NULL_HANDLE)
    {
        vk.DestroyShaderModule(vk.device, this->shaderModule, allocationCallbacks);
        this->shaderModule = VK_NULL_HANDLE;
    }
}

void VulkanShader::HandleCreationFailed(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    Destroy(vk, allocationCallbacks);
}

#endif
