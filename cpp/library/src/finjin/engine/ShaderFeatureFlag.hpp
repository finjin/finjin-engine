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
#include "finjin/common/EnumBitwise.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class ShaderFeatureFlag : uint32_t
    {
        NONE = 0,

        //Map (texture)
        MAP_DIFFUSE = 1 << 0,
        MAP_SPECULAR = 1 << 1,
        MAP_REFLECTION = 1 << 2,
        MAP_REFRACTION = 1 << 3,
        MAP_BUMP = 1 << 4,
        MAP_HEIGHT  = 1 << 5,
        MAP_SELF_ILLUMINATION = 1 << 6,
        MAP_OPACITY = 1 << 7,
        MAP_ENVIRONMENT = 1 << 8,
        MAP_SHININESS = 1 << 9,

        //Mesh
        MESH_SKELETON_ANIMATION = 1 << 10,
        MESH_POSE_ANIMATION = 1 << 11,
        MESH_MORPH_ANIMATION = 1 << 12,

        //Environment
        ENVIRONMENT_LINEAR_DISTANCE_FOG = 1 << 13,
        ENVIRONMENT_EXP_DISTANCE_FOG = 1 << 14,

        //Rendering
        RENDERING_FILL_WIREFRAME = 1 << 15, //If not specified, solid fill is assumed
        RENDERING_VELOCITY_BLUR = 1 << 16,

        ALL = 0xffffffff
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(ShaderFeatureFlag)

    struct RenderShaderFeatureFlags
    {
        RenderShaderFeatureFlags();

        ShaderFeatureFlag ForMaterialMapsAndMesh(ShaderFeatureFlag materialMapFlags, ShaderFeatureFlag meshFlags) const;

        static const RenderShaderFeatureFlags& GetDefault();

        ShaderFeatureFlag environmentFlags;
        ShaderFeatureFlag renderingFlags;
        ShaderFeatureFlag shaderFeatureFlagsMask; //Masks all flags except for 'additionalShaderFeatureFlags'
        ShaderFeatureFlag additionalShaderFeatureFlags; //Applied last
    };

} }
