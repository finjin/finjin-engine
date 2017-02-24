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
#include "finjin/common/EnumBitwise.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    enum class AssetCreationCapability
    {
        NONE = 0,

        MAIN_THREAD = 1 << 0, //Some or all of the asset creation is done on the main thread
        FRAME_THREAD = 1 << 1, //Some or all of the asset creation is done on the frame thread
        
        TWO_STEP = MAIN_THREAD | FRAME_THREAD //Some asset creation is done on main thread, some on frame thread
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(AssetCreationCapability)
    
} }
