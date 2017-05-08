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
#include "StringTable.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
StringTable::StringTable(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
{
}

size_t StringTable::GetCount() const
{
    if (!this->instances.empty())
        return this->instances[0].items.size();
    else
        return 0;
}

const Utf8StringView& StringTable::Get(size_t index) const
{
    if (!this->instances.empty() && index < this->instances[0].items.size())
        return this->instances[0].items[index].second;

    return Utf8StringView::Empty();
}

const Utf8StringView& StringTable::Get(const char* key) const
{
    for (size_t i = this->instances.size() - 1; i != (size_t)-1; i--)
    {
        auto& instance = this->instances[i];

        auto foundAt = std::lower_bound(instance.items.begin(), instance.items.end(), key);
        if (foundAt != instance.items.end() && foundAt->first == key)
            return foundAt->second;
    }

    return Utf8StringView::Empty();
}

const char* StringTable::GetOrDefault(const char* key, const char* defaultValue) const
{
    auto& found = Get(key);
    if (!found.empty())
        return found.begin();
    else if (defaultValue != nullptr)
        return defaultValue;
    else
        return key;
}

void StringTable::Parse(const ByteBufferReader& configFileBuffer, const AssetReference& assetRef, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    ParseSettings(configFileBuffer, assetRef.filePath, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);
}

void StringTable::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (configFileBuffer.empty())
        return;

    if (!this->instances.push_back())
    {
        FINJIN_SET_ERROR(error, "The maximum number of instances have been reached for this string asset.");
        return;
    }

    auto& instance = this->instances.back();

    //Copy the loaded settings buffer
    if (!instance.buffer.Create(configFileBuffer.max_size(), GetAllocator()))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate string table buffer of %1% bytes.", configFileBuffer.max_size()));
        return;
    }
    FINJIN_COPY_MEMORY(instance.buffer.data(), configFileBuffer.data_start(), configFileBuffer.max_size());

    //Count the number of newlines, which will be used to allocate the appropriate number of items
    size_t newlineCount = 0;
    for (size_t i = 0; i < instance.buffer.size(); i++)
    {
        if (instance.buffer[i] == '\n')
            newlineCount++;
    }

    //Initialize items, allocating an extra in case the non-empty line didn't end with a newline
    if (!instance.items.Create(newlineCount + 1, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate string table items.");
        return;
    }

    size_t startOffset = 0;

    //Parse lines
    newlineCount = 0;
    for (size_t i = 0; i <= instance.buffer.size(); i++)
    {
        if (i == instance.buffer.size() || instance.buffer[i] == '\n')
        {
            auto lineLength = i - startOffset;
            if (lineLength > 0)
            {
                size_t equalIndex = (size_t)-1;
                for (size_t rowOffset = 0; rowOffset < lineLength; rowOffset++)
                {
                    if (instance.buffer[startOffset + rowOffset] == '=')
                    {
                        equalIndex = startOffset + rowOffset;
                        break;
                    }
                }

                auto& item = instance.items[newlineCount];

                if (equalIndex != (size_t)-1)
                {
                    //Key
                    instance.buffer[equalIndex] = 0; //Null terminate key
                    item.first.assign(&instance.buffer[startOffset], &instance.buffer[equalIndex]);

                    //Value
                    instance.buffer[lineLength - 1] = 0; //Null terminate value
                    item.second.assign(&instance.buffer[equalIndex + 1], &instance.buffer[startOffset] + lineLength - 1);
                }
                else
                {
                    //Key only
                    instance.buffer[lineLength - 1] = 0; //Null terminate key
                    item.first.assign(&instance.buffer[startOffset], &instance.buffer[startOffset] + lineLength - 1);
                }

                newlineCount++;
            }

            i = startOffset = i + 1;
        }
    }

    //Remove last item if it was not needed
    if (instance.items.back().first.empty())
        instance.items.pop_back();

    //Sort items for faster lookup
    std::sort(instance.items.begin(), instance.items.end());
}
