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
#include "finjin/common/Math.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "MetalLight.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalRenderableEntity
    {
    public:
        MetalRenderableEntity()
        {
            this->entity = nullptr;
            this->subentityIndex = 0;
            this->gpuBufferIndex = 0;
        }

    public:
        MathMatrix4 worldMatrix;
        MathMatrix4 worldInverseMatrix;
        MathMatrix4 worldInverseTransposeMatrix;
        MathVector4 ambientLightColor;
        std::array<uint32_t, EngineConstants::MAX_LIGHTS_PER_OBJECT> lightIndex;
        StaticVector<MetalLight*, EngineConstants::MAX_LIGHTS_PER_OBJECT> lights;
        FinjinSceneObjectEntity* entity;
        size_t subentityIndex;
        size_t gpuBufferIndex; //Index into constant buffer corresponding to this renderable
    };

} }
