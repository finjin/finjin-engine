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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    enum class ScreenCaptureFrequency
    {
        NEVER,
        ON_REQUEST,
        EVERY_FRAME
    };

    enum class ScreenCaptureCapability
    {
        NONE = 0,

        /**
         * Feature provided at the system level (outside the application).
         */
        SYSTEM = 1 << 0,

        /**
         * Internally supported by the GPU subsystem.
         */
        GPU_INTERNAL = 1 << 1
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(ScreenCaptureCapability)

    enum class ScreenCapturePixelFormat
    {
        NONE,
        RGBA8_UNORM,
        RGBA8_UNORM_SRGB,
        BGRA8_UNORM,
        BGRA8_UNORM_SRGB,
        RGBA16_FLOAT,
        BGRA16_FLOAT
    };

    struct ScreenCapture
    {
        ScreenCapture()
        {
            FINJIN_ZERO_ITEM(*this);
        }

        void* image;
        ScreenCapturePixelFormat pixelFormat;
        uint32_t rowStride;
        uint32_t width;
        uint32_t height;
    };

    enum class ScreenCaptureResult
    {
        SUCCESS,
        NOT_SUPPORTED,
        NOT_AVAILABLE
    };

} }
