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
#include "CameraState.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
CameraState::CameraState() : orthoSize{ 0, 0 }, range{ 0, 0 }
{
    this->worldMatrix.setIdentity();
    this->inverseWorldMatrix.setIdentity();
    this->inverseTransposeWorldMatrix.setIdentity();

    this->projectionType = CameraProjectionType::PERSPECTIVE;
}

MathVector4& CameraState::GetWorldPosition(MathVector4& position4) const
{
    static MathVector4 localPosition(0, 0, 0, 1);
    position4 = this->worldMatrix * localPosition;
    return position4;
}

MathVector3& CameraState::GetWorldPosition(MathVector3& position3, MathVector4& position4) const
{
    static MathVector4 localPosition(0, 0, 0, 1);
    position4 = this->worldMatrix * localPosition;
    position3 = MathVector3(position4(0), position4(1), position4(2));
    return position3;
}

MathVector3& CameraState::GetWorldDirection(MathVector3& direction3, MathVector4& direction4) const
{
    static MathVector4 localDirection(0, 1, 0, 0);
    direction4 = this->inverseTransposeWorldMatrix * localDirection;
    direction3 = MathVector3(direction4(0), direction4(1), direction4(2));
    direction3.normalize();
    return direction3;
}
