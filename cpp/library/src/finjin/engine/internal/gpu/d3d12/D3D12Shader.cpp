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

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
D3D12Shader::D3D12Shader(Allocator* allocator) : name(allocator)
{
}

void D3D12Shader::HandeCreationFailed()
{
    this->name.clear();
    this->bytes = ByteBufferReader();
    this->localBytes.clear();
}

#endif
