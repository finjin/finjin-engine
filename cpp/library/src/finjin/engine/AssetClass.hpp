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
#include "finjin/common/Path.hpp"
#include "finjin/common/StreamingFileFormat.hpp"
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class AssetClass
    {
        FONT, //Not used at the moment
        INPUT_BINDINGS, //Prewritten input bindings configuration files
        INPUT_DEVICE, //Prewritten device configuration files
        MATERIAL, //Exported by finjin-exporter
        MESH, //Exported by finjin-exporter
        MORPH_ANIMATION, //Exported by finjin-exporter
        NODE_ANIMATION, //Exported by finjin-exporter
        POSE_ANIMATION, //Exported by finjin-exporter
        PREFAB, //Exported by finjin-exporter
        SCENE, //Exported by finjin-exporter
        SETTINGS, //Various generic configuration files
        SHADER, //Generated by generate_shaders.py or some other means
        SKELETON, //Exported by finjin-exporter
        SKELETON_ANIMATION, //Exported by finjin-exporter
        SOUND,
        STRING_TABLE,
        TEXTURE, //Exported by finjin-exporter
        USER_DATA_TYPES,

        COUNT
    };

    struct AssetClassUtilities
    {
        static const char* ToString(size_t index, bool directoryName = false);
        static const char* ToString(AssetClass value, bool directoryName = false);

        template <typename T>
        static AssetClass Parse(const T& value)
        {
            for (size_t assetClass = 0; assetClass < (size_t)AssetClass::COUNT; assetClass++)
            {
                if (value == ToString(assetClass, false) || value == ToString(assetClass, true))
                    return static_cast<AssetClass>(assetClass);
            }

            return AssetClass::COUNT;
        }

        template <typename T>
        static bool IsDirectoryName(const T& path, AssetClass assetClass)
        {
            return path.EndsWith(ToDirectoryName(assetClass));
        }

        template <typename T>
        static AssetClass ParseFromExtension(const T& extension)
        {
            for (size_t assetClass = 0; assetClass < (size_t)AssetClass::COUNT; assetClass++)
            {
                if (extension.EndsWith(ToString(assetClass)))
                    return static_cast<AssetClass>(assetClass);
            }
            
            return AssetClass::COUNT;
        }

        template <typename T>
        static bool IsHandwrittenExtension(const T& extension)
        {
            auto hyphenFoundAt = extension.find('-');
            if (hyphenFoundAt != Utf8String::npos)
            {
                Utf8StringView sub;
                extension.substr(sub, hyphenFoundAt + 1, 4);
                return sub == "hand";
            }
            else
                return extension.StartsWith("hand");
        }

        static Utf8String ToHandwrittenString(AssetClass assetClass);

        static Utf8String ToDirectoryName(AssetClass value);
        static void ToDirectoryName(Utf8String& result, AssetClass value);

        static void FixObjectName(Path& fileName);
    };

} }
