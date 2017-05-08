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
#include "finjin/common/Angle.hpp"
#include "finjin/common/Math.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class CameraProjectionType
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    struct CameraState
    {
        CameraState();

        MathVector4& GetWorldPosition(MathVector4& position4) const;
        MathVector3& GetWorldPosition(MathVector3& position3, MathVector4& position4) const;

        MathVector3& GetWorldDirection(MathVector3& direction3, MathVector4& direction4) const;

        MathMatrix4 worldMatrix;
        MathMatrix4 inverseWorldMatrix;
        MathMatrix4 inverseTransposeWorldMatrix;

        Radians fovY;

        CameraProjectionType projectionType;

        float orthoSize[2];

        float range[2];
    };

} }
