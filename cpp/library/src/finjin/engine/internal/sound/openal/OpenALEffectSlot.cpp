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
#include "OpenALEffectSlot.hpp"
#include "finjin/common/Math.hpp"
#include "OpenALEffect.hpp"
#include "OpenALContext.hpp"
#include "OpenALSoundSource.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
OpenALEffectSlot::OpenALEffectSlot()
{
    this->context = nullptr;
    this->effectSlotID = AL_EFFECTSLOT_NULL;
    this->invalidated = false;
    this->effect = nullptr;

    this->gain = 1.0f;
    this->autoAdjust = true;

    this->effectNext = nullptr;
}

OpenALEffectSlot::~OpenALEffectSlot()
{
}

void OpenALEffectSlot::Create(OpenALContext* context, ALuint effectSlotID)
{
    this->context = context;
    this->effectSlotID = effectSlotID;
    this->invalidated = this->effectSlotID != AL_EFFECTSLOT_NULL;
}

void OpenALEffectSlot::SetEffect(OpenALEffect* effect)
{
    //Remove this source from existing effect
    if (this->effect != nullptr)
        this->effect->RemoveEffectSlot(this);

    //Set new effect
    this->effect = effect;

    //Add this effect slot to new effect
    ALuint effectID = 0;
    if (this->effect != nullptr)
    {
        effectID = this->effect->_GetEffectID();
        this->effect->AddEffectSlot(this);
    }

    if (this->effectSlotID != AL_EFFECTSLOT_NULL)
    {
        auto& efx = this->context->GetEfxExtensions();
        efx.alAuxiliaryEffectSloti(this->effectSlotID, AL_EFFECTSLOT_EFFECT, effectID);
    }
}

void OpenALEffectSlot::_RefreshEffect()
{
    if (this->effectSlotID != AL_EFFECTSLOT_NULL)
    {
        ALuint effectID = 0;
        if (this->effect != nullptr)
            effectID = this->effect->_GetEffectID();

        auto& efx = this->context->GetEfxExtensions();
        efx.alAuxiliaryEffectSloti(this->effectSlotID, AL_EFFECTSLOT_EFFECT, effectID);
    }
}

ALuint OpenALEffectSlot::_GetEffectSlotID() const
{
    return this->effectSlotID;
}

void OpenALEffectSlot::Update()
{
    if (this->invalidated)
    {
        auto& efx = this->context->GetEfxExtensions();
        efx.alAuxiliaryEffectSlotf(this->effectSlotID, AL_EFFECTSLOT_GAIN, this->gain);
        efx.alAuxiliaryEffectSloti(this->effectSlotID, AL_EFFECTSLOT_AUXILIARY_SEND_AUTO, this->autoAdjust);
    }
}

float OpenALEffectSlot::GetGain() const
{
    return this->gain;
}

void OpenALEffectSlot::SetGain(float gain)
{
    Limit(gain, 0.0f, 1.0f);
    this->gain = gain;
    this->invalidated = this->effectSlotID != AL_EFFECTSLOT_NULL;
}

bool OpenALEffectSlot::GetAutoAdjust() const
{
    return this->autoAdjust;
}

void OpenALEffectSlot::SetAutoAdjust(bool autoAdjust)
{
    this->autoAdjust = autoAdjust;
    this->invalidated = this->effectSlotID != AL_EFFECTSLOT_NULL;
}
