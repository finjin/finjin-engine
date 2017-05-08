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

#include "D3D12Shader.hpp"
#include "finjin/common/Error.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
D3D12Shader::D3D12Shader(Allocator* allocator) : name(allocator)
{
}

void D3D12Shader::Create(Allocator* allocator, const Utf8String& name, const void* bytes, size_t byteCount, bool makeLocalCopy, Error& error)
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
}

void D3D12Shader::Destroy()
{
    this->name.clear();

    this->bytes = ByteBufferReader();

    this->localBytes.clear();
}

void D3D12Shader::HandleCreationFailed()
{
    this->name.clear();
    this->bytes = ByteBufferReader();
    this->localBytes.clear();
}

#endif
