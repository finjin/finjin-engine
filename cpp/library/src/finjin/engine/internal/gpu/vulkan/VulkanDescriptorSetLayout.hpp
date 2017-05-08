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
#include "finjin/common/Hash.hpp"
#include "VulkanIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanDescriptorSetLayoutDescription;

    class VulkanDescriptorSetLayout
    {
    public:
        VulkanDescriptorSetLayout()
        {
            this->vkdescriptorSetLayout = VK_NULL_HANDLE;
            this->description = nullptr;
        }

    public:
        VkDescriptorSetLayout vkdescriptorSetLayout;
        VulkanDescriptorSetLayoutDescription* description;
    };

    class VulkanDescriptorSetLayoutDescription
    {
    public:
        VulkanDescriptorSetLayoutDescription(Allocator* allocator);

        void CreateDescriptorSetLayout
            (
            VulkanDescriptorSetLayout& result,
            VulkanDeviceFunctions& deviceVk,
            VkAllocationCallbacks* allocationCallbacks,
            Error& error
            );

        template <typename Collection, typename Name>
        static VulkanDescriptorSetLayoutDescription* GetByTypeName(Collection& collection, const Name& name)
        {
            for (auto& item : collection)
            {
                if (item.typeName == name)
                    return &item;
            }
            return nullptr;
        }

        struct Binding
        {
            Binding(Allocator* allocator) : id(allocator)
            {
                this->vkbinding = nullptr;
            }

            size_t GetHash() const
            {
                if (this->vkbinding == nullptr)
                    return this->id.GetHash();
                else
                {
                    size_t values[] = { this->id.GetHash(), (size_t)this->vkbinding->descriptorType, (size_t)this->vkbinding->descriptorCount, (size_t)this->vkbinding->stageFlags };
                    return Hash::Bytes(values, sizeof(values));
                }
            }

            template <typename Collection>
            static size_t Hash(Collection& c)
            {
                StaticVector<size_t, 50> values;
                for (auto& item : c)
                {
                    if (values.push_back(item.GetHash()).HasErrorOrValue(false))
                        break;
                }

                assert(values.size() == c.size());
                assert(!values.full());
                values.push_back(c.size());

                return Hash::Bytes(values.data(), sizeof(size_t) * values.size());
            }

            Utf8String id;
            VkDescriptorSetLayoutBinding* vkbinding;
        };

        template <typename T>
        size_t FindBindingIndexByID(const T& id) const
        {
            for (size_t index = 0; index < this->bindings.size(); index++)
            {
                if (this->bindings[index].id == id)
                    return index;
            }

            return (size_t)-1;
        }

        size_t GetHash() const
        {
            size_t values[] = { this->typeName.GetHash(), Binding::Hash(this->bindings) };
            return Hash::Bytes(values, sizeof(values));
        }

        static void Create
            (
            DynamicVector<VulkanDescriptorSetLayoutDescription>& descriptorSetLayoutDescriptors,
            Allocator* allocator,
            const ByteBuffer& readBuffer,
            Error& error
            );

        static void Create
            (
            DynamicVector<VulkanDescriptorSetLayoutDescription>& descriptorSetLayoutDescriptors,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            Error& error
            );

        static void CreateFromScope
            (
            VulkanDescriptorSetLayoutDescription& descriptorSetLayoutDescriptor,
            Allocator* allocator,
            ConfigDocumentReader& reader,
            Error& error
            );

    public:
        Utf8String typeName;
        DynamicVector<Binding> bindings;
        DynamicVector<VkDescriptorSetLayoutBinding> vkbindings;
    };

} }
