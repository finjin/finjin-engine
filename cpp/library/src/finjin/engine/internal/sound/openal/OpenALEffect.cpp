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
#include "OpenALEffect.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "finjin/common/Math.hpp"
#include "OpenALEffectSlot.hpp"
#include "OpenALContext.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//OpenALEffect--------------------------------
OpenALEffect::OpenALEffect()
{
    this->context = nullptr;
    this->enabled = false;
    this->effectID = AL_EFFECT_NULL;
    this->invalidated = false;
    this->effectSlotsHead = nullptr;
}

OpenALEffect::~OpenALEffect()
{
}

void OpenALEffect::Create(OpenALContext* context)
{
    this->context = context;

    //Generate effect ID
    this->effectID = AL_EFFECT_NULL;

    auto& efx = this->context->GetEfxExtensions();
    if (efx.alGenEffects != nullptr)
    {
        efx.alGenEffects(1, &this->effectID);
        if (alGetError() == AL_NO_ERROR)
        {
            //Make sure effect can be set to specified type
            efx.alEffecti(this->effectID, AL_EFFECT_TYPE, _GetEffectType());
            if (alGetError() != AL_NO_ERROR)
            {
                //Failed to set type. Destroy effect
                efx.alDeleteEffects(1, &this->effectID);
                this->effectID = AL_EFFECT_NULL;
            }
        }
    }

    this->enabled = this->effectID != AL_EFFECT_NULL;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

void OpenALEffect::Destroy()
{
    //Notify effect slots
    FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(this->effectSlotsHead, effectNext, effectSlot)
        effectSlot->SetEffect(nullptr);

    //Delete effect ID
    if (this->effectID != AL_EFFECT_NULL)
    {
        auto& efx = this->context->GetEfxExtensions();
        efx.alDeleteEffects(1, &this->effectID);
    }
}

ALuint OpenALEffect::_GetEffectID() const
{
    return this->effectID;
}

bool OpenALEffect::IsValid() const
{
    return this->effectID != AL_EFFECT_NULL;
}

ALenum OpenALEffect::_GetEffectType() const
{
    return AL_EFFECT_NULL;
}

void OpenALEffect::Update()
{
    if (this->invalidated)
    {
        //The effect state has been changed

        if (this->enabled)
        {
            //Refresh all effect slots that use this effect
            FINJIN_INTRUSIVE_SINGLE_LIST_ITERATE(this->effectSlotsHead, effectNext, effectSlot)
                effectSlot->_RefreshEffect();
        }
        else
        {
            //Disable effect
            auto& efx = this->context->GetEfxExtensions();
            efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_NULL);
        }

        this->invalidated = false;
    }
}

void OpenALEffect::Enable(bool enable)
{
    this->enabled = enable;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

bool OpenALEffect::IsEnabled() const
{
    return this->enabled;
}

void OpenALEffect::AddEffectSlot(OpenALEffectSlot* effectSlot)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->effectSlotsHead, effectNext, effectSlot)
}

void OpenALEffect::RemoveEffectSlot(OpenALEffectSlot* effectSlot)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_REMOVE_ITEM(this->effectSlotsHead, effectNext, effectSlot)
}

//OpenALReverbEffect---------------------
OpenALReverbEffect::OpenALReverbEffect()
{
    this->density = AL_REVERB_DEFAULT_DENSITY;
    this->diffusion = AL_REVERB_DEFAULT_DIFFUSION;
    this->gain = AL_REVERB_DEFAULT_GAIN;
    this->gainHighFrequency = AL_REVERB_DEFAULT_GAINHF;
    this->decayTime = AL_REVERB_DEFAULT_DECAY_TIME;
    this->decayHighFrequencyRatio = AL_REVERB_DEFAULT_DECAY_HFRATIO;
    this->reflectionsGain = AL_REVERB_DEFAULT_REFLECTIONS_GAIN;
    this->reflectionsDelay = AL_REVERB_DEFAULT_REFLECTIONS_DELAY;
    this->lateReverbGain = AL_REVERB_DEFAULT_LATE_REVERB_GAIN;
    this->lateReverbDelay = AL_REVERB_DEFAULT_LATE_REVERB_DELAY;
    this->roomRolloffFactor = AL_REVERB_DEFAULT_ROOM_ROLLOFF_FACTOR;
    this->airAbsorptionGainHighFrequency = AL_REVERB_DEFAULT_AIR_ABSORPTION_GAINHF;
    this->decayHighFrequencyLimit = AL_REVERB_DEFAULT_DECAY_HFLIMIT ? true : false;
}

ALenum OpenALReverbEffect::_GetEffectType() const
{
    return AL_EFFECT_REVERB;
}

void OpenALReverbEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
        efx.alEffectf(this->effectID, AL_REVERB_DENSITY, this->density);
        efx.alEffectf(this->effectID, AL_REVERB_DIFFUSION, this->diffusion);
        efx.alEffectf(this->effectID, AL_REVERB_GAIN, this->gain);
        efx.alEffectf(this->effectID, AL_REVERB_GAINHF, this->gainHighFrequency);
        efx.alEffectf(this->effectID, AL_REVERB_DECAY_TIME, this->decayTime);
        efx.alEffectf(this->effectID, AL_REVERB_DECAY_HFRATIO, this->decayHighFrequencyRatio);
        efx.alEffectf(this->effectID, AL_REVERB_REFLECTIONS_GAIN, this->reflectionsGain);
        efx.alEffectf(this->effectID, AL_REVERB_REFLECTIONS_DELAY, this->reflectionsDelay);
        efx.alEffectf(this->effectID, AL_REVERB_LATE_REVERB_GAIN, this->lateReverbGain);
        efx.alEffectf(this->effectID, AL_REVERB_LATE_REVERB_DELAY, this->lateReverbDelay);
        efx.alEffectf(this->effectID, AL_REVERB_ROOM_ROLLOFF_FACTOR, this->roomRolloffFactor);
        efx.alEffectf(this->effectID, AL_REVERB_AIR_ABSORPTION_GAINHF, this->airAbsorptionGainHighFrequency);
        efx.alEffecti(this->effectID, AL_REVERB_DECAY_HFLIMIT, this->decayHighFrequencyLimit);
    }

    OpenALEffect::Update();
}

float OpenALReverbEffect::GetDensity() const
{
    return this->density;
}

