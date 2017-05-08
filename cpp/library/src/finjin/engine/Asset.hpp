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
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/AssetClass.hpp"


//Macros------------------------------------------------------------------------
#define FINJIN_ASSET_CLASS //Macro used to mark classes. Does nothing except make it easy to search through code


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    //Generic pointer to corresponding "hardware" asset, if there is one
    typedef void VoidHardwareAsset;

} }
