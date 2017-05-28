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
#include "finjin/common/Allocator.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/FileAccessor.hpp"
#include "finjin/common/PNGWriter.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//ScreenCaptureWriteSettings
ScreenCaptureWriteSettings::ScreenCaptureWriteSettings()
{
    this->writeUnsupportedFormatAsRaw = false;
    this->forceRaw = false;
    this->tempBuffer = nullptr;
    this->newBufferAllocator = FINJIN_ALLOCATOR_NULL;
}

const ScreenCaptureWriteSettings& ScreenCaptureWriteSettings::GetDefault()
{
    static ScreenCaptureWriteSettings value;
    return value;
}

//ScreenCapture
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

uint32_t ScreenCapture::GetBytesPerPixel() const
{
    return GetChannelCount() * GetBytesPerChannel();
}

ScreenCapture::WriteResult ScreenCapture::WriteToRawFile(Path& filePath) const
{
    auto result = WriteResult::UNSUPPORTED_PIXEL_FORMAT;

    if (!filePath.append("raw").HasError())
    {
        FileAccessor file;
        if (file.OpenForWrite(filePath))
        {
            uint32_t signature = FINJIN_SIGNATURE_FOURCC;
            uint32_t width = this->width;
            uint32_t height = this->height;
            uint32_t format = static_cast<uint32_t>(this->pixelFormat);

            file.Write(&signature, sizeof(signature));
            file.Write(&width, sizeof(width));
            file.Write(&height, sizeof(height));
            file.Write(&format, sizeof(format));

            auto pixels = static_cast<const uint8_t*>(this->image);
            for (size_t y = 0; y < this->height; y++)
            {
                auto byteCount = width * height * GetBytesPerPixel();
                if (file.Write(pixels, byteCount) != byteCount)
                    result = WriteResult::FAILED_TO_WRITE_ALL_DATA;
                pixels += this->rowStride;
            }

            result = WriteResult::SUCCESS;
        }
        else
            result = WriteResult::FAILED_TO_OPEN_OUTPUT_FILE;
    }
    else
        result = WriteResult::FAILED_TO_APPEND_EXTENSION;

    return result;
}

ScreenCapture::WriteResult ScreenCapture::WriteToFile(Path& filePath, const ScreenCaptureWriteSettings& settings) const
{
    auto result = WriteResult::UNSUPPORTED_PIXEL_FORMAT;

    if (settings.forceRaw)
        result = WriteToRawFile(filePath);
    else if (IsIntPixelFormat())
    {
        PNGWriter pngWriter;

        if (IsBGRPixelFormat())
            pngWriter.SetReverseRGB(true);

        if (IsSRGBPixelFormat())
            pngWriter.SetSRGB(true);

        pngWriter.SetChannelCount(GetChannelCount());
        pngWriter.SetBytesPerChannel(GetBytesPerChannel());
        
        auto outputBufferSize = this->width * this->height * GetChannelCount() * 2; //Allocate enough for PNG header and temp data
        ByteBuffer* pngOutputBuffer = nullptr;
        ByteBuffer tempOutputBuffer;        
        if (settings.tempBuffer != nullptr)
        {
            if (settings.tempBuffer->resize(outputBufferSize) == outputBufferSize)
                pngOutputBuffer = settings.tempBuffer;
        }
        else
        {
            if (tempOutputBuffer.Create(outputBufferSize, settings.tempBufferAllocator))
                pngOutputBuffer = &tempOutputBuffer;                
        }

        if (pngOutputBuffer != nullptr)
        {
            auto writeResult = pngWriter.Write(this->image, this->width, this->height, this->rowStride, *pngOutputBuffer);
            if (writeResult == PNGWriter::WriteResult::SUCCESS)                
            {
                if (!filePath.append("png").HasError())
                {
                    FileAccessor file;
                    if (file.OpenForWrite(filePath))
                    {
                        if (file.Write(pngOutputBuffer->data(), pngOutputBuffer->size()) == pngOutputBuffer->size())
                            result = WriteResult::SUCCESS;
                        else
                            result = WriteResult::FAILED_TO_WRITE_ALL_DATA;
                    }
                    else
                        result = WriteResult::FAILED_TO_OPEN_OUTPUT_FILE;
                }
                else
                    result = WriteResult::FAILED_TO_APPEND_EXTENSION;
            }
            else
                result = WriteResult::FAILED_TO_ENCODE_TO_BUFFER;
        }
        else
            result = WriteResult::FAILED_TO_ALLOCATE_ENCODING_BUFFER;
    }

    if (result == WriteResult::UNSUPPORTED_PIXEL_FORMAT && settings.writeUnsupportedFormatAsRaw)
        result = WriteToRawFile(filePath);

    return result;
}

Utf8String ScreenCapture::GetWriteResultString(WriteResult result) const
{
    switch (result)
    {
        case WriteResult::UNSUPPORTED_PIXEL_FORMAT: return "Unsupported screen capture pixel format.";
        case WriteResult::FAILED_TO_APPEND_EXTENSION: return "Failed to append extension to file path.";
        case WriteResult::FAILED_TO_ALLOCATE_ENCODING_BUFFER: return "Failed to allocate intermediate encoding buffer.";
        case WriteResult::FAILED_TO_ENCODE_TO_BUFFER: return "Failed to allocate intermediate encoding buffer.";
        case WriteResult::FAILED_TO_OPEN_OUTPUT_FILE: return "Failed to open output file.";
        case WriteResult::FAILED_TO_WRITE_ALL_DATA: return "Failed to write all data to file.";
        default: return Utf8String::GetEmpty();
    }
}
