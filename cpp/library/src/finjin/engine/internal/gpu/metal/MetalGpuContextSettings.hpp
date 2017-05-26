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
#include "finjin/common/RequestedValue.hpp"
#include "finjin/engine/GpuContextCommonSettings.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct MetalGpuContextSettings : GpuContextCommonSettings
    {
        MetalGpuContextSettings(Allocator* initialAllocator);

        RequestedValue<uint32_t> colorFormat; //MTLPixelFormat
        RequestedValue<uint32_t> depthStencilFormat; //MTLPixelFormat

        bool stencilRequired;
    };

} }
