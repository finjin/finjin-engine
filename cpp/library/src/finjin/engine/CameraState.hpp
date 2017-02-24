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

    struct CameraState
    {
        enum class ProjectionType
        {
            PERSPECTIVE,
            ORTHOGRAPHIC
        };

        CameraState() : orthoSize{ 0, 0 }, range{ 0, 0 }
        {
            this->worldMatrix.setIdentity();
            this->inverseWorldMatrix.setIdentity();

            this->projectionType = ProjectionType::PERSPECTIVE;
        }

        MathMatrix44 worldMatrix;
        MathMatrix44 inverseWorldMatrix;

        Radians fovY;

        ProjectionType projectionType;

        float orthoSize[2];

        float range[2];
    };

} }
