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

    enum class VulkanShaderType
    {
        VERTEX,
        TESSELLATION_CONTROL,
        TESSELLATION_EVALUATION,
        GEOMETRY,
        FRAGMENT,
        COMPUTE,

        COUNT
    };

    struct VulkanShaderTypeUtilities
    {
        static Utf8String ToString(VulkanShaderType value)
        {
            switch (value)
            {
                case VulkanShaderType::VERTEX: return "vertex";
                case VulkanShaderType::TESSELLATION_CONTROL: return "tessellation control";
                case VulkanShaderType::TESSELLATION_EVALUATION: return "tessellation evaluation";
                case VulkanShaderType::GEOMETRY: return "geometry";
                case VulkanShaderType::FRAGMENT: return "fragment";
                case VulkanShaderType::COMPUTE: return "compute";
                default: return FINJIN_ENUM_UNKNOWN_STRING;
            }
        }
    };

} }
