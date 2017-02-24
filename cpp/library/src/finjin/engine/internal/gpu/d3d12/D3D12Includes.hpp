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


//Includes---------------------------------------------------------------------
#include <wtypes.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "d3dx12.h"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    enum class D3D12ShaderType
    {
        VERTEX_SHADER,
        PIXEL_SHADER,
        DOMAIN_SHADER,
        GEOMETRY_SHADER,
        HULL_SHADER,

        COUNT
    };

} }
