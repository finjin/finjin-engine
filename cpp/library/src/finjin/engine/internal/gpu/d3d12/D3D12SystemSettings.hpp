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
#include "finjin/common/Error.hpp"
#include "finjin/engine/GpuSystemCommonSettings.hpp"
#include <d3dcommon.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct D3D12SystemSettings : GpuSystemCommonSettings
    {
        D3D12SystemSettings();

        Utf8String applicationName;
        enum class SoftwareGpuRequirement
        {
            DISALLOW,
            ALLOW,
            REQUIRE
        };
        SoftwareGpuRequirement softwareGpuRequirement; //Indicates whether the software GPU is expected
        bool enableDebug;
        D3D_FEATURE_LEVEL d3dMinimumFeatureLevel;
    };

} }
