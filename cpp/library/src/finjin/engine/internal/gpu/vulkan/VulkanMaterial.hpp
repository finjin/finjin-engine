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
#include "VulkanIncludes.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/ShaderFeatures.hpp"
#include "VulkanTexture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class VulkanMaterial
    {
    public:
        VulkanMaterial(Allocator* allocator);

        enum class MapIndex
        {
            DIFFUSE,
            SPECULAR,
            REFLECTION,
            REFRACTION,
            BUMP,
            HEIGHT,
            SELF_ILLUMINATION,
            OPACITY,
            ENVIRONMENT,
            SHININESS,

            COUNT
        };

        struct Map
        {
            Map();

            FinjinMaterial::Map* finjinMap;
            VulkanTexture* texture;
        };

    public:
        FinjinMaterial* finjinMaterial;

        size_t gpuBufferIndex; //Index into constant buffer or structured buffer corresponding to this material

        EnumArray<MapIndex, MapIndex::COUNT, Map> maps;

        bool isFullyResolved; //Indicates whether all resource references have been resolved
    };

} }
