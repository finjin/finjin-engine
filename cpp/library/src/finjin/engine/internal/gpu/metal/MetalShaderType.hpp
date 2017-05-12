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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class MetalShaderType
    {
        VERTEX,
        TESSELLATION_CONTROL,
        TESSELLATION_EVALUATION,
        GEOMETRY,
        FRAGMENT,
        COMPUTE,

        COUNT
    };

    struct MetalShaderTypeUtilities
    {
        static const char* ToString(MetalShaderType value)
        {
            switch (value)
            {
                case MetalShaderType::VERTEX: return "vertex";
                case MetalShaderType::TESSELLATION_CONTROL: return "tessellation control";
                case MetalShaderType::TESSELLATION_EVALUATION: return "tessellation evaluation";
                case MetalShaderType::GEOMETRY: return "geometry";
                case MetalShaderType::FRAGMENT: return "fragment";
                case MetalShaderType::COMPUTE: return "compute";
                default: return "<unknown>";
            }
        }
    };

} }
