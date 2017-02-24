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
#include "finjin/common/Vector.hpp"
#include "D3D12DisplayMode.hpp"
#include "D3D12RefreshRate.hpp"
#include <dxgi1_4.h>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    using DXGI_MODE_DESCs = StaticVector<DXGI_MODE_DESC, 100>;

    class D3D12GpuOutput
    {
    public:
        bool FindBestDisplayMode(D3D12DisplayMode& result, const D3D12DisplayMode& desired) const;

    public:
        DXGI_OUTPUT_DESC desc;
        DXGI_FORMAT displayModeFormat;
        DXGI_MODE_DESCs displayModes;
    };

    using D3D12GpuOutputs = StaticVector<D3D12GpuOutput, EngineConstants::MAX_DISPLAYS>;

} }
