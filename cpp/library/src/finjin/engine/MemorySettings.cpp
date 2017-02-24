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
#include "finjin/engine/MemorySettings.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/MemorySize.hpp"
#include "finjin/common/Setting.hpp"

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
MemorySettings::MemorySettings(Allocator* allocator)
{
    this->allocator = allocator;
}

const MemorySettings::MemoryConfigurationItem* MemorySettings::GetItem(MemoryArchitecture memoryArch, MemoryLocation memoryLocation, uint64_t actualMemorySize, const Utf8String& name) const
{
    auto& configSizes = this->configs[(size_t)memoryArch][(size_t)memoryLocation];
    for (size_t i = configSizes.size() - 1; i != (size_t)-1; i--)
    {
        auto& configSize = configSizes[i];

        if (actualMemorySize >= configSize.size)
        {
            for (auto& item : configSize.items)
            {
                if (item.name == name)
                    return &item;
            }
        }
    }

    return nullptr;
}

const MemoryArenaSettings* MemorySettings::GetArena(MemoryArchitecture memoryArch, MemoryLocation memoryLocation, uint64_t actualMemorySize, const Utf8String& name) const
{
    auto& configSizes = this->configs[(size_t)memoryArch][(size_t)memoryLocation];
    for (size_t i = configSizes.size() - 1; i != (size_t)-1; i--)
    {
        auto& configSize = configSizes[i];

        if (actualMemorySize >= configSize.size)
        {
            for (auto& arena : configSize.arenas)
            {
                if (arena.name == name)
                    return &arena;
            }
        }
    }

    return nullptr;
}

void MemorySettings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (!this->configBuffer.Create(configFileBuffer.max_size(), this->allocator))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate buffer of %1% bytes.", configFileBuffer.max_size()));
        return;
    }
    FINJIN_COPY_MEMORY(this->configBuffer.data(), configFileBuffer.data_start(), configFileBuffer.max_size());

    Setting<MemoryArchitecture> memoryArchitectureSetting(MemoryArchitecture::SEGMENTED); //Must be set
    Setting<MemoryLocation> memoryLocationSetting(MemoryLocation::SYSTEM_DEDICATED); //Optional
    Setting<uint64_t> memorySizeSetting; //Must be set
    
    NamedMemoryArenaSettings namedMemoryArena;
    
    Utf8StringView section, key, value;
    ConfigDocumentReader reader;
    
    for (auto line = reader.Start(configFileBuffer); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                line->GetSectionName(section);

                if (section == "unified")
                {
                    memoryArchitectureSetting = MemoryArchitecture::UNIFIED;
                }
                else if (section == "segmented")
                {
                    memoryArchitectureSetting = MemoryArchitecture::SEGMENTED;
                }
                else if (section == "system-dedicated")
                {
                    memoryLocationSetting = MemoryLocation::SYSTEM_DEDICATED;
                }
                else if (section == "gpu-dedicated")
                {
                    memoryLocationSetting = MemoryLocation::GPU_DEDICATED;
                }
                else if (section == "arena")
                {
                    //Do nothing
                }
                else
                {
                    uint64_t memorySize;
                    MemorySize::Parse(memorySize, section.ToString(), error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse memory size '%1%'.", section));
                        return;
                    }
                    
                    memorySizeSetting = memorySize;
                }
                
                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                line->GetKeyAndValue(key, value);

                if (memoryArchitectureSetting.IsSet() && memorySizeSetting.IsSet())
                {
                    auto& config = this->configs[(size_t)memoryArchitectureSetting.value][(size_t)memoryLocationSetting.value];

                    auto configSize = config.GetOrAddForSize(memorySizeSetting.value);
                    if (configSize == nullptr)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add lookup entry for memory size '%1%'.", memorySizeSetting.value));
                        return;
                    }
                    
                    if (section == "arena")
                    {
                        //Note: Alignment is intentionally not part of the file format

                        if (key == "name")
                            namedMemoryArena.name = value.ToString();
                        else if (key == "size")
                        {
                            MemorySize::Parse(namedMemoryArena.byteCount, value.ToString(), error);
                            if (error)
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid value '%1%' for setting 'size'.", value));
                                return;
                            }

                            if (configSize->arenas.push_back(namedMemoryArena).HasErrorOrValue(false))
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add arena '%1%' for memory size '%2%'.", namedMemoryArena.name, namedMemoryArena.byteCount));
                                return;
                            }
                        }
                    }
                    else
                    {
                        if (configSize->items.push_back(MemoryConfigurationItem(key, value)).HasErrorOrValue(false))
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add item '%1%' for memory size '%2%'.", key.ToString(), memorySizeSetting.value));
                            return;
                        }
                    }
                }
                
                break;
            }
            default: break;
        }
    }

    for (size_t memoryArch = 0; memoryArch < (size_t)MemoryArchitecture::COUNT; memoryArch++)
    {
        for (size_t memoryLocation = 0; memoryLocation < (size_t)MemoryLocation::COUNT; memoryLocation++)
        {
            auto& config = this->configs[memoryArch][memoryLocation];
            std::sort(config.begin(), config.end());
        }
    }
}
