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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/StaticVector.hpp"
#include "MetalSystemSettings.hpp"
#include "MetalDevices.hpp"
#include "MetalGpuDescription.hpp"
#include "MetalGpuContext.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalSystemImpl : public AllocatedClass
    {
    public:
        MetalSystemImpl(Allocator* allocator);

        MetalDevices& GetDevices();

        id<MTLDevice> GetBestMetalDevice(Utf8String& gpuID, MetalGpuDescription& deviceDescription, Error& error);

    public:
        MetalSystemSettings settings;

        MetalDevices devices;
        MetalHardwareGpuDescriptions deviceDescriptions;
        MetalSoftwareGpuDescriptions softwareDeviceDescriptions;

        StaticVector<MetalGpuContext*, EngineConstants::MAX_WINDOWS> contexts; //The actual objects
    };

} }