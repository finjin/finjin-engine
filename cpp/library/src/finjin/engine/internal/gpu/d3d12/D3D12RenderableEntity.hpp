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


//Includes---------------------------------------------------------------
#include "finjin/common/Math.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"


//Classes----------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    using namespace Finjin::Common;
    
    struct D3D12RenderableEntity
    {
        MathMatrix44 worldMatrix;
        MathMatrix44 worldInverseMatrix;
        MathMatrix44 worldInverseTransposeMatrix;
        MathVector4 ambientLightColor;
        std::array<uint32_t, EngineConstants::MAX_LIGHTS_PER_OBJECT> lightIndex;
        FinjinSceneObjectEntity* entity;
        size_t subentityIndex;
        uint32_t gpuBufferIndex; //Index into constant buffer or structured buffer corresponding to this renderable
    };
    
} }
