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
#include "finjin/common/Utf8String.hpp"


//Macros------------------------------------------------------------------------
#if defined(DOMAIN)
    #undef DOMAIN
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class D3D12ShaderType
    {
        VERTEX,
        PIXEL,
        DOMAIN,
        HULL,
        GEOMETRY,
        COMPUTE,

        COUNT
    };

    struct D3D12ShaderTypeUtilities
    {
        static Utf8String ToString(D3D12ShaderType value)
        {
            switch (value)
            {
                case D3D12ShaderType::VERTEX: return "vertex";
                case D3D12ShaderType::PIXEL: return "pixel";
                case D3D12ShaderType::DOMAIN: return "domain";
                case D3D12ShaderType::HULL: return "hull";
                case D3D12ShaderType::GEOMETRY: return "geometry";
                case D3D12ShaderType::COMPUTE: return "compute";
                default: return "<unknown>";
            }
        }
    };

} }
