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
#include "finjin/engine/PlatformCapabilities.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define UWP_DIPS_PER_INCH 96.0f


//Implementation----------------------------------------------------------------
bool PlatformCapabilities::GetDisplays(DisplayInfos& displays)
{
    //Cached displays are always used
    displays = this->cachedDisplays;
    return !displays.empty();
}

float PlatformCapabilities::DipsToPixels(float dips, float dpi)
{
    return roundf(dips * dpi / UWP_DIPS_PER_INCH);
}

float PlatformCapabilities::PixelsToDips(float pixels, float dpi)
{
    return roundf(pixels * UWP_DIPS_PER_INCH / dpi);
}
