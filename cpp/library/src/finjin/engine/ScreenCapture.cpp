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
#include "ScreenCapture.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
ScreenCapture::ScreenCapture()
{
    FINJIN_ZERO_ITEM(*this);
}

bool ScreenCapture::IsBGRPixelFormat() const
{
    return
        this->pixelFormat == ScreenCapturePixelFormat::BGRA8_UNORM ||
        this->pixelFormat == ScreenCapturePixelFormat::BGRA8_UNORM_SRGB
        ;
}

bool ScreenCapture::IsSRGBPixelFormat() const
{
    return
        this->pixelFormat == ScreenCapturePixelFormat::RGBA8_UNORM_SRGB ||
        this->pixelFormat == ScreenCapturePixelFormat::BGRA8_UNORM_SRGB
        ;
}

bool ScreenCapture::IsIntPixelFormat() const
{
    switch (this->pixelFormat)
    {
        case ScreenCapturePixelFormat::RGBA8_UNORM:
        case ScreenCapturePixelFormat::RGBA8_UNORM_SRGB:
        case ScreenCapturePixelFormat::BGRA8_UNORM:
        case ScreenCapturePixelFormat::BGRA8_UNORM_SRGB: return true;
        default: return false;
    }
}

bool ScreenCapture::IsFloatPixelFormat() const
{
    switch (this->pixelFormat)
    {
        case ScreenCapturePixelFormat::RGBA16_FLOAT:
        case ScreenCapturePixelFormat::BGRA16_FLOAT: return true;
        default: return false;
    }
}

uint32_t ScreenCapture::GetChannelCount() const
{
    return 4;
}

uint32_t ScreenCapture::GetBytesPerChannel() const
{
    switch (this->pixelFormat)
    {
        case ScreenCapturePixelFormat::RGBA8_UNORM:
        case ScreenCapturePixelFormat::RGBA8_UNORM_SRGB:
        case ScreenCapturePixelFormat::BGRA8_UNORM:
        case ScreenCapturePixelFormat::BGRA8_UNORM_SRGB: return 1;
        case ScreenCapturePixelFormat::RGBA16_FLOAT:
        case ScreenCapturePixelFormat::BGRA16_FLOAT: return 2;
        default: return 0;
    }
}
