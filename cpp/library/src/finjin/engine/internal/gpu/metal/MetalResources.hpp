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
#include "finjin/common/DynamicUnorderedMap.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Hash.hpp"
#include "finjin/common/UsableDynamicVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "MetalInputFormat.hpp"
#include "MetalLight.hpp"
#include "MetalMaterial.hpp"
#include "MetalMesh.hpp"
#include "MetalShader.hpp"
#include "MetalTexture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalAssetResources
    {
    public:
        MetalAssetResources();

        void Destroy();

        void ValidateTextureForCreation(const Utf8String& name, Error& error);
        MetalTexture* GetTextureByName(const Utf8String& name);

        void ValidateMeshForCreation(const Utf8String& name, Error& error);
        MetalMesh* GetMeshByName(const Utf8String& name);

        void ValidateMaterialForCreation(const Utf8String& name, Error& error);
        MetalMaterial* GetMaterialByName(const Utf8String& name);

        bool ValidateShaderForCreation(MetalShaderType shaderType, const Utf8String& name, Error& error);
        MetalShader* GetShaderByName(MetalShaderType shaderType, const Utf8String& name);

        MetalInputFormat* GetInputFormatByTypeName(const Utf8String& name);

    public:
        DynamicUnorderedMap<size_t, MetalInputFormat, MapPairConstructNone<size_t, MetalInputFormat>, PassthroughHash> inputFormatsByNameHash;
        DynamicUnorderedMap<size_t, MetalTexture, MapPairConstructSecond<size_t, MetalTexture>, PassthroughHash> texturesByNameHash;
        struct TextureOffset
        {
            TextureOffset()
            {
                Reset();
            }

            void Reset()
            {
                this->baseOffset = 0;
                this->count = 0;
                this->maxCount = 0;
            }

            void SetOffsetAndCount(size_t offset, size_t maxCount)
            {
                this->baseOffset = offset;
                this->count = 0;
                this->maxCount = maxCount;
            }

            size_t GetCurrentOffset() const
            {
                return this->baseOffset + this->count;
            }

            size_t baseOffset;
            size_t count;
            size_t maxCount;
        };
        EnumArray<TextureDimension, TextureDimension::COUNT, TextureOffset> textureOffsetsByDimension;
        using ShadersByNameHash = DynamicUnorderedMap<size_t, MetalShader, MapPairConstructSecond<size_t, MetalShader>, PassthroughHash>;
        EnumArray<MetalShaderType, MetalShaderType::COUNT, ShadersByNameHash> shadersByShaderTypeAndNameHash;
        DynamicUnorderedMap<size_t, MetalMaterial, MapPairConstructSecond<size_t, MetalMaterial>, PassthroughHash> materialsByNameHash;
        DynamicUnorderedMap<size_t, MetalMesh, MapPairConstructSecond<size_t, MetalMesh>, PassthroughHash> meshesByNameHash;
    };

    class MetalTextureResources
    {
    public:
        MetalTextureResources()
        {
            this->defaultSamperDescriptor = nullptr;
            this->defaultSampler = nullptr;
        }

    public:
        MTLSamplerDescriptor* defaultSamperDescriptor;
        id<MTLSamplerState> defaultSampler;
    };

} }
