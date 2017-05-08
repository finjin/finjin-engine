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
#include "finjin/common/PassthroughSystemAllocator.hpp"


//Macros------------------------------------------------------------------------
#define FINJIN_ENGINE_MAIN(AppDelegate)\
    [Platform::MTAThread]\
    int main(Platform::Array<Platform::String^>^ args)\
    {\
        int FinjinMain(ApplicationDelegate* applicationDelegate, Platform::Array<Platform::String^>^ args);\
        \
        Finjin::Common::DefaultAllocator<Finjin::Common::PassthroughSystemAllocator> defaultAllocator;\
        return FinjinMain(Finjin::Common::AllocatedClass::New<AppDelegate>(defaultAllocator, FINJIN_CALLER_ARGUMENTS), args);\
    }
