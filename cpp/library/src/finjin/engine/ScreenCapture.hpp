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
#include "finjin/common/Path.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

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

    struct ScreenCaptureWriteSettings
    {
        ScreenCaptureWriteSettings();

        const ScreenCaptureWriteSettings& GetDefault();

        bool writeUnsupportedFormatAsRaw; //Indicates whether unsupported formats should fall back to being written as to a 'raw' file
        bool forceRaw; //Forces data to always be written to a 'raw' file
        ByteBuffer* tempBuffer; //Optional buffer to use when encoding intermediate data. If not large enough, writing will fail
        Allocator* tempBufferAllocator; //Optional allocator to use when creating a new temp buffer (when tempBuffer is null)
    };

    struct ScreenCapture
    {
        ScreenCapture();

        bool IsBGRPixelFormat() const;
        bool IsSRGBPixelFormat() const;

        bool IsIntPixelFormat() const;
        bool IsFloatPixelFormat() const;

        uint32_t GetChannelCount() const;
        uint32_t GetBytesPerChannel() const;
        uint32_t GetBytesPerPixel() const;

        enum class WriteResult
        {
            SUCCESS,
            UNSUPPORTED_PIXEL_FORMAT,
            FAILED_TO_APPEND_EXTENSION,
            FAILED_TO_ALLOCATE_ENCODING_BUFFER,
            FAILED_TO_ENCODE_TO_BUFFER,
            FAILED_TO_OPEN_OUTPUT_FILE,
            FAILED_TO_WRITE_ALL_DATA
        };

        WriteResult WriteToRawFile(Path& filePath) const;
        WriteResult WriteToFile(Path& filePathNoExtension, const ScreenCaptureWriteSettings& settings) const;
        Utf8String GetWriteResultString(WriteResult result) const;

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
