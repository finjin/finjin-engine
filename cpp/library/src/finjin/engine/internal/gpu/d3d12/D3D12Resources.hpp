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
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/AllocatedUnorderedMap.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Hash.hpp"
#include "finjin/common/UsableAllocatedVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "D3D12InputFormat.hpp"
#include "D3D12Light.hpp"
#include "D3D12Material.hpp"
#include "D3D12Mesh.hpp"
#include "D3D12Shader.hpp"
#include "D3D12Texture.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    using namespace Finjin::Common;

    class D3D12Resources
    {
    public:
        D3D12Resources();

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
        AllocatedUnorderedMap<size_t, D3D12InputFormat, MapPairConstructNone<size_t, D3D12InputFormat>, PassthroughHash> inputFormatsByNameHash;
        AllocatedUnorderedMap<size_t, D3D12Texture, MapPairConstructSecond<size_t, D3D12Texture>, PassthroughHash> texturesByNameHash;
        struct TextureOffset
        {
            TextureOffset()
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
        EnumValues<D3D12Texture::Type, D3D12Texture::Type::COUNT, TextureOffset> textureOffsetsByType;
        using ShadersByNameHash = AllocatedUnorderedMap<size_t, D3D12Shader, MapPairConstructSecond<size_t, D3D12Shader>, PassthroughHash>;
        EnumValues<D3D12ShaderType, D3D12ShaderType::COUNT, ShadersByNameHash> shadersByShaderTypeAndNameHash;
        AllocatedUnorderedMap<size_t, D3D12Material, MapPairConstructSecond<size_t, D3D12Material>, PassthroughHash> materialsByNameHash;
        AllocatedUnorderedMap<size_t, D3D12Mesh, MapPairConstructSecond<size_t, D3D12Mesh>, PassthroughHash> meshesByNameHash;
        
        D3D12Texture* waitingToBeResidentTexturesHead;
        D3D12Mesh* waitingToBeResidentMeshesHead;
    };

} }
