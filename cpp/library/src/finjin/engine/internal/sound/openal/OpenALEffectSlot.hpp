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
#include "OpenALIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class OpenALContext;
    class OpenALSoundSource;
    class OpenALEffect;

    class OpenALEffectSlot
    {
        friend class OpenALContext;

    public:
        OpenALEffectSlot();
        ~OpenALEffectSlot();

        void Create(OpenALContext* context, ALuint effectSlotID);

        void SetEffect(OpenALEffect* effect);

        void _RefreshEffect();

        ALuint _GetEffectSlotID() const;

        void Update();

        float GetGain() const;
        void SetGain(float gain);

        bool GetAutoAdjust() const;
        void SetAutoAdjust(bool autoAdjust);

    private:
        OpenALContext* context;
        ALuint effectSlotID;
        bool invalidated;
        OpenALEffect* effect;

        float gain;
        bool autoAdjust;

    public:
        OpenALEffectSlot* effectNext;
    };

} }
