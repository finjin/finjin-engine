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
#include "ASTCReader.hpp"
#include "finjin/common/ByteOrder.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//ASTCReader::Header
TextureDimension ASTCReader::Header::GetDimension() const
{
    return TextureDimensionUtilities::Get(static_cast<size_t>(GetHeight()), static_cast<size_t>(GetDepth()), (size_t)1, (size_t)0);
}

//ASTCReader
ASTCReader::ASTCReader()
{
    FINJIN_ZERO_ITEM(this->header);
    this->swapBytes = false;
}

ASTCReader::ReadHeaderResult ASTCReader::ReadHeader(ByteBufferReader& reader)
{
    const uint32_t MAGIC_FILE_CONSTANT = 0x5CA1AB13;
    const uint32_t MAGIC_FILE_CONSTANT_REVERSE = 0x13ABA15C;

    if (reader.size_left() < sizeof(MAGIC_FILE_CONSTANT))
        return ReadHeaderResult::FAILED_TO_READ_SIGNATURE;

    //Check signature. Note that checking the signature does not modify the reader
    auto& signature = *reinterpret_cast<const uint32_t*>(reader.data_left());
    if (signature != MAGIC_FILE_CONSTANT)
    {
        //See if the reversed magic constant matches
        if (signature != MAGIC_FILE_CONSTANT_REVERSE)
            return ReadHeaderResult::INVALID_SIGNATURE;

        //Reversed magic constant matches
        this->swapBytes = true;
    }

    //Skip signature
    if (!reader.Skip(sizeof(MAGIC_FILE_CONSTANT)))
        return ReadHeaderResult::FAILED_TO_READ_SIGNATURE;

    //Read header
    if (reader.Read(&this->header, sizeof(this->header)) < sizeof(this->header))
        return ReadHeaderResult::FAILED_TO_READ_HEADER;

    //Check blocks dimensions
    //From https://github.com/ARM-software/astc-encoder/blob/master/Source/astc_toplevel.cpp, in load_astc_file()
    auto xdim = this->header.blockdim_x;
    auto ydim = this->header.blockdim_y;
    auto zdim = this->header.blockdim_z;
    if (xdim < 3 || xdim > 12 || ydim < 3 || ydim > 12 || (zdim < 3 && zdim != 1) || zdim > 12)
        return ReadHeaderResult::INVALID_BLOCK_DIMENSIONS;

    if (this->swapBytes)
    {
        //Requiring byte swapping is probably an error, but the caller might be able to handle it.
        return ReadHeaderResult::IMAGE_BYTE_SWAPPING_REQUIRED;
    }

    return ReadHeaderResult::SUCCESS;
}

Utf8String ASTCReader::GetReadHeaderResultString(ReadHeaderResult result) const
{
    switch (result)
    {
        case ReadHeaderResult::FAILED_TO_READ_SIGNATURE: return "Failed to read ASTC signature.";
        case ReadHeaderResult::INVALID_SIGNATURE: return "Invalid ASTC signature.";
        case ReadHeaderResult::FAILED_TO_READ_HEADER: return "Failed to read header.";
        case ReadHeaderResult::IMAGE_BYTE_SWAPPING_REQUIRED: return "Image byte swapping required.";
        case ReadHeaderResult::INVALID_BLOCK_DIMENSIONS: return "Invalid block dimensions.";
        default: return Utf8String::GetEmpty();
    }
}

void ASTCReader::ReadHeader(ByteBufferReader& reader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = ReadHeader(reader);
    if (result != ReadHeaderResult::SUCCESS)
        FINJIN_SET_ERROR(error, GetReadHeaderResultString(result));
}

const ASTCReader::Header& ASTCReader::GetHeader() const
{
    return this->header;
}

bool ASTCReader::IsImageByteSwapped() const
{
    return this->swapBytes;
}
