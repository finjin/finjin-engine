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
#include "finjin/common/ByteBuffer.hpp"
#include "VulkanIncludes.hpp"
#include "VulkanShaderType.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanShader
    {
    public:
        VulkanShader(Allocator* allocator);

        void Create(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks, Allocator* allocator, const Utf8String& name, const void* bytes, size_t byteCount, bool makeLocalCopy, Error& error);
        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        void HandleCreationFailed(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

    public:
        Utf8String name;
        ByteBufferReader bytes;
        ByteBuffer localBytes;
        VkShaderModule shaderModule;
    };

} }
