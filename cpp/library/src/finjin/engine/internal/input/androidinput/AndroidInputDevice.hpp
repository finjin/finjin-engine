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
#include <android/input.h>
#include "finjin/common/NvAndroidNativeAppGlue.h"
#include "finjin/engine/GenericInputDevice.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class AndroidInputDevice : public GenericInputDeviceImpl
    {
    public:
        using Super = GenericInputDeviceImpl;

        AndroidInputDevice();

        void Reset();

        int GetID() const;

        size_t GetLocatorCount() const;
        InputLocator* GetLocator(size_t locatorIndex);

        static void SimplifyButtonName(Utf8String& name);
        static void SimplifyAxisName(Utf8String& name);

    protected:
        int id;

        bool isVirtual;
    };

} }
