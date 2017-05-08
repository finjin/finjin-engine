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
#include "finjin/common/PNGReader.hpp"
#include "finjin/engine/TextureDimension.hpp"
#include "MetalIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalTexture
    {
    public:
        MetalTexture(Allocator* allocator);

        void CreateFromPNG
            (
            const PNGReader& pngReader,
            id<MTLDevice> device,
            const void* bytes,
            size_t byteCount,
            size_t maxDimension,
            Error& error
            );

        void Destroy();

        void HandleCreationFailure();

    public:
        id<MTLTexture> resource;
        MTLTextureDescriptor* resourceDescriptor;

        Utf8String name;
        size_t textureIndex;

        TextureDimension textureDimension;
    };

} }
