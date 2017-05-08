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
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/gl_format.h"
#include "finjin/engine/TextureDimension.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    //https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
    class KTXReader
    {
    public:
    #pragma pack(push, 1)
        //Data members are from https://github.com/KhronosGroup/KTX/blob/master/lib/ktxint.h, in struct KTX_header_t, minus 'identifier' member
        struct Header
        {
            TextureDimension GetDimension() const;

            uint32_t endianness;
            uint32_t glType; //For compressed textures, must equal 0. For uncompressed textures, usually one of the values from table 8.2 of the OpenGL 4.4 specification (UNSIGNED_BYTE, UNSIGNED_SHORT_5_6_5, etc.)
            uint32_t glTypeSize; //Specifies the data type size that should be used when endianness conversion is required for the texture data stored in the file. If glType is not 0, this should be the size in bytes corresponding to glType. For texture data which does not depend on platform endianness, including compressed texture data, glTypeSize must equal 1.
            uint32_t glFormat; //For compressed textures, must equal 0. For uncompressed textures, specifies one of the values from table 8.3 of the OpenGL 4.4 specification (RGB, RGBA, BGRA, etc.)
            uint32_t glInternalFormat; //For compressed textures, must equal the compressed internal format, usually one of the values from table 8.14 of the OpenGL 4.4 specification. For uncompressed textures, usually one of the sized internal formats from tables 8.12 & 8.13 of the OpenGL 4.4 specification.
            uint32_t glBaseInternalFormat; //For both compressed and uncompressed textures, specifies the base internal format of the texture, usually one of the values from table 8.11 of the OpenGL 4.4 specification (RGB, RGBA, ALPHA, etc.).
            uint32_t width; //Width of texture image for mip level 0
            uint32_t height; //Height of texture image for mip level 0. Must be 0 for 1D textures.
            uint32_t depth; //Depth of texture image for mip level 0. Must be 0 for 1D and 2D textures.
            uint32_t numberOfArrayElements; //The number of array elements. If the texture is not an array texture, must equal 0.
            uint32_t numberOfFaces; //The number of cubemap faces. For cubemaps and cubemap arrays this should be 6. For non cubemaps this should be 1. Cube map faces are stored in the order: +X, -X, +Y, -Y, +Z, -Z.
            uint32_t numberOfMipmapLevels; //numberOfMipmapLevels must equal 1 for non-mipmapped textures. For mipmapped textures, it equals the number of mipmaps. Mipmaps are stored in order from largest size to smallest size.
            uint32_t bytesOfKeyValueData;
        };
    #pragma pack(pop)

        KTXReader();

        enum class ReadHeaderResult
        {
            SUCCESS,
            FAILED_TO_READ_SIGNATURE,
            INVALID_SIGNATURE,
            FAILED_TO_READ_HEADER,
            FAILED_TO_SKIP_KEY_VALUE_DATA,
            IMAGE_BYTE_SWAPPING_REQUIRED
        };
        ReadHeaderResult ReadHeader(ByteBufferReader& reader);
        Utf8String GetReadHeaderResultString(ReadHeaderResult result) const;

        void ReadHeader(ByteBufferReader& reader, Error& error);

        const Header& GetHeader() const;

        bool IsImageByteSwapped() const;

        //Return: true to continue reading mips, false to stop
        //Parameters: const KTXReader* reader, uint32_t mipLevel, uint32_t surfaceIndex, uint32_t faceIndex, uint32_t depthIndex, uint32_t width, uint32_t height, uint32_t imageByteCount, const void* imageBytes
        using ImageFunction = std::function<bool(const KTXReader*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const void*)>;

        size_t ReadImages(ByteBufferReader& reader, ImageFunction imageFunction, Error& error);

    private:
        Header header;
    };

} }
