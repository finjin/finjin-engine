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
#if defined(_WIN32)
    #include <al.h>
    #include <alc.h>
    #include <efx.h>
#elif __APPLE__
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
    #include "efx-copy.h"
#elif defined(__linux)
    #include <AL/al.h>
    #include <AL/alc.h>
    #include <AL/efx.h>
#else
    #include <al.h>
    #include <alc.h>
    #include <efx.h>
#endif


//Macros------------------------------------------------------------------------
#define FINJIN_OPENAL_MAX_SOURCE_IDS 512

#define FINJIN_OPENAL_DEFAULT_FORWARD Finjin::Common::MathVector3(0, 0, -1)
#define FINJIN_OPENAL_DEFAULT_UP Finjin::Common::MathVector3(0, 1, 0)
