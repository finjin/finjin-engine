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

#include "VulkanDescriptorSetLayout.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/StaticVector.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanDescriptorSetLayoutDescription::VulkanDescriptorSetLayoutDescription(Allocator* allocator) : typeName(allocator)
{
}

void VulkanDescriptorSetLayoutDescription::CreateDescriptorSetLayout
    (
    VulkanDescriptorSetLayout& setLayoutResult,
    VulkanDeviceFunctions& deviceVk,
    VkAllocationCallbacks* allocationCallbacks,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    VulkanDescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.bindingCount = static_cast<uint32_t>(this->vkbindings.size());
    layoutInfo.pBindings = this->vkbindings.data();

    auto result = deviceVk.CreateDescriptorSetLayout(deviceVk.device, &layoutInfo, allocationCallbacks, &setLayoutResult.vkdescriptorSetLayout);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create descriptor set layout '%1%'. Vulkan result: %2%", this->typeName, VulkanResult::ToString(result)));
        return;
    }

    setLayoutResult.description = this;
}

void VulkanDescriptorSetLayoutDescription::Create
    (
    DynamicVector<VulkanDescriptorSetLayoutDescription>& descriptorSetLayoutDescriptors,
    Allocator* allocator,
    const ByteBuffer& readBuffer,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    ConfigDocumentReader reader;
    reader.Start(readBuffer);

    Create(descriptorSetLayoutDescriptors, allocator, reader, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void VulkanDescriptorSetLayoutDescription::Create
    (
    DynamicVector<VulkanDescriptorSetLayoutDescription>& descriptorSetLayoutDescriptors,
    Allocator* allocator,
    ConfigDocumentReader& reader,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    static Utf8String configSectionName("descriptor-set-layout");

    if (reader.Current() == nullptr)
    {
        FINJIN_SET_ERROR(error, "The specified reader ended unexpectedly.");
        return;
    }

    auto startLine = *reader.Current();

    auto descriptorSetLayoutDescriptorCount = reader.GetSectionCount(configSectionName);
    if (descriptorSetLayoutDescriptorCount > 0)
    {
        if (!descriptorSetLayoutDescriptors.Create(descriptorSetLayoutDescriptorCount, allocator, allocator))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate '%1%' descriptor set layouts.", descriptorSetLayoutDescriptorCount));
            return;
        }

        descriptorSetLayoutDescriptorCount = 0;

        for (auto line = reader.Current(); line != nullptr; line = reader.Next())
        {
            switch (line->GetType())
            {
                case ConfigDocumentLine::Type::SECTION:
                {
                    Utf8StringView sectionName;
                    line->GetSectionName(sectionName);

                    if (sectionName == configSectionName)
                    {
                        auto& descriptorSetLayoutDescriptor = descriptorSetLayoutDescriptors[descriptorSetLayoutDescriptorCount++];

                        CreateFromScope(descriptorSetLayoutDescriptor, allocator, reader, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to read buffer descriptor set layout.");
                            return;
                        }
                    }

                    break;
                }
                default: break;
            }
        }
    }

    reader.Restart(startLine);
}

void VulkanDescriptorSetLayoutDescription::CreateFromScope
    (
    VulkanDescriptorSetLayoutDescription& descriptorSetLayoutDescriptor,
    Allocator* allocator,
    ConfigDocumentReader& reader,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    auto bindingCount = reader.GetSectionCountBeneathCurrent(Utf8String::GetEmpty());
    if (bindingCount > 0)
    {
        if (!descriptorSetLayoutDescriptor.bindings.Create(bindingCount, allocator, allocator))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to allocate '%1%' bindings metadata.", bindingCount));
            return;
        }

        if (!descriptorSetLayoutDescriptor.vkbindings.Create(bindingCount, allocator))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to allocate '%1%' bindings.", bindingCount));
            return;
        }
        FINJIN_ZERO_MEMORY(descriptorSetLayoutDescriptor.vkbindings.data(), descriptorSetLayoutDescriptor.vkbindings.size() * sizeof(VkDescriptorSetLayoutBinding));

        for (size_t bindingIndex = 0; bindingIndex < descriptorSetLayoutDescriptor.vkbindings.size(); bindingIndex++)
        {
            descriptorSetLayoutDescriptor.bindings[bindingIndex].vkbinding = &descriptorSetLayoutDescriptor.vkbindings[bindingIndex];

            descriptorSetLayoutDescriptor.vkbindings[bindingIndex].binding = static_cast<uint32_t>(bindingIndex);
            descriptorSetLayoutDescriptor.vkbindings[bindingIndex].descriptorCount = 1;
        }

        bindingCount = 0;

        int depth = 0;
        auto descriptionDone = false;

        for (auto line = reader.Next(); line != nullptr && !descriptionDone; )
        {
            switch (line->GetType())
            {
                case ConfigDocumentLine::Type::SECTION:
                {
                    Utf8StringView sectionName;
                    line->GetSectionName(sectionName);

                    if (sectionName == "binding")
                        bindingCount++;
                    else
                        reader.SkipScope();

                    break;
                }
                case ConfigDocumentLine::Type::KEY_AND_VALUE:
                {
                    Utf8StringView key, value;
                    line->GetKeyAndValue(key, value);

                    if (depth == 1)
                    {
                        if (key == "type")
                        {
                            if (descriptorSetLayoutDescriptor.typeName.assign(value.begin(), value.size()).HasError())
                            {
                                FINJIN_SET_ERROR(error, "Failed to assign descriptor set layout type.");
                                return;
                            }
                        }
                    }
                    else if (depth == 2)
                    {
                        auto& element = descriptorSetLayoutDescriptor.bindings[bindingCount - 1];
                        auto& binding = descriptorSetLayoutDescriptor.vkbindings[bindingCount - 1];

                        if (key == "id")
                            element.id = value;
                        else if (key == "binding")
                            binding.binding = Convert::ToInteger(value, binding.binding);
                        else if (key == "type")
                        {
                            if (value == "sampler")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                            else if (value == "combined-image-sampler")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                            else if (value == "sampled-image")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                            else if (value == "storage-image")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                            else if (value == "uniform-texel-buffer")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
                            else if (value == "storage-texel-buffer")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
                            else if (value == "uniform-buffer")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            else if (value == "storage-buffer")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                            else if (value == "uniform-buffer-dynamic")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                            else if (value == "storage-buffer-dynamic")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
                            else if (value == "input-attachment")
                                binding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                        }
                        else if (key == "descriptor-count")
                            binding.descriptorCount = Convert::ToInteger(value, binding.descriptorCount);
                        else if (key == "stages")
                        {
                            binding.stageFlags = VulkanUtilities::ParseShaderStages(value, error);
                            if (error)
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse binding stage flags '%1%'.", value));
                                 return;
                            }
                        }
                    }

                    break;
                }
                case ConfigDocumentLine::Type::SCOPE_START:
                {
                    depth++;
                    break;
                }
                case ConfigDocumentLine::Type::SCOPE_END:
                {
                    depth--;
                    if (depth == 0)
                        descriptionDone = true;
                    break;
                }
                default: break;
            }

            if (!descriptionDone)
                line = reader.Next();
        }
    }
}

#endif
