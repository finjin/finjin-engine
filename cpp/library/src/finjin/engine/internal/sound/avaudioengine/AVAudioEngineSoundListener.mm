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
#include "AVAudioEngineSoundListener.hpp"
#include "AVAudioEngineSoundContextImpl.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct AVAudioEngineSoundListener::Impl
{
    Impl()
    {
        this->context = nullptr;

        FINJIN_ZERO_ITEM(this->position);
        FINJIN_ZERO_ITEM(this->orientation);
        this->invalidated = true;
    }

    AVAudioEngineSoundContextImpl* context;

    AVAudio3DVector position;
    AVAudio3DVectorOrientation orientation;
    bool invalidated;
};


//Implementation----------------------------------------------------------------
AVAudioEngineSoundListener::AVAudioEngineSoundListener() : impl(new Impl)
{
    SetDefaults();
}

AVAudioEngineSoundListener::~AVAudioEngineSoundListener()
{
    delete impl;
}

void AVAudioEngineSoundListener::Create(AVAudioEngineSoundContextImpl* context)
{
    impl->context = context;

    SetDefaults();
}

void AVAudioEngineSoundListener::SetDefaults()
{
    Set
        (
        MathVector3::Zero(),
        MathVector3::Zero(),
        MathVector3(0, 0, -1),
        MathVector3(0, 1, 0)
        );

    //SetConeAngle(Radians(0), Radians(2 * PI / 4));
    SetConeVolume(1, .5f);

    MakeOmnidirectional();
}

void AVAudioEngineSoundListener::MakeDirectional()
{
}

void AVAudioEngineSoundListener::MakeOmnidirectional()
{
}

void AVAudioEngineSoundListener::Set
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

void AVAudioEngineSoundListener::GetOrientation(MathVector3& direction, MathVector3& up) const
{
    direction = MathVector3(impl->orientation.forward.x, impl->orientation.forward.y, impl->orientation.forward.z);
    up = MathVector3(impl->orientation.up.x, impl->orientation.up.y, impl->orientation.up.z);
}

void AVAudioEngineSoundListener::SetOrientation(const MathVector3& direction, const MathVector3& up)
{
    impl->orientation.forward = AVAudioMake3DVector(direction(0), direction(1), direction(2));
    impl->orientation.up = AVAudioMake3DVector(up(0), up(1), up(2));

    impl->invalidated = true;
}

void AVAudioEngineSoundListener::GetPosition(MathVector3& value) const
{
    value = MathVector3(impl->position.x, impl->position.y, impl->position.z);
}

void AVAudioEngineSoundListener::SetPosition(const MathVector3& value)
{
    impl->position = AVAudioMake3DVector(value(0), value(1), value(2));

    impl->invalidated = true;
}

void AVAudioEngineSoundListener::GetVelocity(MathVector3& value) const
{
    value = MathVector3::Zero();
}

void AVAudioEngineSoundListener::SetVelocity(const MathVector3& value)
{
}

void AVAudioEngineSoundListener::GetConeAngle(Angle& inner, Angle& outer) const
{
    inner = Angle(0);
    outer = Angle(0);
}

void AVAudioEngineSoundListener::SetConeAngle(Angle inner, Angle outer)
{
}

void AVAudioEngineSoundListener::GetConeVolume(float& inner, float& outer) const
{
    inner = outer = 1.0f;
}

void AVAudioEngineSoundListener::SetConeVolume(float inner, float outer)
{
}

void AVAudioEngineSoundListener::Commit(bool force)
{
    if (force || impl->invalidated)
    {
        [impl->context->nsimpl->avenv setListenerPosition:impl->position];

        [impl->context->nsimpl->avenv setListenerVectorOrientation:impl->orientation];

        impl->invalidated = false;
    }
}
