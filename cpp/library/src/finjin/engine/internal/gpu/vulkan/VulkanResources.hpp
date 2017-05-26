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

    struct VulkanCommonResources
    {
        VulkanCommonResources(Allocator* allocator)
        {
            //Vulkan clip space has inverted Y and half Z
            this->clipInvertMatrix <<
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, -1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.5f, 0.0f,
                0.0f, 0.0f, 0.5f, 1.0f
                ;

            this->pipelineCache = VK_NULL_HANDLE;

            this->defaultSampler = VK_NULL_HANDLE;

            for (auto& shader : this->fullScreenShaders)
                shader.name.Create(allocator);

            this->texturesDescriptorSetLayout = VK_NULL_HANDLE;
            this->textureDescriptorPool = VK_NULL_HANDLE;
            this->textureDescriptorSet = VK_NULL_HANDLE;

            this->fullScreenQuadDescriptorSetLayout = VK_NULL_HANDLE;
            this->fullScreenQuadDescriptorPool = VK_NULL_HANDLE;
            this->fullScreenQuadDescriptorSet = VK_NULL_HANDLE;
            this->fullScreenQuadPipelineLayout = VK_NULL_HANDLE;
            this->fullScreenQuadPipeline = VK_NULL_HANDLE;
        }

        void Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
        {
            if (this->fullScreenQuadDescriptorPool != VK_NULL_HANDLE)
            {
                vk.DestroyDescriptorPool(vk.device, this->fullScreenQuadDescriptorPool, allocationCallbacks);
                this->fullScreenQuadDescriptorPool = VK_NULL_HANDLE;
                this->fullScreenQuadDescriptorSet = VK_NULL_HANDLE; //Freed with pool
            }

            if (this->fullScreenQuadPipeline != VK_NULL_HANDLE)
            {
                vk.DestroyPipeline(vk.device, this->fullScreenQuadPipeline, allocationCallbacks);
                this->fullScreenQuadPipeline = VK_NULL_HANDLE;
            }

            if (this->fullScreenQuadPipelineLayout != VK_NULL_HANDLE)
            {
                vk.DestroyPipelineLayout(vk.device, this->fullScreenQuadPipelineLayout, allocationCallbacks);
                this->fullScreenQuadPipelineLayout = VK_NULL_HANDLE;
            }

            if (this->texturesDescriptorSetLayout != VK_NULL_HANDLE)
            {
                vk.DestroyDescriptorSetLayout(vk.device, this->texturesDescriptorSetLayout, allocationCallbacks);
                this->texturesDescriptorSetLayout = VK_NULL_HANDLE;
            }

            if (this->textureDescriptorPool != VK_NULL_HANDLE)
            {
                vk.DestroyDescriptorPool(vk.device, this->textureDescriptorPool, allocationCallbacks);
                this->textureDescriptorPool = VK_NULL_HANDLE;
                this->textureDescriptorSet = VK_NULL_HANDLE; //Freed with pool
            }

            if (this->fullScreenQuadDescriptorSetLayout != VK_NULL_HANDLE)
            {
                vk.DestroyDescriptorSetLayout(vk.device, this->fullScreenQuadDescriptorSetLayout, allocationCallbacks);
                this->fullScreenQuadDescriptorSetLayout = VK_NULL_HANDLE;
            }

            for (auto& shader : this->fullScreenShaders)
                shader.Destroy(vk, allocationCallbacks);

            if (this->defaultSampler != VK_NULL_HANDLE)
            {
                vk.DestroySampler(vk.device, this->defaultSampler, allocationCallbacks);
                this->defaultSampler = VK_NULL_HANDLE;
            }

            if (this->pipelineCache != VK_NULL_HANDLE)
            {
                vk.DestroyPipelineCache(vk.device, this->pipelineCache, allocationCallbacks);
                this->pipelineCache = VK_NULL_HANDLE;
            }
        }

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
