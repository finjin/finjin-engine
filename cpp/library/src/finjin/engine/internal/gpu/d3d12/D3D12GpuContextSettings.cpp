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

#include "D3D12GpuContextSettings.hpp"
#include "D3D12Includes.hpp"

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Implementation---------------------------------------------------------
D3D12GpuContextSettings::D3D12GpuContextSettings(Allocator* allocator) : GpuContextCommonSettings(allocator)
{
    this->useSoftwareGpu = false;
    this->colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    this->depthStencilFormat = DXGI_FORMAT_D32_FLOAT;    
}

#endif
