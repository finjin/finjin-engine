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
#include "finjin/engine/VRContextCommonSettings.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OpenVRContext;

    enum class VRContextInitializationStatus
    {
        NONE,
        NO_HMD_FOUND,
        RUNTIME_NOT_INITIALIZED,
        CANCELED_BY_USER,
        ANOTHER_APP_LAUNCHING,
        INITIALIZED
    };

    struct OpenVRContextSettings : VRContextCommonSettings
    {
        OpenVRContextSettings(Allocator* initialAllocator);

        FINJIN_VR_CONTEXT_DEVICE_CALLBACKS(OpenVRContext);
    };

} }
