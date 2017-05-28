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

        template <typename T>
        void ValidateTextureForCreation(const T& name, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (this->texturesByNameHash.full())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create texture '%1%'. Texture lookup is full.", name));
                return;
            }

            Utf8StringHash hash;
            if (this->texturesByNameHash.contains(hash(name)))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create texture '%1%'. The name already exists.", name));
                return;
            }
        }

        template <typename T>
        D3D12Texture* GetTextureByName(const T& name)
        {
            Utf8StringHash hash;
            auto foundAt = this->texturesByNameHash.find(hash(name));
            if (foundAt != this->texturesByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

        template <typename T>
        void ValidateMeshForCreation(const T& name, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (this->meshesByNameHash.full())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create mesh '%1%'. Mesh lookup is full.", name));
                return;
            }

            Utf8StringHash hash;
            if (this->meshesByNameHash.contains(hash(name)))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create mesh '%1%'. The name already exists.", name));
                return;
            }
        }

        template <typename T>
        D3D12Mesh* GetMeshByName(const T& name)
        {
            Utf8StringHash hash;
            auto foundAt = this->meshesByNameHash.find(hash(name));
            if (foundAt != this->meshesByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

        template <typename T>
        void ValidateMaterialForCreation(const T& name, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (this->materialsByNameHash.full())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create material '%1%'. Material lookup is full.", name));
                return;
            }

            Utf8StringHash hash;
            if (this->materialsByNameHash.contains(hash(name)))
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create material '%1%'. The name already exists.", name));
                return;
            }
        }

        template <typename T>
        D3D12Material* GetMaterialByName(const T& name)
        {
            Utf8StringHash hash;
            auto foundAt = this->materialsByNameHash.find(hash(name));
            if (foundAt != this->materialsByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

        template <typename T>
        bool ValidateShaderForCreation(D3D12ShaderType shaderType, const T& name, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& shadersByNameHash = this->shadersByShaderTypeAndNameHash[shaderType];

            if (shadersByNameHash.full())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create shader '%1%'. Shader lookup is full.", name));
                return false;
            }

            //Encountering a duplicate shader isn't an error condition
            Utf8StringHash hash;
            if (shadersByNameHash.contains(hash(name)))
                return false;

            return true;
        }

        template <typename T>
        D3D12Shader* GetShaderByName(D3D12ShaderType shaderType, const T& name)
        {
            auto& shadersByNameHash = this->shadersByShaderTypeAndNameHash[shaderType];

            Utf8StringHash hash;
            auto foundAt = shadersByNameHash.find(hash(name));
            if (foundAt != shadersByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

        template <typename T>
        D3D12InputFormat* GetInputFormatByTypeName(const T& name)
        {
            Utf8StringHash hash;
            auto foundAt = this->inputFormatsByNameHash.find(hash(name));
            if (foundAt != this->inputFormatsByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

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

    struct D3D12CommonResources
    {
        D3D12CommonResources(Allocator* allocator);

        void Destroy();

        UsableDynamicVector<D3D12Light> lights;

        Microsoft::WRL::ComPtr<ID3D12RootSignature> fullScreenShaderRootSignature;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> fullScreenShaderGraphicsPipelineState;
        std::array<ByteBuffer, 2> fullScreenShaderFileBytes; //0 = vertex shader, 1 = pixel shader

        D3D12DescriptorHeap srvTextureDescHeap;
    };

} }
