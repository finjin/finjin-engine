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
#include "finjin/engine/GpuRenderTargetSize.hpp"
#include "finjin/engine/OSWindow.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
GpuRenderTargetSize::GpuRenderTargetSize()
{
    this->sizeType = GpuRenderTargetSizeType::WINDOW_SIZE;
    this->explicitWidth = 256;
    this->explicitHeight = 256;
    this->minScale = 1;
    this->maxScale = 1;
}

GpuRenderTargetSizeType GpuRenderTargetSize::GetType() const
{
    return this->sizeType;
}

void GpuRenderTargetSize::SetType(GpuRenderTargetSizeType value)
{
    this->sizeType = value;
}

double GpuRenderTargetSize::GetMinScale() const
{
    return this->minScale;
}

double GpuRenderTargetSize::GetMaxScale() const
{
    return this->maxScale;
}

void GpuRenderTargetSize::SetScale(double minScale, double maxScale)
{
    this->minScale = std::min(minScale, maxScale);
    this->maxScale = std::max(minScale, maxScale);
}

std::array<uint32_t, 2> GpuRenderTargetSize::Evaluate(OSWindow* osWindow, const WindowBounds* renderBounds, double scale)
{
    Limit(scale, this->minScale, this->maxScale);

    std::array<uint32_t, 2> result;

    if (this->sizeType == GpuRenderTargetSizeType::EXPLICIT_SIZE)
    {
        result[0] = RoundToUInt32(scale * this->explicitWidth);
        result[1] = RoundToUInt32(scale * this->explicitHeight);
    }
    else //if (this->sizeType == GpuRenderTargetSizeType::WINDOW_SIZE)
    {
        if (renderBounds != nullptr)
        {
            result[0] = RoundToUInt32(scale * renderBounds->GetClientWidth());
            result[1] = RoundToUInt32(scale * renderBounds->GetClientHeight());
        }
        else if (osWindow != nullptr)
        {
            auto renderBounds = osWindow->GetWindowSize().GetSafeCurrentBounds();
            renderBounds.Scale(osWindow->GetDisplayDensity());

            result[0] = RoundToUInt32(scale * renderBounds.GetClientWidth());
            result[1] = RoundToUInt32(scale * renderBounds.GetClientHeight());
        }
        else
        {
            result[0] = 0;
            result[1] = 0;
        }
    }

    return result;
}

std::array<uint32_t, 2> GpuRenderTargetSize::EvaluateMax(OSWindow* osWindow, const WindowBounds* renderBounds)
{
    return Evaluate(osWindow, renderBounds, this->maxScale);
}
