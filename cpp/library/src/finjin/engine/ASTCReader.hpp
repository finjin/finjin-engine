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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/TextureDimension.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class ASTCReader
    {
    public:
    #pragma pack(push, 1)
        //Data members are from https://github.com/ARM-software/astc-encoder/blob/master/Source/astc_toplevel.cpp, in struct astc_header, minus 'magic' member
        struct Header
        {
            TextureDimension GetDimension() const;

            uint32_t GetWidth() const { return this->xsize[0] + 256 * this->xsize[1] + 65536 * this->xsize[2]; }
            uint32_t GetHeight() const { return this->ysize[0] + 256 * this->ysize[1] + 65536 * this->ysize[2]; }
            uint32_t GetDepth() const { return this->zsize[0] + 256 * this->zsize[1] + 65536 * this->zsize[2]; }

            uint8_t blockdim_x;
            uint8_t blockdim_y;
            uint8_t blockdim_z;
            uint8_t xsize[3]; //24-bit int
            uint8_t ysize[3]; //24-bit int
            uint8_t zsize[3]; //24-bit int
        };
    #pragma pack(pop)

        ASTCReader();

        enum class ReadHeaderResult
        {
            SUCCESS,
            FAILED_TO_READ_SIGNATURE,
            INVALID_SIGNATURE,
            FAILED_TO_READ_HEADER,
            IMAGE_BYTE_SWAPPING_REQUIRED,
            INVALID_BLOCK_DIMENSIONS
        };
        ReadHeaderResult ReadHeader(ByteBufferReader& reader);
        Utf8String GetReadHeaderResultString(ReadHeaderResult result) const;

        void ReadHeader(ByteBufferReader& reader, Error& error);

        const Header& GetHeader() const;

        bool IsImageByteSwapped() const;

    private:
        Header header;
        bool swapBytes;
    };

} }
