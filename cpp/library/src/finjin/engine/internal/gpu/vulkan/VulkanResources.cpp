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


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN

#include "VulkanResources.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//VulkanAssetResources
VulkanAssetResources::VulkanAssetResources()
{
    this->waitingToBeResidentTexturesHead = nullptr;
    this->waitingToBeResidentMeshesHead = nullptr;
}

void VulkanAssetResources::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
{
    this->inputFormatsByNameHash.Destroy();

    for (auto& texture : this->texturesByNameHash)
        texture.second.Destroy(vk, allocationCallbacks);
    this->texturesByNameHash.Destroy();

    for (auto& count : this->textureOffsetsByDimension)
        count.Reset();

    for (auto& shadersByType : this->shadersByShaderTypeAndNameHash)
    {
        for (auto& shader : shadersByType)
            shader.second.Destroy(vk, allocationCallbacks);
        shadersByType.Destroy();
    }

    this->materialsByNameHash.Destroy();

    for (auto& mesh : this->meshesByNameHash)
        mesh.second.Destroy(vk, allocationCallbacks);
    this->meshesByNameHash.Destroy();

    this->waitingToBeResidentTexturesHead = nullptr;
    this->waitingToBeResidentMeshesHead = nullptr;
}

//VulkanCommonResources
VulkanCommonResources::VulkanCommonResources(Allocator* allocator)
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

void VulkanCommonResources::Destroy(VulkanDeviceFunctions& vk, VkAllocationCallbacks* allocationCallbacks)
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

#endif
