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
#include "finjin/common/Allocator.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/engine/AssetClassFileReader.hpp"
#include "finjin/engine/AssetReference.hpp"
#include "MetalIncludes.hpp"
#include "MetalShaderType.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalShaderLibrary
    {
    public:
        MetalShaderLibrary();

        void Create(id<MTLDevice> device, const AssetReference& shaderFileAssetReference, AssetClassFileReader& assetClassFileReader, ByteBuffer& readBuffer, Allocator* allocator, Error& error);
        void Destroy();

    public:
        ByteBuffer fileBytes;
        dispatch_data_t dispatchData;
        id<MTLLibrary> metalLibrary;
    };

    class MetalShader
    {
    public:
        MetalShader(Allocator* allocator);

        void Create(id<MTLLibrary> shaderLibrary, const Utf8String& functionName, Error& error);
        void Destroy();

        void HandleCreationFailed();

    public:
        Utf8String functionName;

        id<MTLFunction> metalFunction;
    };

} }
