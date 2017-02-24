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


//Implementation---------------------------------------------------------------
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
        0, 0, 0,
        0, 0, 0,
        0, 0, -1,
        0, 1, 0
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
    float x, float y, float z,
    float vx, float vy, float vz,
    float directionx, float directiony, float directionz,
    float upx, float upy, float upz
    ) 
{
    SetPosition(x, y, z);
    SetVelocity(vx, vy, vz);
    SetOrientation(directionx, directiony, directionz, upx, upy, upz);
}

void XAudio2Listener::GetOrientation
    (
    float& directionx, float& directiony, float& directionz,
    float& upx, float& upy, float& upz
    ) const 
{
    directionx = impl->x3dListener.OrientFront.x;
    directiony = impl->x3dListener.OrientFront.y;
    directionz = impl->x3dListener.OrientFront.z;
    upx = impl->x3dListener.OrientTop.x;
    upy = impl->x3dListener.OrientTop.y;
    upz = impl->x3dListener.OrientTop.z;
}

void XAudio2Listener::SetOrientation
    (
    float directionx, float directiony, float directionz,
    float upx, float upy, float upz
    ) 
{
    impl->x3dListener.OrientFront.x = directionx;
    impl->x3dListener.OrientFront.y = directiony;
    impl->x3dListener.OrientFront.z = directionz;
    impl->x3dListener.OrientTop.x = upx;
    impl->x3dListener.OrientTop.y = upy;
    impl->x3dListener.OrientTop.z = upz;    
    impl->invalidated = true;
}

void XAudio2Listener::GetPosition(float& x, float& y, float& z) const
{
    x = impl->x3dListener.Position.x;
    y = impl->x3dListener.Position.y;
    z = impl->x3dListener.Position.z;
}

void XAudio2Listener::SetPosition(float x, float y, float z)
{
    impl->x3dListener.Position.x = x;
    impl->x3dListener.Position.y = y;
    impl->x3dListener.Position.z = z;        
    impl->invalidated = true;
}

void XAudio2Listener::GetVelocity(float& x, float& y, float& z) const 
{
    x = impl->x3dListener.Velocity.x;
    y = impl->x3dListener.Velocity.y;
    z = impl->x3dListener.Velocity.z;
}

void XAudio2Listener::SetVelocity(float x, float y, float z)
{
    impl->x3dListener.Velocity.x = x;
    impl->x3dListener.Velocity.y = y;
    impl->x3dListener.Velocity.z = z;
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
