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
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/engine/SoundFormat.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct SoundConstants
    {
        enum { DEFAULT_SOURCE_PRIORITY = 128 }; //Default priority of sound sources
    };

    enum class Sound3DQuality
    {
        LOW,
        MEDIUM,
        HIGH
    };

    enum class SoundSourceState
    {
        NONE = 0, //If this is the state, the source doesn't have a buffer

        VIRTUALIZED = 1 << 0, //If not set, then the source is "realized"

        PLAYING = 1 << 1, //Source is playing at a high level
        PAUSED = 1 << 2,
        STOPPED = 1 << 3,

        LOOPING = 1 << 4 //Source is in a looping state
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(SoundSourceState)

} }
