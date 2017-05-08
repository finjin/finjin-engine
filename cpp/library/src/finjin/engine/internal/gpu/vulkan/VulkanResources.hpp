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

        void ValidateTextureForCreation(const Utf8String& name, Error& error);
        VulkanTexture* GetTextureByName(const Utf8String& name);

        void ValidateMeshForCreation(const Utf8String& name, Error& error);
        VulkanMesh* GetMeshByName(const Utf8String& name);

        void ValidateMaterialForCreation(const Utf8String& name, Error& error);
        VulkanMaterial* GetMaterialByName(const Utf8String& name);

        bool ValidateShaderForCreation(VulkanShaderType shaderType, const Utf8String& name, Error& error);
        VulkanShader* GetShaderByName(VulkanShaderType shaderType, const Utf8String& name);

        VulkanInputFormat* GetInputFormatByTypeName(const Utf8String& name);

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

    struct VulkanTextureResources
    {
        VulkanTextureResources()
        {
            this->defaultSampler = VK_NULL_HANDLE;
            this->texturesDescriptorSetLayout = VK_NULL_HANDLE;
            this->textureDescriptorPool = VK_NULL_HANDLE;
            this->textureDescriptorSet = VK_NULL_HANDLE;
        }

        VkSampler defaultSampler;

        VkDescriptorSetLayout texturesDescriptorSetLayout;
        VkDescriptorPool textureDescriptorPool;
        VkDescriptorSet textureDescriptorSet;
    };

} }
