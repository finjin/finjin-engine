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


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class XAudio2Context;
    class XAudio2ListenerImpl;

    class XAudio2Listener
    {
    public:
        XAudio2Listener();
        ~XAudio2Listener();

        void Create(XAudio2Context* context);

        void SetDefaults();

        void MakeDirectional();
        void MakeOmnidirectional();
        
        void Set
            (
            float x, float y, float z,
            float vx, float vy, float vz,
            float directionx, float directiony, float directionz,
            float upx, float upy, float upz
            );

        void GetOrientation
            (
            float& directionx, float& directiony, float& directionz,
            float& upx, float& upy, float& upz
            ) const;
        void SetOrientation
            (
            float directionx, float directiony, float directionz,
            float upx, float upy, float upz
            );

        void GetPosition(float& x, float& y, float& z) const;
        void SetPosition(float x, float y, float z);
        
        void GetVelocity(float& x, float& y, float& z) const;
        void SetVelocity(float x, float y, float z);

        void Commit(bool force = false);

        //XAudio-related methods--------------
        void GetConeAngle(Angle& inner, Angle& outer) const;
        void SetConeAngle(Angle inner, Angle outer);

        void GetConeVolume(float& inner, float& outer) const;
        void SetConeVolume(float inner, float outer);

        void GetConeReverb(float& inner, float& outer) const;
        void SetConeReverb(float inner, float outer);

        void GetConeCoefficients(float& inner, float& outer) const;
        void SetConeCoefficients(float inner, float outer);

        XAudio2ListenerImpl* GetImpl();

    private:
        std::unique_ptr<XAudio2ListenerImpl> impl;
    };

} }
