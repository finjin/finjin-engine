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
#include "finjin/engine/ShaderFeatures.hpp"
#include "finjin/common/Hash.hpp"
#include "finjin/common/StaticUnorderedMap.hpp"

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
ShaderFeatures::ShaderFeatures()
{
    this->inputFormatHash = 0;
    this->flags = ShaderFeatureFlag::NONE;
}

bool ShaderFeatures::ParseAndAddFlag(const Utf8StringView& feature)
{
    const size_t count = 40;
    static StaticUnorderedMap<size_t, ShaderFeatureFlag, count, FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT(count), MapPairConstructNone<size_t, ShaderFeatureFlag>, PassthroughHash> lookup
        (
        Utf8String::Hash("map-di"), ShaderFeatureFlag::MAP_DIFFUSE,
        Utf8String::Hash("map-diffuse"), ShaderFeatureFlag::MAP_DIFFUSE,
        Utf8String::Hash("map-sp"), ShaderFeatureFlag::MAP_SPECULAR,
        Utf8String::Hash("map-specular"), ShaderFeatureFlag::MAP_SPECULAR,
        Utf8String::Hash("map-rl"), ShaderFeatureFlag::MAP_REFLECTION,
        Utf8String::Hash("map-reflection"), ShaderFeatureFlag::MAP_REFLECTION,
        Utf8String::Hash("map-rr"), ShaderFeatureFlag::MAP_REFRACTION,
        Utf8String::Hash("map-refraction"), ShaderFeatureFlag::MAP_REFRACTION,
        Utf8String::Hash("map-bu"), ShaderFeatureFlag::MAP_BUMP,
        Utf8String::Hash("map-bump"), ShaderFeatureFlag::MAP_BUMP,
        Utf8String::Hash("map-he"), ShaderFeatureFlag::MAP_HEIGHT,
        Utf8String::Hash("map-height"), ShaderFeatureFlag::MAP_HEIGHT,
        Utf8String::Hash("map-si"), ShaderFeatureFlag::MAP_SELF_ILLUMINATION,
        Utf8String::Hash("map-self-illumination"), ShaderFeatureFlag::MAP_SELF_ILLUMINATION,
        Utf8String::Hash("map-op"), ShaderFeatureFlag::MAP_OPACITY,
        Utf8String::Hash("map-opacity"), ShaderFeatureFlag::MAP_OPACITY,
        Utf8String::Hash("map-en"), ShaderFeatureFlag::MAP_ENVIRONMENT,
        Utf8String::Hash("map-environment"), ShaderFeatureFlag::MAP_ENVIRONMENT,    
        Utf8String::Hash("map-sh"), ShaderFeatureFlag::MAP_SHININESS,
        Utf8String::Hash("map-shininess"), ShaderFeatureFlag::MAP_SHININESS,    

        Utf8String::Hash("mesh-sa"), ShaderFeatureFlag::MESH_SKELETON_ANIMATION,
        Utf8String::Hash("mesh-skeleton-animation"), ShaderFeatureFlag::MESH_SKELETON_ANIMATION,
        Utf8String::Hash("mesh-pa"), ShaderFeatureFlag::MESH_POSE_ANIMATION,
        Utf8String::Hash("mesh-pose-animation"), ShaderFeatureFlag::MESH_POSE_ANIMATION,
        Utf8String::Hash("mesh-ma"), ShaderFeatureFlag::MESH_MORPH_ANIMATION,
        Utf8String::Hash("mesh-morph-animation"), ShaderFeatureFlag::MESH_MORPH_ANIMATION,

        Utf8String::Hash("environment-ldf"), ShaderFeatureFlag::ENVIRONMENT_LINEAR_DISTANCE_FOG,
        Utf8String::Hash("environment-linear-distance-fog"), ShaderFeatureFlag::ENVIRONMENT_LINEAR_DISTANCE_FOG,
        Utf8String::Hash("environment-edf"), ShaderFeatureFlag::ENVIRONMENT_EXP_DISTANCE_FOG,
        Utf8String::Hash("environment-exp-distance-fog"), ShaderFeatureFlag::ENVIRONMENT_EXP_DISTANCE_FOG,

        Utf8String::Hash("rendering-w"), ShaderFeatureFlag::RENDERING_FILL_WIREFRAME,
        Utf8String::Hash("rendering-wireframe"), ShaderFeatureFlag::RENDERING_FILL_WIREFRAME,
        Utf8String::Hash("rendering-vb"), ShaderFeatureFlag::RENDERING_VELOCITY_BLUR,
        Utf8String::Hash("rendering-velocity-blur"), ShaderFeatureFlag::RENDERING_VELOCITY_BLUR
        );

    Utf8StringHash hash;
    auto foundAt = lookup.find(hash(feature));
    if (foundAt != lookup.end())
    {
        this->flags |= foundAt->second;
        return true;
    }
    else
        return false;
}

bool ShaderFeatures::ParseAndAddLightType(const Utf8StringView& lightType)
{
    const size_t count = 6;
    static StaticUnorderedMap<size_t, LightType, count, FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT(count), MapPairConstructNone<size_t, LightType>, PassthroughHash> lookup
        (
        Utf8String::Hash("d"), LightType::DIRECTIONAL,
        Utf8String::Hash("directional"), LightType::DIRECTIONAL,

        Utf8String::Hash("p"), LightType::POINT,
        Utf8String::Hash("point"), LightType::POINT,

        Utf8String::Hash("s"), LightType::SPOT,
        Utf8String::Hash("spot"), LightType::SPOT
        );

    Utf8StringHash hash;
    auto foundAt = lookup.find(hash(lightType));
    if (foundAt != lookup.end())
        return this->lights.push_back(foundAt->second).HasValue(true);
    else
        return false;
}

size_t ShaderFeatures::GetHash() const
{
    size_t values[] = { this->inputFormatHash, (size_t)this->flags, Hash::Bytes(this->lights.data(), sizeof(LightType) * this->lights.size()), this->lights.size() };
    return Hash::Bytes(values, sizeof(values));
}