void OpenALReverbEffect::SetDensity(float density)
{
    Limit(density, AL_REVERB_MIN_DENSITY, AL_REVERB_MAX_DENSITY);
    this->density = density;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetDiffusion() const
{
    return this->diffusion;
}

void OpenALReverbEffect::SetDiffusion(float diffusion)
{
    Limit(diffusion, AL_REVERB_MIN_DIFFUSION, AL_REVERB_MAX_DIFFUSION);
    this->diffusion = diffusion;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetGain() const
{
    return this->gain;
}

void OpenALReverbEffect::SetGain(float gain)
{
    Limit(gain, AL_REVERB_MIN_GAIN, AL_REVERB_MAX_GAIN);
    this->gain = gain;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetGainHighFrequency() const
{
    return this->gainHighFrequency;
}

void OpenALReverbEffect::SetGainHighFrequency(float gainHighFrequency)
{
    Limit(gainHighFrequency, AL_REVERB_MIN_GAINHF, AL_REVERB_MAX_GAINHF);
    this->gainHighFrequency = gainHighFrequency;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetDecayTime() const
{
    return this->decayTime;
}

void OpenALReverbEffect::SetDecayTime(float decayTime)
{
    Limit(decayTime, AL_REVERB_MIN_DECAY_TIME, AL_REVERB_MAX_DECAY_TIME);
    this->decayTime = decayTime;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetDecayHighFrequencyRatio() const
{
    return this->decayHighFrequencyRatio;
}

void OpenALReverbEffect::SetDecayHighFrequencyRatio(float decayHighFrequencyRatio)
{
    Limit(decayHighFrequencyRatio, AL_REVERB_MIN_DECAY_HFRATIO, AL_REVERB_MAX_DECAY_HFRATIO);
    this->decayHighFrequencyRatio = decayHighFrequencyRatio;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetReflectionsGain() const
{
    return this->reflectionsGain;
}

void OpenALReverbEffect::SetReflectionsGain(float reflectionsGain)
{
    Limit(reflectionsGain, AL_REVERB_MIN_REFLECTIONS_GAIN, AL_REVERB_MAX_REFLECTIONS_GAIN);
    this->reflectionsGain = reflectionsGain;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetReflectionsDelay() const
{
    return this->reflectionsDelay;
}

void OpenALReverbEffect::SetReflectionsDelay(float reflectionsDelay)
{
    Limit(reflectionsDelay, AL_REVERB_MIN_REFLECTIONS_DELAY, AL_REVERB_MAX_REFLECTIONS_DELAY);
    this->reflectionsDelay = reflectionsDelay;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetLateReverbGain() const
{
    return this->lateReverbGain;
}

void OpenALReverbEffect::SetLateReverbGain(float lateReverbGain)
{
    Limit(lateReverbGain, AL_REVERB_MIN_LATE_REVERB_GAIN, AL_REVERB_MAX_LATE_REVERB_GAIN);
    this->lateReverbGain = lateReverbGain;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetLateReverbDelay() const
{
    return this->lateReverbDelay;
}

void OpenALReverbEffect::SetLateReverbDelay(float lateReverbDelay)
{
    Limit(lateReverbDelay, AL_REVERB_MIN_LATE_REVERB_DELAY, AL_REVERB_MAX_LATE_REVERB_DELAY);
    this->lateReverbDelay = lateReverbDelay;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetRoomRolloffFactor() const
{
    return this->roomRolloffFactor;
}

void OpenALReverbEffect::SetRoomRolloffFactor(float roomRolloffFactor)
{
    Limit(roomRolloffFactor, AL_REVERB_MIN_ROOM_ROLLOFF_FACTOR, AL_REVERB_MAX_ROOM_ROLLOFF_FACTOR);
    this->roomRolloffFactor = roomRolloffFactor;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALReverbEffect::GetAirAbsorptionGainHighFrequency() const
{
    return this->airAbsorptionGainHighFrequency;
}

void OpenALReverbEffect::SetAirAbsorptionGainHighFrequency(float airAbsorptionGainHighFrequency)
{
    Limit(airAbsorptionGainHighFrequency, AL_REVERB_MIN_AIR_ABSORPTION_GAINHF, AL_REVERB_MAX_AIR_ABSORPTION_GAINHF);
    this->airAbsorptionGainHighFrequency = airAbsorptionGainHighFrequency;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

bool OpenALReverbEffect::GetDecayHighFrequencyLimit() const
{
    return this->decayHighFrequencyLimit;
}

void OpenALReverbEffect::SetDecayHighFrequencyLimit(bool decayHighFrequencyLimit)
{
    this->decayHighFrequencyLimit = decayHighFrequencyLimit;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//OpenALChorusEffect-----------------------------
OpenALChorusEffect::OpenALChorusEffect()
{
    this->waveformType = TRIANGLE_WAVE;
    this->phase = AL_CHORUS_DEFAULT_PHASE;
    this->rate = AL_CHORUS_DEFAULT_RATE;
    this->depth = AL_CHORUS_DEFAULT_DEPTH;
    this->feedback = AL_CHORUS_DEFAULT_FEEDBACK;
    this->delay = AL_CHORUS_DEFAULT_DELAY;
}

ALenum OpenALChorusEffect::_GetEffectType() const
{
    return AL_EFFECT_CHORUS;
}

void OpenALChorusEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_CHORUS);
        efx.alEffecti(this->effectID, AL_CHORUS_WAVEFORM, (int)this->waveformType);
        efx.alEffecti(this->effectID, AL_CHORUS_PHASE, this->phase);
        efx.alEffectf(this->effectID, AL_CHORUS_RATE, this->rate);
        efx.alEffectf(this->effectID, AL_CHORUS_DEPTH, this->depth);
        efx.alEffectf(this->effectID, AL_CHORUS_FEEDBACK, this->feedback);
        efx.alEffectf(this->effectID, AL_CHORUS_DELAY, this->delay);
    }

    OpenALEffect::Update();
}

OpenALChorusEffect::WaveformType OpenALChorusEffect::GetWaveformType() const
{
    return this->waveformType;
}

void OpenALChorusEffect::SetWaveformType(WaveformType waveformType)
{
    this->waveformType = waveformType;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

int OpenALChorusEffect::GetPhase() const
{
    return this->phase;
}

void OpenALChorusEffect::SetPhase(int phase)
{
    Limit(phase, AL_CHORUS_MIN_PHASE, AL_CHORUS_MAX_PHASE);
    this->phase = phase;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALChorusEffect::GetRate() const
{
    return this->rate;
}

void OpenALChorusEffect::SetRate(float rate)
{
    Limit(rate, AL_CHORUS_MIN_RATE, AL_CHORUS_MAX_RATE);
    this->rate = rate;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALChorusEffect::GetDepth() const
{
    return this->depth;
}

void OpenALChorusEffect::SetDepth(float depth)
{
    Limit(depth, AL_CHORUS_MIN_DEPTH, AL_CHORUS_MAX_DEPTH);
    this->depth = depth;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALChorusEffect::GetFeedback() const
{
    return this->feedback;
}

void OpenALChorusEffect::SetFeedback(float feedback)
{
    Limit(feedback, AL_CHORUS_MIN_FEEDBACK, AL_CHORUS_MAX_FEEDBACK);
    this->feedback = feedback;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALChorusEffect::GetDelay() const
{
    return this->delay;
}

void OpenALChorusEffect::SetDelay(float delay)
{
    Limit(delay, AL_CHORUS_MIN_DELAY, AL_CHORUS_MAX_DELAY);
    this->delay = delay;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//OpenALDistortionEffect--------------------------------
OpenALDistortionEffect::OpenALDistortionEffect()
{
    this->edge = AL_DISTORTION_DEFAULT_EDGE;
    this->gain = AL_DISTORTION_DEFAULT_GAIN;
    this->lowPassCutoff = AL_DISTORTION_DEFAULT_LOWPASS_CUTOFF;
    this->eqCenter = AL_DISTORTION_DEFAULT_EQCENTER;
    this->eqBandwidth = AL_DISTORTION_DEFAULT_EQBANDWIDTH;
}

ALenum OpenALDistortionEffect::_GetEffectType() const
{
    return AL_EFFECT_DISTORTION;
}

void OpenALDistortionEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_DISTORTION);
        efx.alEffectf(this->effectID, AL_DISTORTION_EDGE, this->edge);
        efx.alEffectf(this->effectID, AL_DISTORTION_GAIN, this->gain);
        efx.alEffectf(this->effectID, AL_DISTORTION_LOWPASS_CUTOFF, this->lowPassCutoff);
        efx.alEffectf(this->effectID, AL_DISTORTION_EQCENTER, this->eqCenter);
        efx.alEffectf(this->effectID, AL_DISTORTION_EQBANDWIDTH, this->eqBandwidth);
    }

    OpenALEffect::Update();
}

float OpenALDistortionEffect::GetEdge() const
{
    return this->edge;
}

void OpenALDistortionEffect::SetEdge(float edge)
{
    Limit(edge, AL_DISTORTION_MIN_EDGE, AL_DISTORTION_MAX_EDGE);
    this->edge = edge;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALDistortionEffect::GetGain() const
{
    return this->gain;
}

void OpenALDistortionEffect::SetGain(float gain)
{
    Limit(gain, AL_DISTORTION_MIN_GAIN, AL_DISTORTION_MAX_GAIN);
    this->gain = gain;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALDistortionEffect::GetLowPassCutoff() const
{
    return this->lowPassCutoff;
}

void OpenALDistortionEffect::SetLowPassCutoff(float lowPassCutoff)
{
    Limit(lowPassCutoff, AL_DISTORTION_MIN_LOWPASS_CUTOFF, AL_DISTORTION_MAX_LOWPASS_CUTOFF);
    this->lowPassCutoff = lowPassCutoff;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALDistortionEffect::GetEqCenter() const
{
    return this->eqCenter;
}

void OpenALDistortionEffect::SetEqCenter(float eqCenter)
{
    Limit(eqCenter, AL_DISTORTION_MIN_EQCENTER, AL_DISTORTION_MAX_EQCENTER);
    this->eqCenter = eqCenter;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALDistortionEffect::GetEqBandwidth() const
{
    return this->eqBandwidth;
}

void OpenALDistortionEffect::SetEqBandwidth(float eqBandwidth)
{
    Limit(eqBandwidth, AL_DISTORTION_MIN_EQBANDWIDTH, AL_DISTORTION_MAX_EQBANDWIDTH);
    this->eqBandwidth = eqBandwidth;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//EchoEffect-----------------------------
OpenALEchoEffect::OpenALEchoEffect()
{
    this->delay = AL_ECHO_DEFAULT_DELAY;
    this->lrDelay = AL_ECHO_DEFAULT_LRDELAY;
    this->damping = AL_ECHO_DEFAULT_DAMPING;
    this->feedback = AL_ECHO_DEFAULT_FEEDBACK;
    this->spread = AL_ECHO_DEFAULT_SPREAD;
}

ALenum OpenALEchoEffect::_GetEffectType() const
{
    return AL_EFFECT_ECHO;
}

void OpenALEchoEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_ECHO);
        efx.alEffectf(this->effectID, AL_ECHO_DELAY, this->delay);
        efx.alEffectf(this->effectID, AL_ECHO_LRDELAY, this->lrDelay);
        efx.alEffectf(this->effectID, AL_ECHO_DAMPING, this->damping);
        efx.alEffectf(this->effectID, AL_ECHO_FEEDBACK, this->feedback);
        efx.alEffectf(this->effectID, AL_ECHO_SPREAD, this->spread);
    }

    OpenALEffect::Update();
}

float OpenALEchoEffect::GetDelay() const
{
    return this->delay;
}

void OpenALEchoEffect::SetDelay(float delay)
{
    Limit(delay, AL_ECHO_MIN_DELAY, AL_ECHO_MAX_DELAY);
    this->delay = delay;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEchoEffect::GetLrDelay() const
{
    return this->lrDelay;
}

void OpenALEchoEffect::SetLrDelay(float lrDelay)
{
    Limit(lrDelay, AL_ECHO_MIN_LRDELAY, AL_ECHO_MAX_LRDELAY);
    this->lrDelay = lrDelay;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEchoEffect::GetDamping() const
{
    return this->damping;
}

void OpenALEchoEffect::SetDamping(float damping)
{
    Limit(damping, AL_ECHO_MIN_DAMPING, AL_ECHO_MAX_DAMPING);
    this->damping = damping;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEchoEffect::GetFeedback() const
{
    return this->feedback;
}

void OpenALEchoEffect::SetFeedback(float feedback)
{
    Limit(feedback, AL_ECHO_MIN_FEEDBACK, AL_ECHO_MAX_FEEDBACK);
    this->feedback = feedback;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEchoEffect::GetSpread() const
{
    return this->spread;
}

void OpenALEchoEffect::SetSpread(float spread)
{
    Limit(spread, AL_ECHO_MIN_SPREAD, AL_ECHO_MAX_SPREAD);
    this->spread = spread;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//OpenALFlangerEffect---------------------------------------
OpenALFlangerEffect::OpenALFlangerEffect()
{
    this->waveformType = TRIANGLE_WAVE;
    this->phase = AL_FLANGER_DEFAULT_PHASE;
    this->rate = AL_FLANGER_DEFAULT_RATE;
    this->depth = AL_FLANGER_DEFAULT_DEPTH;
    this->feedback = AL_FLANGER_DEFAULT_FEEDBACK;
    this->delay = AL_FLANGER_DEFAULT_DELAY;
}

ALenum OpenALFlangerEffect::_GetEffectType() const
{
    return AL_EFFECT_FLANGER;
}

void OpenALFlangerEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_FLANGER);
        efx.alEffecti(this->effectID, AL_FLANGER_WAVEFORM, (int)this->waveformType);
        efx.alEffecti(this->effectID, AL_FLANGER_PHASE, this->phase);
        efx.alEffectf(this->effectID, AL_FLANGER_RATE, this->rate);
        efx.alEffectf(this->effectID, AL_FLANGER_DEPTH, this->depth);
        efx.alEffectf(this->effectID, AL_FLANGER_FEEDBACK, this->feedback);
        efx.alEffectf(this->effectID, AL_FLANGER_DELAY, this->delay);
    }

    OpenALEffect::Update();
}

OpenALFlangerEffect::WaveformType OpenALFlangerEffect::GetWaveformType() const
{
    return this->waveformType;
}

void OpenALFlangerEffect::SetWaveformType(WaveformType waveformType)
{
    this->waveformType = waveformType;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

int OpenALFlangerEffect::GetPhase() const
{
    return this->phase;
}

void OpenALFlangerEffect::SetPhase(int phase)
{
    Limit(phase, AL_FLANGER_MIN_PHASE, AL_FLANGER_MAX_PHASE);
    this->phase = phase;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALFlangerEffect::GetRate() const
{
    return this->rate;
}

void OpenALFlangerEffect::SetRate(float rate)
{
    Limit(rate, AL_FLANGER_MIN_RATE, AL_FLANGER_MAX_RATE);
    this->rate = rate;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALFlangerEffect::GetDepth() const
{
    return this->depth;
}

void OpenALFlangerEffect::SetDepth(float depth)
{
    Limit(depth, AL_FLANGER_MIN_DEPTH, AL_FLANGER_MAX_DEPTH);
    this->depth = depth;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALFlangerEffect::GetFeedback() const
{
    return this->feedback;
}

void OpenALFlangerEffect::SetFeedback(float feedback)
{
    Limit(feedback, AL_FLANGER_MIN_FEEDBACK, AL_FLANGER_MAX_FEEDBACK);
    this->feedback = feedback;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALFlangerEffect::GetDelay() const
{
    return this->delay;
}

void OpenALFlangerEffect::SetDelay(float delay)
{
    Limit(delay, AL_FLANGER_MIN_DELAY, AL_FLANGER_MAX_DELAY);
    this->delay = delay;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//OpenALFrequencyShifterEffect------------------------------
OpenALFrequencyShifterEffect::OpenALFrequencyShifterEffect()
{
    this->frequency = AL_FREQUENCY_SHIFTER_DEFAULT_FREQUENCY;
    this->leftDirection = DOWN_DIRECTION;
    this->rightDirection = DOWN_DIRECTION;
}

ALenum OpenALFrequencyShifterEffect::_GetEffectType() const
{
    return AL_EFFECT_FREQUENCY_SHIFTER;
}

void OpenALFrequencyShifterEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_FREQUENCY_SHIFTER);
        efx.alEffectf(this->effectID, AL_FREQUENCY_SHIFTER_FREQUENCY, this->frequency);
        efx.alEffecti(this->effectID, AL_FREQUENCY_SHIFTER_LEFT_DIRECTION, (int)this->leftDirection);
        efx.alEffecti(this->effectID, AL_FREQUENCY_SHIFTER_RIGHT_DIRECTION, (int)this->rightDirection);
    }

    OpenALEffect::Update();
}

float OpenALFrequencyShifterEffect::GetFrequency() const
{
    return this->frequency;
}

void OpenALFrequencyShifterEffect::SetFrequency(float frequency)
{
    Limit(frequency, AL_FREQUENCY_SHIFTER_MIN_FREQUENCY, AL_FREQUENCY_SHIFTER_MAX_FREQUENCY);
    this->frequency = frequency;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

OpenALFrequencyShifterEffect::Direction OpenALFrequencyShifterEffect::GetLeftDirection() const
{
    return this->leftDirection;
}

void OpenALFrequencyShifterEffect::SetLeftDirection(Direction leftDirection)
{
    this->leftDirection = leftDirection;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

OpenALFrequencyShifterEffect::Direction OpenALFrequencyShifterEffect::GetRightDirection() const
{
    return this->rightDirection;
}

void OpenALFrequencyShifterEffect::SetRightDirection(Direction rightDirection)
{
    this->rightDirection = rightDirection;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//OpenALVocalMorpherEffect-----------------------
OpenALVocalMorpherEffect::OpenALVocalMorpherEffect()
{
    this->phonemeA = AL_VOCAL_MORPHER_DEFAULT_PHONEMEA;
    this->phonemeB = AL_VOCAL_MORPHER_DEFAULT_PHONEMEB;
    this->phonemeACoarseTuning = AL_VOCAL_MORPHER_MIN_PHONEMEA_COARSE_TUNING;
    this->phonemeBCoarseTuning = AL_VOCAL_MORPHER_MIN_PHONEMEB_COARSE_TUNING;
    this->waveformType = SIN_WAVE;
    this->rate = AL_VOCAL_MORPHER_DEFAULT_RATE;
}

ALenum OpenALVocalMorpherEffect::_GetEffectType() const
{
    return AL_EFFECT_VOCAL_MORPHER;
}

void OpenALVocalMorpherEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_VOCAL_MORPHER);
        efx.alEffecti(this->effectID, AL_VOCAL_MORPHER_PHONEMEA, this->phonemeA);
        efx.alEffecti(this->effectID, AL_VOCAL_MORPHER_PHONEMEB, this->phonemeB);
        efx.alEffecti(this->effectID, AL_VOCAL_MORPHER_PHONEMEA_COARSE_TUNING, this->phonemeACoarseTuning);
        efx.alEffecti(this->effectID, AL_VOCAL_MORPHER_PHONEMEB_COARSE_TUNING, this->phonemeBCoarseTuning);
        efx.alEffecti(this->effectID, AL_VOCAL_MORPHER_WAVEFORM, (int)this->waveformType);
        efx.alEffectf(this->effectID, AL_VOCAL_MORPHER_RATE, this->rate);
    }

    OpenALEffect::Update();
}

int OpenALVocalMorpherEffect::GetPhonemeA() const
{
    return this->phonemeA;
}

void OpenALVocalMorpherEffect::SetPhonemeA(int phonemeA)
{
    Limit(phonemeA, AL_VOCAL_MORPHER_MIN_PHONEMEA, AL_VOCAL_MORPHER_MAX_PHONEMEA);
    this->phonemeA = phonemeA;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

int OpenALVocalMorpherEffect::GetPhonemeB() const
{
    return this->phonemeB;
}

void OpenALVocalMorpherEffect::SetPhonemeB(int phonemeB)
{
    Limit(phonemeB, AL_VOCAL_MORPHER_MIN_PHONEMEB, AL_VOCAL_MORPHER_MAX_PHONEMEB);
    this->phonemeB = phonemeB;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

int OpenALVocalMorpherEffect::GetPhonemeACoarseTuning() const
{
    return this->phonemeACoarseTuning;
}

void OpenALVocalMorpherEffect::GetPhonemeACoarseTuning(int phonemeACoarseTuning)
{
    Limit(phonemeACoarseTuning, AL_VOCAL_MORPHER_MIN_PHONEMEA_COARSE_TUNING, AL_VOCAL_MORPHER_MAX_PHONEMEA_COARSE_TUNING);
    this->phonemeACoarseTuning = phonemeACoarseTuning;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

int OpenALVocalMorpherEffect::GetPhonemeBCoarseTuning() const
{
    return this->phonemeBCoarseTuning;
}

void OpenALVocalMorpherEffect::GetPhonemeBCoarseTuning(int phonemeBCoarseTuning)
{
    Limit(phonemeBCoarseTuning, AL_VOCAL_MORPHER_MIN_PHONEMEB_COARSE_TUNING, AL_VOCAL_MORPHER_MAX_PHONEMEB_COARSE_TUNING);
    this->phonemeBCoarseTuning = phonemeBCoarseTuning;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

OpenALVocalMorpherEffect::WaveformType OpenALVocalMorpherEffect::GetWaveformType() const
{
    return this->waveformType;
}

void OpenALVocalMorpherEffect::SetWaveformType(WaveformType waveformType)
{
    this->waveformType = waveformType;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALVocalMorpherEffect::GetRate() const
{
    return this->rate;
}

void OpenALVocalMorpherEffect::SetRate(float rate)
{
    Limit(rate, AL_VOCAL_MORPHER_MIN_RATE, AL_VOCAL_MORPHER_MAX_RATE);
    this->rate = rate;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//OpenALPitchShifterEffect----------------------------
OpenALPitchShifterEffect::OpenALPitchShifterEffect()
{
    this->coarseTune = AL_PITCH_SHIFTER_DEFAULT_COARSE_TUNE;
    this->fineTune = AL_PITCH_SHIFTER_DEFAULT_FINE_TUNE;
}

ALenum OpenALPitchShifterEffect::_GetEffectType() const
{
    return AL_EFFECT_PITCH_SHIFTER;
}

void OpenALPitchShifterEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_PITCH_SHIFTER);
        efx.alEffecti(this->effectID, AL_PITCH_SHIFTER_COARSE_TUNE, this->coarseTune);
        efx.alEffecti(this->effectID, AL_PITCH_SHIFTER_FINE_TUNE, this->fineTune);
    }

    OpenALEffect::Update();
}

int OpenALPitchShifterEffect::GetCoarseTune() const
{
    return this->coarseTune;
}

void OpenALPitchShifterEffect::SetCoarseTune(int coarseTune)
{
    Limit(coarseTune, AL_PITCH_SHIFTER_MIN_COARSE_TUNE, AL_PITCH_SHIFTER_MAX_COARSE_TUNE);
    this->coarseTune = coarseTune;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

int OpenALPitchShifterEffect::GetFineTune() const
{
    return this->fineTune;
}

void OpenALPitchShifterEffect::SetFineTune(int fineTune)
{
    Limit(fineTune, AL_PITCH_SHIFTER_MIN_FINE_TUNE, AL_PITCH_SHIFTER_MAX_FINE_TUNE);
    this->fineTune = fineTune;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//OpenALRingModulatorEffect------------------------------
OpenALRingModulatorEffect::OpenALRingModulatorEffect()
{
    this->frequency = AL_RING_MODULATOR_DEFAULT_FREQUENCY;
    this->highPassCutoff = AL_RING_MODULATOR_DEFAULT_HIGHPASS_CUTOFF;
    this->waveformType = SIN_WAVE;
}

ALenum OpenALRingModulatorEffect::_GetEffectType() const
{
    return AL_EFFECT_RING_MODULATOR;
}

void OpenALRingModulatorEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_RING_MODULATOR);
        efx.alEffectf(this->effectID, AL_RING_MODULATOR_FREQUENCY, this->frequency);
        efx.alEffectf(this->effectID, AL_RING_MODULATOR_HIGHPASS_CUTOFF, this->highPassCutoff);
        efx.alEffecti(this->effectID, AL_RING_MODULATOR_WAVEFORM, (int)this->waveformType);
    }

    OpenALEffect::Update();
}

float OpenALRingModulatorEffect::GetFrequency() const
{
    return this->frequency;
}

void OpenALRingModulatorEffect::SetFrequency(float frequency)
{
    Limit(frequency, AL_RING_MODULATOR_MIN_FREQUENCY, AL_RING_MODULATOR_MAX_FREQUENCY);
    this->frequency = frequency;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALRingModulatorEffect::GetHighPassCutoff() const
{
    return this->highPassCutoff;
}

void OpenALRingModulatorEffect::SetHighPassCutoff(float highPassCutoff)
{
    Limit(highPassCutoff, AL_RING_MODULATOR_MIN_HIGHPASS_CUTOFF, AL_RING_MODULATOR_MAX_HIGHPASS_CUTOFF);
    this->highPassCutoff = highPassCutoff;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

OpenALRingModulatorEffect::WaveformType OpenALRingModulatorEffect::GetWaveformType() const
{
    return this->waveformType;
}

void OpenALRingModulatorEffect::SetWaveformType(WaveformType waveformType)
{
    this->waveformType = waveformType;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//OpenALAutoWahEffect-----------------------------
OpenALAutoWahEffect::OpenALAutoWahEffect()
{
    this->attackTime = AL_AUTOWAH_DEFAULT_ATTACK_TIME;
    this->releaseTime = AL_AUTOWAH_DEFAULT_RELEASE_TIME;
    this->resonance = AL_AUTOWAH_DEFAULT_RESONANCE;
    this->peakGain = AL_AUTOWAH_DEFAULT_PEAK_GAIN;
}

ALenum OpenALAutoWahEffect::_GetEffectType() const
{
    return AL_EFFECT_AUTOWAH;
}

void OpenALAutoWahEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_AUTOWAH);
        efx.alEffectf(this->effectID, AL_AUTOWAH_ATTACK_TIME, this->attackTime);
        efx.alEffectf(this->effectID, AL_AUTOWAH_RELEASE_TIME, this->releaseTime);
        efx.alEffectf(this->effectID, AL_AUTOWAH_RESONANCE, this->resonance);
        efx.alEffectf(this->effectID, AL_AUTOWAH_PEAK_GAIN, this->peakGain);
    }

    OpenALEffect::Update();
}

float OpenALAutoWahEffect::GetAttackTime() const
{
    return this->attackTime;
}

void OpenALAutoWahEffect::SetAttackTime(float attackTime)
{
    Limit(attackTime, AL_AUTOWAH_MIN_ATTACK_TIME, AL_AUTOWAH_MAX_ATTACK_TIME);
    this->attackTime = attackTime;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALAutoWahEffect::GetReleaseTime() const
{
    return this->releaseTime;
}

void OpenALAutoWahEffect::SetReleaseTime(float releaseTime)
{
    Limit(releaseTime, AL_AUTOWAH_MIN_RELEASE_TIME, AL_AUTOWAH_MAX_RELEASE_TIME);
    this->releaseTime = releaseTime;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALAutoWahEffect::GetResonance() const
{
    return this->resonance;
}

void OpenALAutoWahEffect::SetResonance(float resonance)
{
    Limit(resonance, AL_AUTOWAH_MIN_RESONANCE, AL_AUTOWAH_MAX_RESONANCE);
    this->resonance = resonance;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALAutoWahEffect::GetPeakGain() const
{
    return this->peakGain;
}

void OpenALAutoWahEffect::SetPeakGain(float peakGain)
{
    Limit(peakGain, AL_AUTOWAH_MIN_PEAK_GAIN, AL_AUTOWAH_MAX_PEAK_GAIN);
    this->peakGain = peakGain;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//OpenALCompressorEffect--------------------------
OpenALCompressorEffect::OpenALCompressorEffect()
{
    this->on = AL_COMPRESSOR_DEFAULT_ONOFF ? true : false;
}

ALenum OpenALCompressorEffect::_GetEffectType() const
{
    return AL_EFFECT_COMPRESSOR;
}

void OpenALCompressorEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_COMPRESSOR);
        efx.alEffecti(this->effectID, AL_COMPRESSOR_ONOFF, this->on);
    }

    OpenALEffect::Update();
}

bool OpenALCompressorEffect::GetOn() const
{
    return this->on;
}

void OpenALCompressorEffect::SetOn(bool on)
{
    this->on = on;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

//OpenALEqualizerEffect---------------------------------------
OpenALEqualizerEffect::OpenALEqualizerEffect()
{
    this->lowGain = AL_EQUALIZER_DEFAULT_LOW_GAIN;
    this->lowCutoff = AL_EQUALIZER_DEFAULT_LOW_CUTOFF;
    this->mid1Gain = AL_EQUALIZER_DEFAULT_MID1_GAIN;
    this->mid1Center = AL_EQUALIZER_DEFAULT_MID1_CENTER;
    this->mid1Width = AL_EQUALIZER_DEFAULT_MID1_WIDTH;
    this->mid2Gain = AL_EQUALIZER_DEFAULT_MID2_GAIN;
    this->mid2Center = AL_EQUALIZER_DEFAULT_MID2_CENTER;
    this->mid2Width = AL_EQUALIZER_DEFAULT_MID2_WIDTH;
    this->highGain = AL_EQUALIZER_DEFAULT_HIGH_GAIN;
    this->highCutoff = AL_EQUALIZER_DEFAULT_HIGH_CUTOFF;
}

ALenum OpenALEqualizerEffect::_GetEffectType() const
{
    return AL_EFFECT_EQUALIZER;
}

void OpenALEqualizerEffect::Update()
{
    if (this->invalidated && this->enabled)
    {
        auto& efx = this->context->GetEfxExtensions();

        efx.alEffecti(this->effectID, AL_EFFECT_TYPE, AL_EFFECT_EQUALIZER);
        efx.alEffectf(this->effectID, AL_EQUALIZER_LOW_GAIN, this->lowGain);
        efx.alEffectf(this->effectID, AL_EQUALIZER_LOW_CUTOFF, this->lowCutoff);
        efx.alEffectf(this->effectID, AL_EQUALIZER_MID1_GAIN, this->mid1Gain);
        efx.alEffectf(this->effectID, AL_EQUALIZER_MID1_CENTER, this->mid1Center);
        efx.alEffectf(this->effectID, AL_EQUALIZER_MID1_WIDTH, this->mid1Width);
        efx.alEffectf(this->effectID, AL_EQUALIZER_MID2_GAIN, this->mid2Gain);
        efx.alEffectf(this->effectID, AL_EQUALIZER_MID2_CENTER, this->mid2Center);
        efx.alEffectf(this->effectID, AL_EQUALIZER_MID2_WIDTH, this->mid2Width);
        efx.alEffectf(this->effectID, AL_EQUALIZER_HIGH_GAIN, this->highGain);
        efx.alEffectf(this->effectID, AL_EQUALIZER_HIGH_CUTOFF, this->highCutoff);
    }

    OpenALEffect::Update();
}

float OpenALEqualizerEffect::GetLowGain() const
{
    return this->lowGain;
}

void OpenALEqualizerEffect::SetLowGain(float lowGain)
{
    Limit(lowGain, AL_EQUALIZER_MIN_LOW_GAIN, AL_EQUALIZER_MAX_LOW_GAIN);
    this->lowGain = lowGain;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEqualizerEffect::GetLowCutoff() const
{
    return this->lowCutoff;
}

void OpenALEqualizerEffect::SetLowCutoff(float lowCutoff)
{
    Limit(lowCutoff, AL_EQUALIZER_MIN_LOW_CUTOFF, AL_EQUALIZER_MAX_LOW_CUTOFF);
    this->lowCutoff = lowCutoff;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEqualizerEffect::GetMid1Gain() const
{
    return this->mid1Gain;
}

void OpenALEqualizerEffect::SetMid1Gain(float mid1Gain)
{
    Limit(mid1Gain, AL_EQUALIZER_MIN_MID1_GAIN, AL_EQUALIZER_MAX_MID1_GAIN);
    this->mid1Gain = mid1Gain;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEqualizerEffect::GetMid1Center() const
{
    return this->mid1Center;
}

void OpenALEqualizerEffect::SetMid1Center(float mid1Center)
{
    Limit(mid1Center, AL_EQUALIZER_MIN_MID1_CENTER, AL_EQUALIZER_MAX_MID1_CENTER);
    this->mid1Center = mid1Center;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEqualizerEffect::GetMid1Width() const
{
    return this->mid1Width;
}

void OpenALEqualizerEffect::SetMid1Width(float mid1Width)
{
    Limit(mid1Width, AL_EQUALIZER_MIN_MID1_WIDTH, AL_EQUALIZER_MAX_MID1_WIDTH);
    this->mid1Width = mid1Width;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEqualizerEffect::GetMid2Gain() const
{
    return this->mid2Gain;
}

void OpenALEqualizerEffect::SetMid2Gain(float mid2Gain)
{
    Limit(mid2Gain, AL_EQUALIZER_MIN_MID2_GAIN, AL_EQUALIZER_MAX_MID2_GAIN);
    this->mid2Gain = mid2Gain;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEqualizerEffect::GetMid2Center() const
{
    return this->mid2Center;
}

void OpenALEqualizerEffect::SetMid2Center(float mid2Center)
{
    Limit(mid2Center, AL_EQUALIZER_MIN_MID2_CENTER, AL_EQUALIZER_MAX_MID2_CENTER);
    this->mid2Center = mid2Center;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEqualizerEffect::GetMid2Width() const
{
    return this->mid2Width;
}

void OpenALEqualizerEffect::SetMid2Width(float mid2Width)
{
    Limit(mid2Width, AL_EQUALIZER_MIN_MID2_WIDTH, AL_EQUALIZER_MAX_MID2_WIDTH);
    this->mid2Width = mid2Width;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEqualizerEffect::GetHighGain() const
{
    return this->highGain;
}

void OpenALEqualizerEffect::SetHighGain(float highGain)
{
    Limit(mid2Gain, AL_EQUALIZER_MIN_HIGH_GAIN, AL_EQUALIZER_MAX_HIGH_GAIN);
    this->highGain = highGain;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}

float OpenALEqualizerEffect::GetHighCutoff() const
{
    return this->highCutoff;
}

void OpenALEqualizerEffect::SetHighCutoff(float highCutoff)
{
    Limit(highCutoff, AL_EQUALIZER_MIN_HIGH_CUTOFF, AL_EQUALIZER_MAX_HIGH_CUTOFF);
    this->highCutoff = highCutoff;
    this->invalidated = this->effectID != AL_EFFECT_NULL;
}
