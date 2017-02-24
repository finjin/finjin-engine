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

#include "D3D12SystemSettings.hpp"
#include "D3D12Utilities.hpp"

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
D3D12SystemSettings::D3D12SystemSettings()
{
    this->softwareGpuRequirement = SoftwareGpuRequirement::DISALLOW;

    this->enableDebug = FINJIN_DEBUG ? true : false;

    this->d3dMinimumFeatureLevel = D3D_FEATURE_LEVEL_11_1;// D3D_FEATURE_LEVEL_12_0;
}

void D3D12SystemSettings::SetMinimumFeatureLevel(const Utf8String& s, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->d3dMinimumFeatureLevel = D3D12Utilities::ParseFeatureLevel(s, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to parse Direct3D 12 feature level.");
        return;
    }
}

#endif
