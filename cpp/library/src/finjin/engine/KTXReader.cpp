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
#include "KTXReader.hpp"
#include "finjin/common/Allocator.hpp"
#include "finjin/common/ByteOrder.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define KTX_IDENTIFIER_BYTE_COUNT 12
#define KTX_ENDIAN_REF 0x04030201
#define KTX_ENDIAN_REF_REV 0x01020304


//Implementation----------------------------------------------------------------

//KTXReader::Header
TextureDimension KTXReader::Header::GetDimension() const
{
    return TextureDimensionUtilities::Get(this->height, static_cast<size_t>(this->depth), static_cast<size_t>(this->numberOfFaces), static_cast<size_t>(this->numberOfArrayElements));
}

//KTXReader
KTXReader::KTXReader()
{
    FINJIN_ZERO_ITEM(this->header);
}

KTXReader::ReadHeaderResult KTXReader::ReadHeader(ByteBufferReader& reader)
{
    static const uint8_t KTX_IDENTIFIER[KTX_IDENTIFIER_BYTE_COUNT] = { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };

    if (reader.size_left() < KTX_IDENTIFIER_BYTE_COUNT)
        return ReadHeaderResult::FAILED_TO_READ_SIGNATURE;

    //Check signature. Note that checking the signature does not modify the reader
    if (memcmp(reader.data_left(), KTX_IDENTIFIER, KTX_IDENTIFIER_BYTE_COUNT) != 0)
        return ReadHeaderResult::INVALID_SIGNATURE;

    //Skip signature
    if (!reader.Skip(KTX_IDENTIFIER_BYTE_COUNT))
        return ReadHeaderResult::FAILED_TO_READ_SIGNATURE;

    //Read header
    if (reader.Read(&this->header, sizeof(this->header)) < sizeof(this->header))
        return ReadHeaderResult::FAILED_TO_READ_HEADER;

    //Swap bytes if necessary
    if (this->header.endianness == KTX_ENDIAN_REF_REV)
    {
        //Requiring byte swapping is probably an error, but the caller might be able to handle it.
        SwapBytes(this->header.glType);
        SwapBytes(this->header.glTypeSize);
        SwapBytes(this->header.glFormat);
        SwapBytes(this->header.glInternalFormat);
        SwapBytes(this->header.glBaseInternalFormat);
        SwapBytes(this->header.width);
        SwapBytes(this->header.height);
        SwapBytes(this->header.depth);
        SwapBytes(this->header.numberOfArrayElements);
        SwapBytes(this->header.numberOfFaces);
        SwapBytes(this->header.numberOfMipmapLevels);
        SwapBytes(this->header.bytesOfKeyValueData);
    }

    //Skip key/value data
    if (!reader.Skip(this->header.bytesOfKeyValueData))
        return ReadHeaderResult::FAILED_TO_SKIP_KEY_VALUE_DATA;

    if (this->header.endianness == KTX_ENDIAN_REF_REV)
    {
        //Requiring byte swapping is probably an error, but the caller might be able to handle it.
        return ReadHeaderResult::IMAGE_BYTE_SWAPPING_REQUIRED;
    }

    return ReadHeaderResult::SUCCESS;
}

Utf8String KTXReader::GetReadHeaderResultString(ReadHeaderResult result) const
{
    switch (result)
    {
        case ReadHeaderResult::FAILED_TO_READ_SIGNATURE: return "Failed to read KTX signature.";
        case ReadHeaderResult::INVALID_SIGNATURE: return "Invalid KTX signature.";
        case ReadHeaderResult::FAILED_TO_READ_HEADER: return "Failed to read header.";
        case ReadHeaderResult::FAILED_TO_SKIP_KEY_VALUE_DATA: return "Failed to skip key/value data.";
        case ReadHeaderResult::IMAGE_BYTE_SWAPPING_REQUIRED: return "Image byte swapping required.";
        default: return Utf8String::GetEmpty();
    }
}

void KTXReader::ReadHeader(ByteBufferReader& reader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadHeader(reader);
    if (result != ReadHeaderResult::SUCCESS)
        FINJIN_SET_ERROR(error, GetReadHeaderResultString(result));
}

const KTXReader::Header& KTXReader::GetHeader() const
{
    return this->header;
}

bool KTXReader::IsImageByteSwapped() const
{
    return this->header.endianness == KTX_ENDIAN_REF_REV;
}

size_t KTXReader::ReadImages(ByteBufferReader& reader, ImageFunction imageFunction, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto readerStart = reader.data_left();

    auto mipMapCount = this->header.numberOfMipmapLevels;
    if (mipMapCount == 0)
        mipMapCount = 1;

    auto arraySize = this->header.numberOfArrayElements;
    if (arraySize == 0)
        arraySize = 1;

    auto width = this->header.width;
    auto height = this->header.height;
    auto depth = this->header.depth;

    auto isNonArrayCubeMap = this->header.numberOfArrayElements == 0 && this->header.numberOfFaces == 6;

    for (uint32_t mipLevel = 0; mipLevel < mipMapCount; mipLevel++)
    {
        if (width == 0)
            width = 1;
        if (height == 0)
            height = 1;
        if (depth == 0)
            depth = 1;

        uint32_t imageSizeBytes = 0;
        auto amountRead = reader.ReadAndSwap(&imageSizeBytes, sizeof(imageSizeBytes), this->header.endianness == KTX_ENDIAN_REF_REV);
        if (amountRead < sizeof(imageSizeBytes))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read image byte size at mip level '%1%'.", mipLevel));
            return 0;
        }

        for (uint32_t arrayItemIndex = 0; arrayItemIndex < arraySize; arrayItemIndex++)
        {
           for (uint32_t faceIndex = 0; faceIndex < this->header.numberOfFaces; faceIndex++)
           {
               for (uint32_t depthIndex = 0; depthIndex < depth; depthIndex++)
               {
                    if (imageFunction != nullptr && !imageFunction(this, mipLevel, arrayItemIndex, faceIndex, depthIndex, width, height, imageSizeBytes, reader.data_left()))
                        return reader.data_left() - readerStart;

                    if (!reader.Skip(imageSizeBytes))
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read mip level '%1%', array item '%2%', face index '%3%', depth index '%4%'.", mipLevel, arrayItemIndex, faceIndex, depthIndex));
                        return 0;
                    }
               }

               //Skip non-array cube map padding
               if (isNonArrayCubeMap)
               {
                   auto skipAmount = Allocator::AlignMemoryUp(reader.data_left(), 4) - reader.data_left();
                   if (!reader.Skip(skipAmount))
                   {
                       FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to skip non-array-cube-map padding at mip level '%1%', array item '%2%', face index '%3%'.", mipLevel, arrayItemIndex, faceIndex));
                       return 0;
                   }
               }
           }
        }

        //Skip mip padding
        {
            auto skipAmount = Allocator::AlignMemoryUp(reader.data_left(), 4) - reader.data_left();
            if (!reader.Skip(skipAmount))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to skip mip padding at mip level '%1%'.", mipLevel));
                return 0;
            }
        }

        //Move to next mip
        width /= 2;
        height /= 2;
        depth /= 2;
    }

    return reader.data_left() - readerStart;
}
