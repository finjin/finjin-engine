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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_METAL

#include "MetalDevices.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
id<MTLDevice> MetalDevices::GetByGpuID(const Utf8String& gpuID) const
{
    for (auto device : *this)
    {
        if (gpuID == device.name.UTF8String)
            return device;
    }

    return nullptr;
}

#endif
