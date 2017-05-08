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


//Implementation----------------------------------------------------------------
bool PlatformCapabilities::GetDisplays(DisplayInfos& displays)
{
    //Cached displays are always used
    displays = this->cachedDisplays;
    return !displays.empty();
}

float PlatformCapabilities::DipsToPixels(float dips, float dpi)
{
    static const float dipsPerInch = 160.0f;
    return floorf(dips * dpi / dipsPerInch + 0.5f); //Round to nearest integer
}

float PlatformCapabilities::PixelsToDips(float pixels, float dpi)
{
    static const float dipsPerInch = 160.0f;
    return floorf(pixels * dipsPerInch / dpi + 0.5f); //Round to nearest integer
}
