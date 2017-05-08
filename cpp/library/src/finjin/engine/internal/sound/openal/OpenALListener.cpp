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
#include "OpenALListener.hpp"
#include "OpenALIncludes.hpp"
#include "OpenALContext.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
OpenALListener::OpenALListener()
{
    this->context = nullptr;
    SetDefaults();
}

void OpenALListener::Create(OpenALContext* context)
{
    this->context = context;
    SetDefaults();
}

void OpenALListener::SetDefaults()
{
    Set
        (
        MathVector3::Zero(),
        MathVector3::Zero(),
        FINJIN_OPENAL_DEFAULT_FORWARD,
        FINJIN_OPENAL_DEFAULT_UP
        );
}

void OpenALListener::Set(const MathVector3& value)
{
    Set
        (
        value,
        MathVector3::Zero(),
        FINJIN_OPENAL_DEFAULT_FORWARD,
        FINJIN_OPENAL_DEFAULT_UP
        );
}

void OpenALListener::Set
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

void OpenALListener::GetOrientation(MathVector3& direction, MathVector3& up) const
{
    direction = this->orientation[0];
    up = this->orientation[1];
}

void OpenALListener::SetOrientation(const MathVector3& direction, const MathVector3& up)
{
    this->orientation[0] = direction;
    this->orientation[1] = up;
    this->invalidated = true;
}

void OpenALListener::GetPosition(MathVector3& value) const
{
    value = this->position;
}

void OpenALListener::SetPosition(const MathVector3& value)
{
    this->position = value;
    this->invalidated = true;
}

void OpenALListener::GetVelocity(MathVector3& value) const
{
    value = this->velocity;
}

void OpenALListener::SetVelocity(const MathVector3& value)
{
    this->velocity = value;
    this->invalidated = true;
}

void OpenALListener::Commit(bool force)
{
    if (force || this->invalidated)
    {
        alListenerfv(AL_POSITION, this->position.data());

        alListenerfv(AL_VELOCITY, this->velocity.data());

        float orientation[6];
        memcpy(&orientation[0], this->orientation[0].data(), sizeof(float) * 3);
        memcpy(&orientation[3], this->orientation[1].data(), sizeof(float) * 3);
        alListenerfv(AL_ORIENTATION, orientation);

        this->invalidated = false;
    }
}
