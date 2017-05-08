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
#include "D3D12Includes.hpp"
#include "D3D12ShaderType.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12Shader
    {
    public:
        D3D12Shader(Allocator* allocator);

        void Create(Allocator* allocator, const Utf8String& name, const void* bytes, size_t byteCount, bool makeLocalCopy, Error& error);
        void Destroy();

        void HandleCreationFailed();

    public:
        Utf8String name;
        ByteBufferReader bytes;
        ByteBuffer localBytes;
    };

} }
