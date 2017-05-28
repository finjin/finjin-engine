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
#include "VulkanInputFormat.hpp"
#include "VulkanLight.hpp"
#include "VulkanMaterial.hpp"
#include "VulkanMesh.hpp"
#include "VulkanShader.hpp"
#include "VulkanTexture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanAssetResources
    {
    public:
        VulkanAssetResources();

        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

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
        VulkanTexture* GetTextureByName(const T& name)
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
        VulkanMesh* GetMeshByName(const T& name)
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
        VulkanMaterial* GetMaterialByName(const T& name)
        {
            Utf8StringHash hash;
            auto foundAt = this->materialsByNameHash.find(hash(name));
            if (foundAt != this->materialsByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

        template <typename T>
        bool ValidateShaderForCreation(VulkanShaderType shaderType, const T& name, Error& error)
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
        VulkanShader* GetShaderByName(VulkanShaderType shaderType, const T& name)
        {
            auto& shadersByNameHash = this->shadersByShaderTypeAndNameHash[shaderType];

            Utf8StringHash hash;
            auto foundAt = shadersByNameHash.find(hash(name));
            if (foundAt != shadersByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

        template <typename T>
        VulkanInputFormat* GetInputFormatByTypeName(const T& name)
        {
            Utf8StringHash hash;
            auto foundAt = this->inputFormatsByNameHash.find(hash(name));
            if (foundAt != this->inputFormatsByNameHash.end())
                return &foundAt->second;
            return nullptr;
        }

    public:
        DynamicUnorderedMap<size_t, VulkanInputFormat, MapPairConstructNone<size_t, VulkanInputFormat>, PassthroughHash> inputFormatsByNameHash;
        DynamicUnorderedMap<size_t, VulkanTexture, MapPairConstructSecond<size_t, VulkanTexture>, PassthroughHash> texturesByNameHash;
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
        using ShadersByNameHash = DynamicUnorderedMap<size_t, VulkanShader, MapPairConstructSecond<size_t, VulkanShader>, PassthroughHash>;
        EnumArray<VulkanShaderType, VulkanShaderType::COUNT, ShadersByNameHash> shadersByShaderTypeAndNameHash;
        DynamicUnorderedMap<size_t, VulkanMaterial, MapPairConstructSecond<size_t, VulkanMaterial>, PassthroughHash> materialsByNameHash;
        DynamicUnorderedMap<size_t, VulkanMesh, MapPairConstructSecond<size_t, VulkanMesh>, PassthroughHash> meshesByNameHash;

        VulkanTexture* waitingToBeResidentTexturesHead;
        VulkanMesh* waitingToBeResidentMeshesHead;
    };

    struct VulkanCommonResources
    {
        VulkanCommonResources(Allocator* allocator);

        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks);

        MathMatrix4 clipInvertMatrix;

        UsableDynamicVector<VulkanLight> lights;

        VkPipelineCache pipelineCache;

        VkSampler defaultSampler;

        std::array<VulkanShader, 2> fullScreenShaders; //0 = vertex shader, 1 = pixel shader

        VkDescriptorSetLayout texturesDescriptorSetLayout;
        VkDescriptorPool textureDescriptorPool;
        VkDescriptorSet textureDescriptorSet;

        VkDescriptorSetLayout fullScreenQuadDescriptorSetLayout;
        VkDescriptorPool fullScreenQuadDescriptorPool;
        VkDescriptorSet fullScreenQuadDescriptorSet;
        VkPipelineLayout fullScreenQuadPipelineLayout;
        VkPipeline fullScreenQuadPipeline;
    };

} }
