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

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
Utf8String AssetClassUtilities::ToString(size_t index, bool directoryName)
{
    Utf8String result;
    ToString(result, (AssetClass)index, directoryName);
    return result;
}

Utf8String AssetClassUtilities::ToString(AssetClass value, bool directoryName)
{
    Utf8String result;
    ToString(result, value, directoryName);
    return result;
}

void AssetClassUtilities::ToString(Utf8String& result, size_t index, bool directoryName)
{
    ToString(result, (AssetClass)index, directoryName);
}

void AssetClassUtilities::ToString(Utf8String& result, AssetClass value, bool directoryName)
{
    switch (value)
    {
        case AssetClass::FONT: result = directoryName ? "fonts" : "font"; break;
        case AssetClass::INPUT_BINDINGS: result = directoryName ? "inputbindings" : "inputbindings"; break; //Yes, both forms are the same
        case AssetClass::INPUT_DEVICE: result = directoryName ? "inputdevices" : "inputdevice"; break;
        case AssetClass::MATERIAL: result = directoryName ? "materials" : "material"; break;
        case AssetClass::MESH: result = directoryName ? "meshes" : "mesh"; break;
        case AssetClass::MORPH_ANIMATION: result = directoryName ? "morphanims" : "morphanim"; break;
        case AssetClass::NODE_ANIMATION: result = directoryName ? "nodeanims" : "nodeanim"; break;
        case AssetClass::POSE_ANIMATION: result = directoryName ? "poseanims" : "poseanim"; break;
        case AssetClass::PREFAB: result = directoryName ? "prefabs" : "prefab"; break;
        case AssetClass::SCENE: result = directoryName ? "scenes" : "scene"; break;
        case AssetClass::SETTINGS: result = directoryName ? "settings" : "settings"; break; //Yes, both forms are the same
        case AssetClass::SHADER: result = directoryName ? "shaders" : "shader"; break;
        case AssetClass::SKELETON: result = directoryName ? "skeletons" : "skeleton"; break;
        case AssetClass::SKELETON_ANIMATION: result = directoryName ? "skeletonanims" : "skeletonanim"; break;
        case AssetClass::SOUND: result = directoryName ? "sounds" : "sound"; break;
        case AssetClass::STRING_TABLE: result = directoryName ? "stringtables" : "stringtable"; break;
        case AssetClass::TEXTURE: result = directoryName ? "textures" : "texture"; break;
        case AssetClass::USER_DATA_TYPES: result = directoryName ? "userdatatypes" : "userdatatypes"; break; //Yes, both forms are the same
        default: result.clear(); break;
    }
}

AssetClass AssetClassUtilities::ParseFromExtension(const Utf8String& extension)
{
    for (size_t i = 0; i < (size_t)AssetClass::COUNT; i++)
    {
        if (extension.EndsWith(ToString(i)))
            return (AssetClass)i;
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
    ToString(result, value, true);
}

void AssetClassUtilities::FixObjectName(Path& fileName)
{
    fileName.RemoveAllChars(" :[]*#?\"'<>|");
    fileName.ReplaceAllChars("\\!", '/');
    fileName.RemoveLeadingSeparators();
}
