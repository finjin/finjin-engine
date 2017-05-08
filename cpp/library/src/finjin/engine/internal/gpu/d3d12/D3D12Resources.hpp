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
#include "D3D12DescriptorHeap.hpp"
#include "D3D12InputFormat.hpp"
#include "D3D12Light.hpp"
#include "D3D12Material.hpp"
#include "D3D12Mesh.hpp"
#include "D3D12Shader.hpp"
#include "D3D12Texture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12AssetResources
    {
    public:
        D3D12AssetResources();

        void Destroy();

        void ValidateTextureForCreation(const Utf8String& name, Error& error);
        D3D12Texture* GetTextureByName(const Utf8String& name);

        void ValidateMeshForCreation(const Utf8String& name, Error& error);
        D3D12Mesh* GetMeshByName(const Utf8String& name);

        void ValidateMaterialForCreation(const Utf8String& name, Error& error);
        D3D12Material* GetMaterialByName(const Utf8String& name);

        bool ValidateShaderForCreation(D3D12ShaderType shaderType, const Utf8String& name, Error& error);
        D3D12Shader* GetShaderByName(D3D12ShaderType shaderType, const Utf8String& name);

        D3D12InputFormat* GetInputFormatByTypeName(const Utf8String& name);

    public:
        DynamicUnorderedMap<size_t, D3D12InputFormat, MapPairConstructNone<size_t, D3D12InputFormat>, PassthroughHash> inputFormatsByNameHash;
        DynamicUnorderedMap<size_t, D3D12Texture, MapPairConstructSecond<size_t, D3D12Texture>, PassthroughHash> texturesByNameHash;
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
        using ShadersByNameHash = DynamicUnorderedMap<size_t, D3D12Shader, MapPairConstructSecond<size_t, D3D12Shader>, PassthroughHash>;
        EnumArray<D3D12ShaderType, D3D12ShaderType::COUNT, ShadersByNameHash> shadersByShaderTypeAndNameHash;
        DynamicUnorderedMap<size_t, D3D12Material, MapPairConstructSecond<size_t, D3D12Material>, PassthroughHash> materialsByNameHash;
        DynamicUnorderedMap<size_t, D3D12Mesh, MapPairConstructSecond<size_t, D3D12Mesh>, PassthroughHash> meshesByNameHash;

        D3D12Texture* waitingToBeResidentTexturesHead;
        D3D12Mesh* waitingToBeResidentMeshesHead;
    };

    struct D3D12TextureResources
    {
        D3D12DescriptorHeap srvTextureDescHeap;
    };

} }
