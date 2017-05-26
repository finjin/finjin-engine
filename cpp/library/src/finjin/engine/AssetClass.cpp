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
#include "AssetClass.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
const char* AssetClassUtilities::ToString(size_t index, bool directoryName)
{
    return ToString(static_cast<AssetClass>(index), directoryName);
}

const char* AssetClassUtilities::ToString(AssetClass value, bool directoryName)
{
    switch (value)
    {
        case AssetClass::FONT: return directoryName ? "fonts" : "font";
        case AssetClass::INPUT_BINDINGS: return directoryName ? "inputbindings" : "inputbindings"; //Yes, both forms are the same
        case AssetClass::INPUT_DEVICE: return directoryName ? "inputdevices" : "inputdevice";
        case AssetClass::MATERIAL: return directoryName ? "materials" : "material";
        case AssetClass::MESH: return directoryName ? "meshes" : "mesh";
        case AssetClass::MORPH_ANIMATION: return directoryName ? "morphanims" : "morphanim";
        case AssetClass::NODE_ANIMATION: return directoryName ? "nodeanims" : "nodeanim";
        case AssetClass::POSE_ANIMATION: return directoryName ? "poseanims" : "poseanim";
        case AssetClass::PREFAB: return directoryName ? "prefabs" : "prefab";
        case AssetClass::SCENE: return directoryName ? "scenes" : "scene";
        case AssetClass::SETTINGS: return directoryName ? "settings" : "settings"; //Yes, both forms are the same
        case AssetClass::SHADER: return directoryName ? "shaders" : "shader";
        case AssetClass::SKELETON: return directoryName ? "skeletons" : "skeleton";
        case AssetClass::SKELETON_ANIMATION: return directoryName ? "skeletonanims" : "skeletonanim";
        case AssetClass::SOUND: return directoryName ? "sounds" : "sound";
        case AssetClass::STRING_TABLE: return directoryName ? "stringtables" : "stringtable";
        case AssetClass::TEXTURE: return directoryName ? "textures" : "texture";
        case AssetClass::USER_DATA_TYPES: return directoryName ? "userdatatypes" : "userdatatypes"; //Yes, both forms are the same
        default: return FINJIN_ENUM_UNKNOWN_STRING;
    }
}

AssetClass AssetClassUtilities::ParseFromExtension(const Utf8String& extension)
{
    for (size_t assetClass = 0; assetClass < (size_t)AssetClass::COUNT; assetClass++)
    {
        if (extension.EndsWith(ToString(assetClass)))
            return static_cast<AssetClass>(assetClass);
    }

    return AssetClass::COUNT;
}

bool AssetClassUtilities::IsHandwrittenExtension(const Utf8String& extension)
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

Utf8String AssetClassUtilities::ToHandwrittenString(AssetClass assetClass)
{
    Utf8String result;
    result = "hand";
    result += ToString(assetClass);
    return result;
}

Utf8String AssetClassUtilities::ToDirectoryName(AssetClass value)
{
    Utf8String result;
    ToDirectoryName(result, value);
    return result;
}

void AssetClassUtilities::ToDirectoryName(Utf8String& result, AssetClass value)
{
    result = ToString(value, true);
}

void AssetClassUtilities::FixObjectName(Path& fileName)
{
    fileName.RemoveAllChars(" :[]*#?\"'<>|");
    fileName.ReplaceAllChars("\\!", '/');
    fileName.RemoveLeadingSeparators();
}
