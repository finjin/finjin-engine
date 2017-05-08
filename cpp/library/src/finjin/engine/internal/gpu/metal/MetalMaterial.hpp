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
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "MetalTexture.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalMaterial
    {
    public:
        MetalMaterial(Allocator* allocator);

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
            MetalTexture* texture;
        };

    public:
        FinjinMaterial* finjinMaterial;

        size_t gpuBufferIndex; //Index into uniform buffer corresponding to this material

        EnumArray<MapIndex, MapIndex::COUNT, Map> maps;

        bool isFullyResolved; //Indicates whether all resource references have been resolved
    };

} }
