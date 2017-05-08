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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/MemoryArchitecture.hpp"
#include "finjin/common/MemoryArena.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/AssetReference.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MemorySettings
    {
    public:
        struct MemoryConfigurationItem
        {
            MemoryConfigurationItem() {}
            MemoryConfigurationItem(Utf8StringView _name, Utf8StringView _value) : name(_name), value(_value) {}

            Utf8StringView name;
            Utf8StringView value;
        };

        struct MemoryConfigurationForSize
        {
            //Size
            uint64_t size;

            //Configuration
            StaticVector<MemoryConfigurationItem, 16> items;
            StaticVector<NamedMemoryArenaSettings, 16> arenas;

            bool operator < (const MemoryConfigurationForSize& other) const
            {
                return this->size < other.size;
            }
        };

        MemorySettings(Allocator* allocator = nullptr);

        const MemoryConfigurationItem* GetItem(MemoryArchitecture memoryArchitecture, MemoryLocation memoryLocation, uint64_t actualMemorySize, const Utf8String& name) const;
        const MemoryArenaSettings* GetArena(MemoryArchitecture memoryArchitecture, MemoryLocation memoryLocation, uint64_t actualMemorySize, const Utf8String& name) const;

        void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

    private:
        Allocator* allocator;

        ByteBuffer configBuffer;

        struct MemoryConfigurationForSizes : StaticVector<MemoryConfigurationForSize, 16> //Usually correspond to 256MiB, 512MiB, 1GiB, 2GiB, 3GiB, 4GiB, 6GiB, 8GiB
        {
            MemoryConfigurationForSize* GetOrAddForSize(uint64_t size)
            {
                for (auto& s : *this)
                {
                    if (s.size == size)
                        return &s;
                }

                if (!push_back())
                    return nullptr;

                auto result = &back();
                result->size = size;
                return result;
            }
        };
        MemoryConfigurationForSizes configs[(size_t)MemoryArchitecture::COUNT][(size_t)MemoryLocation::COUNT];
    };

} }
