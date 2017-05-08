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
#include "OpenSLESListener.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
OpenSLESListener::OpenSLESListener()
{
    this->context = nullptr;
    SetDefaults();
}

void OpenSLESListener::Create(OpenSLESContext* context)
{
    this->context = context;
    SetDefaults();
}

void OpenSLESListener::SetDefaults()
{
    Set
        (
        MathVector3::Zero(),
        MathVector3::Zero(),
        MathVector3(0, 0, -1),
        MathVector3(0, 1, 0)
        );
}

void OpenSLESListener::MakeDirectional()
{
}

void OpenSLESListener::MakeOmnidirectional()
{
}

void OpenSLESListener::Set
    (
    const MathVector3& position,
    const MathVector3& velocity,
    const MathVector3& direction,
    const MathVector3& up
    )
{
    SetPosition(position);
    SetVelocity(velocity);
    SetOrientation(direction, up);
}

void OpenSLESListener::GetOrientation(MathVector3& direction, MathVector3& up) const
{
    direction = this->forward;
    up = this->up;
}

void OpenSLESListener::SetOrientation(const MathVector3& direction, const MathVector3& up)
{
    this->forward = direction;
    this->up = up;
    this->invalidated = true;
}

void OpenSLESListener::GetPosition(MathVector3& value) const
{
    value = this->position;
}

void OpenSLESListener::SetPosition(const MathVector3& value)
{
    this->position = value;
    this->invalidated = true;
}

void OpenSLESListener::GetVelocity(MathVector3& value) const
{
    value = this->velocity;
}

void OpenSLESListener::SetVelocity(const MathVector3& value)
{
    this->velocity = value;
    //this->invalidated = true; //Velocity isn't used for anything at the moment, so don't flag as invalidated
}

void OpenSLESListener::GetConeAngle(Angle& inner, Angle& outer) const
{
    inner = Angle(0);
    outer = Angle(0);
}

void OpenSLESListener::SetConeAngle(Angle inner, Angle outer)
{
}

void OpenSLESListener::GetConeVolume(float& inner, float& outer) const
{
    inner = 1;
    outer = 1;
}

void OpenSLESListener::SetConeVolume(float inner, float outer)
{
}

void OpenSLESListener::Commit(bool force)
{
    if (force || this->invalidated)
    {
        MathVector3 right = this->up.cross(this->forward);

        this->worldToListener(0, 0) = right(0);         this->worldToListener(0, 1) = right(1);         this->worldToListener(0, 2) = right(2);         this->worldToListener(0, 3) = -right.dot(this->position);
        this->worldToListener(1, 0) = this->up(0);      this->worldToListener(1, 1) = this->up(1);      this->worldToListener(1, 2) = this->up(2);      this->worldToListener(1, 3) = -this->up.dot(this->position);
        this->worldToListener(2, 0) = this->forward(2); this->worldToListener(2, 1) = this->forward(1); this->worldToListener(2, 2) = this->forward(2); this->worldToListener(2, 3) = -this->forward.dot(this->position);
        this->worldToListener(3, 0) = 0.0f;             this->worldToListener(3, 1) = 0.0f;             this->worldToListener(3, 2) = 0.0f;             this->worldToListener(3, 3) = 1.0f;

        this->invalidated = false;
    }
}
