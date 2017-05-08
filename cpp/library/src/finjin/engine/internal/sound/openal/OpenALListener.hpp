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
#include "finjin/common/Math.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OpenALContext;

    /**
     * Manages access to the device context's one and only listener.
     * Many sound listeners can be created, but only one can be Select()ed at a time.
     */
    class OpenALListener
    {
    public:
        OpenALListener();

        void Create(OpenALContext* context);

        void SetDefaults();

        void Set(const MathVector3& position);

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

    private:
        OpenALContext* context;
        bool invalidated;
        MathVector3 position;
        MathVector3 orientation[2];
        MathVector3 velocity;
    };

} }
