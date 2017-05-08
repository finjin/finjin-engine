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

#include "VulkanPipelineLayout.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
VulkanPipelineLayoutDescription::VulkanPipelineLayoutDescription(Allocator* allocator) :
    typeName(allocator)
{
    this->descriptorSetLayoutTypeNames.maximize();
    for (auto& item : this->descriptorSetLayoutTypeNames)
        item.Create(allocator);
    this->descriptorSetLayoutTypeNames.clear();

    this->pushConstantRanges.maximize();
    for (auto& pushConstantRange : this->pushConstantRanges)
        pushConstantRange.id.Create(allocator);
    this->pushConstantRanges.clear();

    FINJIN_ZERO_ITEM(this->vkpushConstantRanges);
}

void VulkanPipelineLayoutDescription::CreatePipelineLayout
    (
    VulkanPipelineLayout& pipelineLayoutResult,
    VulkanDeviceFunctions& deviceVk,
    VkAllocationCallbacks* allocationCallbacks,
    DynamicVector<VulkanDescriptorSetLayout>& allDescriptorSetLayouts,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //Find the descriptor set layout objects
    for (auto& descriptorSetLayoutTypeName : this->descriptorSetLayoutTypeNames)
    {
        VulkanDescriptorSetLayout* foundDescriptorSetLayout = nullptr;
        for (auto& descriptorSetLayout : allDescriptorSetLayouts)
        {
            if (descriptorSetLayout.description->typeName == descriptorSetLayoutTypeName)
            {
                foundDescriptorSetLayout = &descriptorSetLayout;
                break;
            }
        }
        if (foundDescriptorSetLayout == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find descriptor set layout '%1%'.", descriptorSetLayoutTypeName));
            return;
        }

        if (pipelineLayoutResult.descriptorSetLayouts.push_back(foundDescriptorSetLayout).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add layout '%1%'. Too many entries.", descriptorSetLayoutTypeName));
            return;
        }
    }

    auto pipelineLayoutDescriptorSetLayouts = pipelineLayoutResult.GetVkDescriptorSetLayouts();

    VulkanPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(pipelineLayoutDescriptorSetLayouts.size());
    pipelineLayoutCreateInfo.pSetLayouts = pipelineLayoutDescriptorSetLayouts.data();
    pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(this->vkpushConstantRanges.size());
    pipelineLayoutCreateInfo.pPushConstantRanges = this->vkpushConstantRanges.data();

    auto result = deviceVk.CreatePipelineLayout(deviceVk.device, &pipelineLayoutCreateInfo, allocationCallbacks, &pipelineLayoutResult.vkpipelineLayout);
    if (result != VK_SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create pipeline layout '%1%'. Vulkan result: %2%", this->typeName, VulkanResult::ToString(result)));
        return;
    }
}

void VulkanPipelineLayoutDescription::Create
    (
    DynamicVector<VulkanPipelineLayoutDescription>& pipelineLayouts,
    Allocator* allocator,
    const ByteBuffer& readBuffer,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    ConfigDocumentReader reader;
    reader.Start(readBuffer);

    Create(pipelineLayouts, allocator, reader, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void VulkanPipelineLayoutDescription::Create
    (
    DynamicVector<VulkanPipelineLayoutDescription>& pipelineLayouts,
    Allocator* allocator,
    ConfigDocumentReader& reader,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    static Utf8String configSectionName("pipeline-layout");

    if (reader.Current() == nullptr)
    {
        FINJIN_SET_ERROR(error, "The specified reader ended unexpectedly.");
        return;
    }

    auto startLine = *reader.Current();

    auto pipelineLayoutDescriptorCount = reader.GetSectionCount(configSectionName);
    if (pipelineLayoutDescriptorCount > 0)
    {
        if (!pipelineLayouts.Create(pipelineLayoutDescriptorCount, allocator, allocator))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate '%1%' pipeline layouts.", pipelineLayoutDescriptorCount));
            return;
        }

        pipelineLayoutDescriptorCount = 0;

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
                        auto& pipelineLayoutDescriptor = pipelineLayouts[pipelineLayoutDescriptorCount++];

                        CreateFromScope(pipelineLayoutDescriptor, allocator, reader, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to read buffer pipeline layout.");
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

void VulkanPipelineLayoutDescription::CreateFromScope
    (
    VulkanPipelineLayoutDescription& pipelineLayoutDescription,
    Allocator* allocator,
    ConfigDocumentReader& reader,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

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

                if (sectionName == "push-constant-range")
                {
                    if (pipelineLayoutDescription.vkpushConstantRanges.full())
                    {
                        FINJIN_SET_ERROR(error, "Too many push constants specified.");
                        return;
                    }

                    pipelineLayoutDescription.pushConstantRanges.push_back();
                    pipelineLayoutDescription.vkpushConstantRanges.push_back();
                }
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
                        if (pipelineLayoutDescription.typeName.assign(value.begin(), value.size()).HasError())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign pipeline layout type '%1%' value '%2%'.", pipelineLayoutDescription.typeName, value));
                            return;
                        }
                    }
                    else if (key == "descriptor-set-layouts")
                    {
                        auto splitResult = Split(value, ' ', [&pipelineLayoutDescription](Utf8StringView& value)
                        {
                            if (pipelineLayoutDescription.descriptorSetLayoutTypeNames.push_back(value.ToString()).HasErrorOrValue(false))
                                return ValueOrError<bool>::CreateError();

                            return ValueOrError<bool>(true);
                        });
                        if (splitResult.HasError())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse pipeline layout type '%1%' descriptor set layouts '%2%'.", pipelineLayoutDescription.typeName, value));
                            return;
                        }
                    }
                }
                else if (depth == 2)
                {
                    auto& pushConstantRange = pipelineLayoutDescription.pushConstantRanges.back();
                    auto& vkpushConstantRange = pipelineLayoutDescription.vkpushConstantRanges.back();

                    if (key == "id")
                    {
                        if (pushConstantRange.id.assign(value).HasErrorOrValue(false))
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign pipeline layout type '%1%' id '%2%'.", pipelineLayoutDescription.typeName, value));
                            return;
                        }
                    }
                    else if (key == "stages")
                    {
                        vkpushConstantRange.stageFlags = VulkanUtilities::ParseShaderStages(value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign pipeline layout type '%1%' push constant range shader stage flags '%2%'.", pipelineLayoutDescription.typeName, value));
                            return;
                        }
                    }
                    else if (key == "offset")
                    {
                        VulkanUtilities::ParsePushConstantRangeOffsetAndSize(vkpushConstantRange, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign pipeline layout type '%1%' push constant range shader offset/size '%2%'.", pipelineLayoutDescription.typeName, value));
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

#endif
