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
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/EnumArray.hpp"
#include "finjin/common/Setting.hpp"
#include "finjin/engine/AssetReference.hpp"
#include "finjin/engine/ShaderFeatures.hpp"
#include "VulkanIncludes.hpp"
#include "VulkanShaderType.hpp"
#include "VulkanDescriptorSetLayout.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct VulkanFoundPipelineLayoutBinding
    {
        VulkanFoundPipelineLayoutBinding()
        {
            this->descriptorSetLayoutIndex = (size_t)-1;
            this->indexInDescriptorSetLayout = (size_t)-1;
            this->vkbinding = nullptr;
        }

        bool IsValid() const
        {
            return this->vkbinding != nullptr;
        }

        size_t descriptorSetLayoutIndex; //Descriptor set index in the pipeline layout
        size_t indexInDescriptorSetLayout; //Index of binding in descriptor set binding collection
        VkDescriptorSetLayoutBinding* vkbinding; //The binding. The shader binding value is at binding->binding
    };

    class VulkanPipelineLayout
    {
    public:
        enum { MAX_DESCRIPTOR_SET_LAYOUTS = 4 };
        enum { MAX_PUSH_CONSTANT_RANGES = 8 };

        VulkanPipelineLayout()
        {
            this->vkpipelineLayout = VK_NULL_HANDLE;
        }

        template <typename T>
        VulkanFoundPipelineLayoutBinding FindBindingByID(const T& id) const
        {
            VulkanFoundPipelineLayoutBinding result;

            for (size_t descriptorSetLayoutIndex = 0; descriptorSetLayoutIndex < this->descriptorSetLayouts.size(); descriptorSetLayoutIndex++)
            {
                auto descriptorLayoutDescription = this->descriptorSetLayouts[descriptorSetLayoutIndex]->description;

                auto foundBindingIndex = descriptorLayoutDescription->FindBindingIndexByID(id);
                if (foundBindingIndex != (size_t)-1)
                {
                    result.descriptorSetLayoutIndex = descriptorSetLayoutIndex;
                    result.indexInDescriptorSetLayout = foundBindingIndex;
                    result.vkbinding = &descriptorLayoutDescription->vkbindings[foundBindingIndex];
                    break;
                }
            }

            return result;
        }

        template <size_t MaxItems>
        void GetDescriptorSetPoolSizes(StaticVector<VkDescriptorPoolSize, MaxItems>& poolSizes, Error& error) const
        {
            FINJIN_ERROR_METHOD_START(error);

            for (auto descriptorSetLayout : this->descriptorSetLayouts)
            {
                for (auto& binding : descriptorSetLayout->description->bindings)
                {
                    if (!poolSizes.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to add pool size. Too many entries for destination collection.");
                        return;
                    }

                    auto& poolSize = poolSizes.back();
                    poolSize.type = binding.vkbinding->descriptorType;
                    poolSize.descriptorCount = static_cast<uint32_t>(binding.vkbinding->descriptorCount);
                }
            }
        }

        template <size_t MaxItems>
        void GetVkDescriptorSetBindings(StaticVector<VkDescriptorSetLayoutBinding, MaxItems>& bindings, Error& error) const
        {
            FINJIN_ERROR_METHOD_START(error);

            for (auto descriptorSetLayout : this->descriptorSetLayouts)
            {
                for (auto& binding : descriptorSetLayout->description->bindings)
                {
                    if (bindings.push_back(*binding.vkbinding).HasErrorOrValue(false))
                    {
                        FINJIN_SET_ERROR(error, "Failed to add binding. Too many entries for destination collection.");
                        return;
                    }
                }
            }
        }

        template <size_t MaxItems>
        void GetDescriptorSetBindings(StaticVector<VulkanDescriptorSetLayoutDescription::Binding*, MaxItems>& bindings, Error& error) const
        {
            FINJIN_ERROR_METHOD_START(error);

            for (auto descriptorSetLayout : this->descriptorSetLayouts)
            {
                for (auto& binding : descriptorSetLayout->description->bindings)
                {
                    if (bindings.push_back(&binding).HasErrorOrValue(false))
                    {
                        FINJIN_SET_ERROR(error, "Failed to add binding. Too many entries for destination collection.");
                        return;
                    }
                }
            }
        }

        StaticVector<VkDescriptorSetLayout, MAX_DESCRIPTOR_SET_LAYOUTS> GetVkDescriptorSetLayouts()
        {
            StaticVector<VkDescriptorSetLayout, MAX_DESCRIPTOR_SET_LAYOUTS> result;
            for (auto descriptorSetLayout : this->descriptorSetLayouts)
                result.push_back(descriptorSetLayout->vkdescriptorSetLayout);
            return result;
        }

        VkPipelineLayout vkpipelineLayout;

        StaticVector<VulkanDescriptorSetLayout*, MAX_DESCRIPTOR_SET_LAYOUTS> descriptorSetLayouts;
    };

    class VulkanPipelineLayoutDescription
    {
    public:
        VulkanPipelineLayoutDescription(Allocator* allocator);

        void CreatePipelineLayout
            (
            VulkanPipelineLayout& result,
            VulkanDeviceFunctions& deviceVk,
            VkAllocationCallbacks* allocationCallbacks,
            DynamicVector<VulkanDescriptorSetLayout>& allDescriptorSetLayouts,
            Error& error
            );

        template <typename Collection, typename Name>
        static VulkanPipelineLayoutDescription* GetByTypeName(Collection& collection, const Name& name)
        {
            for (auto& item : collection)
            {
                if (item.typeName == name)
                    return &item;
            }
            return nullptr;
        }

        static void Create
            (
            DynamicVector<VulkanPipelineLayoutDescription>& pipelineLayouts,
            Allocator* allocator,
            const ByteBuffer& readBuffer,
            Error& error
            );

        static void Create
            (
            DynamicVector<VulkanPipelineLayoutDescription>& pipelineLayouts,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            Error& error
            );

        static void CreateFromScope
            (
            VulkanPipelineLayoutDescription& pipelineLayout,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            Error& error
            );

    public:
        Utf8String typeName;

        StaticVector<Utf8String, VulkanPipelineLayout::MAX_DESCRIPTOR_SET_LAYOUTS> descriptorSetLayoutTypeNames;

        struct PushConstantRange
        {
            Utf8String id;
        };
        StaticVector<PushConstantRange, VulkanPipelineLayout::MAX_PUSH_CONSTANT_RANGES> pushConstantRanges;
        StaticVector<VkPushConstantRange, VulkanPipelineLayout::MAX_PUSH_CONSTANT_RANGES> vkpushConstantRanges;
    };

} }
