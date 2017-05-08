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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12

#include "D3D12GpuOutput.hpp"
#include "finjin/common/Math.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static size_t Difference(size_t a, size_t b)
{
    if (a > b)
        return a - b;
    else
        return b - a;
}

static double Difference(const D3D12RefreshRate& a, const D3D12RefreshRate& b)
{
    return fabs(a.ToDouble() - b.ToDouble());
}

static float ResolutionDifference(const D3D12DisplayMode& a, const D3D12DisplayMode& b)
{
    //Use the number of power of 2 steps across the diagonal
    auto aDist = logf(sqrtf(RoundToFloat(a.width * a.width + a.height * a.height)));
    auto bDist = logf(sqrtf(RoundToFloat(b.width * b.width + b.height * b.height)));
    return fabsf(aDist - bDist);
}


//Implementation----------------------------------------------------------------
bool D3D12GpuOutput::FindBestDisplayMode(D3D12DisplayMode& result, const D3D12DisplayMode& desired) const
{
    //Try to find exact match
    for (const auto& displayMode : this->displayModes)
    {
        if (desired.IsEquivalent(D3D12DisplayMode(displayMode)))
        {
            result = desired;
            return true;
        }
    }

    //Try to find a width/height/format match
    {
        const DXGI_MODE_DESC* widthHeightFormatMatch = nullptr;
        for (size_t i = 0; i < this->displayModes.size(); i++)
        {
            const auto& displayMode = this->displayModes[i];

            if (displayMode.Width == desired.width && displayMode.Height == desired.height && displayMode.Format == desired.format)
            {
                if (widthHeightFormatMatch != nullptr)
                {
                    if (Difference(displayMode.RefreshRate, desired.refresh) < Difference(widthHeightFormatMatch->RefreshRate, desired.refresh))
                        widthHeightFormatMatch = &this->displayModes[i];
                }
                else
                    widthHeightFormatMatch = &this->displayModes[i];
            }
        }
        if (widthHeightFormatMatch != nullptr)
        {
            result = *widthHeightFormatMatch;
            return true;
        }
    }

    //Try to find a format match
    {
        const DXGI_MODE_DESC* formatMatch = nullptr;
        for (size_t i = 0; i < this->displayModes.size(); i++)
        {
            const auto& displayMode = this->displayModes[i];

            if (displayMode.Format == desired.format)
            {
                //Find closest resolution and format. Note: This isn't great
                if (formatMatch != nullptr)
                {
                    if (ResolutionDifference(displayMode, desired) <= ResolutionDifference(*formatMatch, desired) &&
                        Difference(displayMode.RefreshRate, desired.refresh) <= Difference(formatMatch->RefreshRate, desired.refresh))
                    {
                        formatMatch = &this->displayModes[i];
                    }
                }
                else
                    formatMatch = &this->displayModes[i];
            }
        }
        if (formatMatch != nullptr)
        {
            result = *formatMatch;
            return true;
        }
    }

    //Everything failed
    return false;
}

#endif
