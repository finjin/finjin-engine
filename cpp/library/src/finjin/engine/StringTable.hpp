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


//Includes---------------------------------------------------------------------
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/Asset.hpp"
#include "finjin/engine/AssetReference.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class ASSET_CLASS(StringTable) : public AllocatedClass
    {
    public:
        enum { MAX_INSTANCES = 2 };

        StringTable(Allocator* allocator = nullptr);

        size_t GetCount() const;
        const Utf8StringView& Get(size_t index) const;
        const Utf8StringView& Get(const char* key) const;
        const char* GetOrDefault(const char* key, const char* defaultValue = nullptr) const;

        void Parse(const ByteBufferReader& configFileBuffer, const AssetReference& assetRef, Error& error);
        void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

    public:
        Utf8String name;

        struct Pair
        {
            Utf8StringView first, second;

            bool operator < (const Pair& other) const
            {
                const char* a = this->first.begin() ? this->first.begin() : "";
                const char* b = other.first.begin() ? other.first.begin() : "";
                return strcmp(a, b) < 0;
            }

            bool operator < (const Utf8String& other) const
            {
                const char* a = this->first.begin() ? this->first.begin() : "";
                const char* b = other.c_str();
                return strcmp(a, b) < 0;
            }

            bool operator < (const Utf8StringView& other) const
            {
                const char* a = this->first.begin() ? this->first.begin() : "";
                const char* b = other.begin() ? other.begin() : "";
                return strcmp(a, b) < 0;
            }

            bool operator < (const char* other) const
            {
                const char* a = this->first.begin() ? this->first.begin() : "";
                const char* b = other ? other : "";
                return strcmp(a, b) < 0;
            }
        };

        struct Instance
        {
            ByteBuffer buffer;
            AllocatedVector<Pair> items;
        };
        
        StaticVector<Instance, MAX_INSTANCES> instances;
    };

} }
