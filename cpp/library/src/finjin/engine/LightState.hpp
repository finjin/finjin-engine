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


//Includes---------------------------------------------------------------------
#include "finjin/common/Angle.hpp"
#include "finjin/common/Math.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;    

    enum class LightType
    {
        DIRECTIONAL,
        POINT,
        SPOT
    };

    struct LightState
    {
        LightState() :
            color(0, 0, 0, 1),            
            range{ 0, 10000 }
        {
            this->worldMatrix.setIdentity();
            this->inverseWorldMatrix.setIdentity();
            this->inverseTransposeWorldMatrix.setIdentity();
            this->isActive = false;
            this->lightType = LightType::DIRECTIONAL;
            this->power = 1;
        }
        
        int GetLightTypePriority() const { return (int)this->lightType; }
        static int GetLightTypePriority(LightType lightType) { return (int)lightType; }

        MathMatrix44 worldMatrix;
        MathMatrix44 inverseWorldMatrix; 
        MathMatrix44 inverseTransposeWorldMatrix;
        bool isActive;
        LightType lightType;
        float power;
        MathVector4 color;
        float range[2]; //Near/far range                        
        Radians coneRange[2]; //Inner/outer angles
    };

} }
