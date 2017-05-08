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
            const MathVector3& position,
            const MathVector3& velocity,
            const MathVector3& direction,
            const MathVector3& up
            );

        void GetOrientation(MathVector3& direction, MathVector3& up) const;
        void SetOrientation(const MathVector3& direction, const MathVector3& up);

        void GetPosition(MathVector3& value) const;
        void SetPosition(const MathVector3& value);

        void GetVelocity(MathVector3& value) const;
        void SetVelocity(const MathVector3& value);

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
