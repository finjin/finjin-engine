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
#include "XAudio2Listener.hpp"
#include "XAudio2ListenerImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
XAudio2Listener::XAudio2Listener() : impl(new XAudio2ListenerImpl)
{
    SetDefaults();
}

XAudio2Listener::~XAudio2Listener()
{
}

void XAudio2Listener::Create(XAudio2Context* context)
{
    impl->context = context;

    SetDefaults();
}

void XAudio2Listener::SetDefaults()
{
    Set
        (
        MathVector3::Zero(),
        MathVector3::Zero(),
        MathVector3(0, 0, -1),
        MathVector3(0, 1, 0)
        );

    SetConeAngle(Radians(0), Radians(X3DAUDIO_2PI / 4));
    SetConeVolume(1, .5f);
    SetConeReverb(0, 0);
    SetConeCoefficients(0, 0);

    MakeOmnidirectional();
}

void XAudio2Listener::MakeDirectional()
{
    impl->x3dListener.pCone = &impl->x3dCone;
}

void XAudio2Listener::MakeOmnidirectional()
{
    impl->x3dListener.pCone = nullptr;
}

void XAudio2Listener::Set
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

void XAudio2Listener::GetOrientation(MathVector3& direction, MathVector3& up) const
{
    direction = MathVector3(impl->x3dListener.OrientFront.x, impl->x3dListener.OrientFront.y, impl->x3dListener.OrientFront.z);
    up = MathVector3(impl->x3dListener.OrientTop.x, impl->x3dListener.OrientTop.y, impl->x3dListener.OrientTop.z);
}

void XAudio2Listener::SetOrientation(const MathVector3& direction, const MathVector3& up)
{
    impl->x3dListener.OrientFront.x = direction(0);
    impl->x3dListener.OrientFront.y = direction(1);
    impl->x3dListener.OrientFront.z = direction(2);
    impl->x3dListener.OrientTop.x = up(0);
    impl->x3dListener.OrientTop.y = up(1);
    impl->x3dListener.OrientTop.z = up(2);
    impl->invalidated = true;
}

void XAudio2Listener::GetPosition(MathVector3& value) const
{
    value = MathVector3(impl->x3dListener.Position.x, impl->x3dListener.Position.y, impl->x3dListener.Position.z);
}

void XAudio2Listener::SetPosition(const MathVector3& value)
{
    impl->x3dListener.Position.x = value(0);
    impl->x3dListener.Position.y = value(1);
    impl->x3dListener.Position.z = value(2);
    impl->invalidated = true;
}

void XAudio2Listener::GetVelocity(MathVector3& value) const
{
    value = MathVector3(impl->x3dListener.Velocity.x, impl->x3dListener.Velocity.y, impl->x3dListener.Velocity.z);
}

void XAudio2Listener::SetVelocity(const MathVector3& value)
{
    impl->x3dListener.Velocity.x = value(0);
    impl->x3dListener.Velocity.y = value(1);
    impl->x3dListener.Velocity.z = value(2);
    impl->invalidated = true;
}

void XAudio2Listener::Commit(bool force)
{
}

void XAudio2Listener::GetConeAngle(Angle& inner, Angle& outer) const
{
    inner = Radians(impl->x3dCone.InnerAngle);
    outer = Radians(impl->x3dCone.OuterAngle);
}

void XAudio2Listener::SetConeAngle(Angle inner, Angle outer)
{
    impl->x3dCone.InnerAngle = inner.ToRadiansValue();
    impl->x3dCone.OuterAngle = outer.ToRadiansValue();
}

void XAudio2Listener::GetConeVolume(float& inner, float& outer) const
{
    inner = impl->x3dCone.InnerVolume / 2.0f;
    outer = impl->x3dCone.OuterVolume / 2.0f;
}

void XAudio2Listener::SetConeVolume(float inner, float outer)
{
    impl->x3dCone.InnerVolume = inner * 2.0f;
    impl->x3dCone.OuterVolume = outer * 2.0f;
}

void XAudio2Listener::GetConeReverb(float& inner, float& outer) const
{
    inner = impl->x3dCone.InnerReverb / 2.0f;
    outer = impl->x3dCone.OuterReverb / 2.0f;
}

void XAudio2Listener::SetConeReverb(float inner, float outer)
{
    impl->x3dCone.InnerReverb = inner * 2.0f;
    impl->x3dCone.OuterReverb = outer * 2.0f;
}

void XAudio2Listener::GetConeCoefficients(float& inner, float& outer) const
{
    inner = impl->x3dCone.InnerLPF;
    outer = impl->x3dCone.OuterLPF;
}

void XAudio2Listener::SetConeCoefficients(float inner, float outer)
{
    impl->x3dCone.InnerLPF = inner;
    impl->x3dCone.OuterLPF = outer;
}

XAudio2ListenerImpl* XAudio2Listener::GetImpl()
{
    return impl.get();
}
