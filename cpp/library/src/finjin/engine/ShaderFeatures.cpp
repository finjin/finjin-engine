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


//Implementation----------------------------------------------------------------
ShaderFeatures::ShaderFeatures()
{
    this->inputFormatHash = 0;
    this->flags = ShaderFeatureFlag::NONE;
}

void ShaderFeatures::ParseAndAddFlags(const Utf8StringView& features, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto splitResult = Split(features, ' ', [this](Utf8StringView& feature)
    {
        if (!ParseAndAddFlag(feature))
            return ValueOrError<bool>::CreateError();

        return ValueOrError<bool>(true);
    });
    if (splitResult.HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to parse feature flags.");
        return;
    }
    else if (!splitResult.value)
    {
        FINJIN_SET_ERROR(error, "Failed to parse and add all feature flags.");
        return;
    }
}

bool ShaderFeatures::ParseAndAddFlag(const Utf8StringView& feature)
{
    static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(ShaderFeatureFlag, 40) lookup
        (
        "map-di", ShaderFeatureFlag::MAP_DIFFUSE,
        "map-diffuse", ShaderFeatureFlag::MAP_DIFFUSE,
        "map-sp", ShaderFeatureFlag::MAP_SPECULAR,
        "map-specular", ShaderFeatureFlag::MAP_SPECULAR,
        "map-rl", ShaderFeatureFlag::MAP_REFLECTION,
        "map-reflection", ShaderFeatureFlag::MAP_REFLECTION,
        "map-rr", ShaderFeatureFlag::MAP_REFRACTION,
        "map-refraction", ShaderFeatureFlag::MAP_REFRACTION,
        "map-bu", ShaderFeatureFlag::MAP_BUMP,
        "map-bump", ShaderFeatureFlag::MAP_BUMP,
        "map-he", ShaderFeatureFlag::MAP_HEIGHT,
        "map-height", ShaderFeatureFlag::MAP_HEIGHT,
        "map-si", ShaderFeatureFlag::MAP_SELF_ILLUMINATION,
        "map-self-illumination", ShaderFeatureFlag::MAP_SELF_ILLUMINATION,
        "map-op", ShaderFeatureFlag::MAP_OPACITY,
        "map-opacity", ShaderFeatureFlag::MAP_OPACITY,
        "map-en", ShaderFeatureFlag::MAP_ENVIRONMENT,
        "map-environment", ShaderFeatureFlag::MAP_ENVIRONMENT,
        "map-sh", ShaderFeatureFlag::MAP_SHININESS,
        "map-shininess", ShaderFeatureFlag::MAP_SHININESS,

        "mesh-sa", ShaderFeatureFlag::MESH_SKELETON_ANIMATION,
        "mesh-skeleton-animation", ShaderFeatureFlag::MESH_SKELETON_ANIMATION,
        "mesh-pa", ShaderFeatureFlag::MESH_POSE_ANIMATION,
        "mesh-pose-animation", ShaderFeatureFlag::MESH_POSE_ANIMATION,
        "mesh-ma", ShaderFeatureFlag::MESH_MORPH_ANIMATION,
        "mesh-morph-animation", ShaderFeatureFlag::MESH_MORPH_ANIMATION,

        "environment-ldf", ShaderFeatureFlag::ENVIRONMENT_LINEAR_DISTANCE_FOG,
        "environment-linear-distance-fog", ShaderFeatureFlag::ENVIRONMENT_LINEAR_DISTANCE_FOG,
        "environment-edf", ShaderFeatureFlag::ENVIRONMENT_EXP_DISTANCE_FOG,
        "environment-exp-distance-fog", ShaderFeatureFlag::ENVIRONMENT_EXP_DISTANCE_FOG,

        "rendering-w", ShaderFeatureFlag::RENDERING_FILL_WIREFRAME,
        "rendering-wireframe", ShaderFeatureFlag::RENDERING_FILL_WIREFRAME,
        "rendering-vb", ShaderFeatureFlag::RENDERING_VELOCITY_BLUR,
        "rendering-velocity-blur", ShaderFeatureFlag::RENDERING_VELOCITY_BLUR
        );

    auto foundAt = lookup.find(feature);
    if (foundAt != lookup.end())
    {
        this->flags |= foundAt->second;
        return true;
    }
    else
        return false;
}

void ShaderFeatures::ParseAndAddLightTypes(const Utf8StringView& lightTypes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto splitResult = Split(lightTypes, ' ', [this](Utf8StringView& lightType)
    {
        if (this->lights.full())
            return ValueOrError<bool>(false);

        ParseAndAddLightType(lightType);

        return ValueOrError<bool>(true);
    });
    if (splitResult.HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to parse light types.");
        return;
    }
    else if (!splitResult.value)
    {
        FINJIN_SET_ERROR(error, "Failed to parse and add all light types.");
        return;
    }
}

bool ShaderFeatures::ParseAndAddLightType(const Utf8StringView& lightType)
{
    static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(LightType, 6) lookup
        (
        "d", LightType::DIRECTIONAL,
        "directional", LightType::DIRECTIONAL,

        "p", LightType::POINT,
        "point", LightType::POINT,

        "s", LightType::SPOT,
        "spot", LightType::SPOT
        );

    auto foundAt = lookup.find(lightType);
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
