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
#include "finjin/common/DefaultAllocator.hpp"
#include "finjin/common/NvAndroidNativeAppGlue.h"
#include "finjin/common/SystemAllocator.hpp"


//Macros------------------------------------------------------------------------
#define FINJIN_ENGINE_MAIN(AppDelegate)\
    void android_main(android_app* androidApp)\
    {\
        void FinjinMain(ApplicationDelegate* del, android_app* androidApp);\
        \
        Finjin::Common::DefaultAllocator<Finjin::Common::SystemAllocator> defaultAllocator;\
        FinjinMain(Finjin::Common::AllocatedClass::New<AppDelegate>(defaultAllocator, FINJIN_CALLER_ARGUMENTS), androidApp);\
    }
