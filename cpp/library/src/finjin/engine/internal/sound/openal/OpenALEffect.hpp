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

    class OpenALEffectSlot;
    class OpenALContext;

    /** Base effect class */
    class OpenALEffect
    {
    public:
        OpenALEffect();
        virtual ~OpenALEffect();

        void Create(OpenALContext* context);
        void Destroy();

        ALuint _GetEffectID() const;

        bool IsValid() const;

        virtual ALenum _GetEffectType() const;

        virtual void Update();

        void Enable(bool enable = true);
        bool IsEnabled() const;

        void AddEffectSlot(OpenALEffectSlot* effectSlot);
        void RemoveEffectSlot(OpenALEffectSlot* effectSlot);

    protected:
        OpenALContext* context;
        ALuint effectID;
        bool invalidated;
        bool enabled;
        OpenALEffectSlot* effectSlotsHead; //Linked list of effect slots that this effect is used in. Use effectSlotsHead->effectNext to iterate through list
    };

    class OpenALReverbEffect : public OpenALEffect
    {
    public:
        OpenALReverbEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        float GetDensity() const;
        void SetDensity(float density);

        float GetDiffusion() const;
        void SetDiffusion(float diffusion);

        float GetGain() const;
        void SetGain(float gain);

        float GetGainHighFrequency() const;
        void SetGainHighFrequency(float gainHighFrequency);

        float GetDecayTime() const;
        void SetDecayTime(float decayTime);

        float GetDecayHighFrequencyRatio() const;
        void SetDecayHighFrequencyRatio(float decayHighFrequencyRatio);

        float GetReflectionsGain() const;
        void SetReflectionsGain(float reflectionsGain);

        float GetReflectionsDelay() const;
        void SetReflectionsDelay(float reflectionsDelay);

        float GetLateReverbGain() const;
        void SetLateReverbGain(float lateReverbGain);

        float GetLateReverbDelay() const;
        void SetLateReverbDelay(float lateReverbDelay);

        float GetRoomRolloffFactor() const;
        void SetRoomRolloffFactor(float roomRolloffFactor);

        float GetAirAbsorptionGainHighFrequency() const;
        void SetAirAbsorptionGainHighFrequency(float airAbsorptionGainHighFrequency);

        bool GetDecayHighFrequencyLimit() const;
        void SetDecayHighFrequencyLimit(bool decayHighFrequencyLimit);

    protected:
        float density;
        float diffusion;
        float gain;
        float gainHighFrequency;
        float decayTime;
        float decayHighFrequencyRatio;
        float reflectionsGain;
        float reflectionsDelay;
        float lateReverbGain;
        float lateReverbDelay;
        float roomRolloffFactor;
        float airAbsorptionGainHighFrequency;
        bool decayHighFrequencyLimit;
    };

    class OpenALChorusEffect : public OpenALEffect
    {
    public:
        enum WaveformType
        {
            SIN_WAVE,
            TRIANGLE_WAVE
        };

        OpenALChorusEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        WaveformType GetWaveformType() const;
        void SetWaveformType(WaveformType waveformType);

        int GetPhase() const;
        void SetPhase(int phase);

        float GetRate() const;
        void SetRate(float rate);

        float GetDepth() const;
        void SetDepth(float depth);

        float GetFeedback() const;
        void SetFeedback(float feedback);

        float GetDelay() const;
        void SetDelay(float delay);

    protected:
        WaveformType waveformType;
        int phase;
        float rate;
        float depth;
        float feedback;
        float delay;
    };

    class OpenALDistortionEffect : public OpenALEffect
    {
    public:
        OpenALDistortionEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        float GetEdge() const;
        void SetEdge(float edge);

        float GetGain() const;
        void SetGain(float gain);

        float GetLowPassCutoff() const;
        void SetLowPassCutoff(float lowPassCutoff);

        float GetEqCenter() const;
        void SetEqCenter(float eqCenter);

        float GetEqBandwidth() const;
        void SetEqBandwidth(float eqBandwidth);

    protected:
        float edge;
        float gain;
        float lowPassCutoff;
        float eqCenter;
        float eqBandwidth;
    };

    class OpenALEchoEffect : public OpenALEffect
    {
    public:
        OpenALEchoEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        float GetDelay() const;
        void SetDelay(float delay);

        float GetLrDelay() const;
        void SetLrDelay(float lrDelay);

        float GetDamping() const;
        void SetDamping(float damping);

        float GetFeedback() const;
        void SetFeedback(float feedback);

        float GetSpread() const;
        void SetSpread(float spread);

    protected:
        float delay;
        float lrDelay;
        float damping;
        float feedback;
        float spread;
    };

    class OpenALFlangerEffect : public OpenALEffect
    {
    public:
        enum WaveformType
        {
            SIN_WAVE,
            TRIANGLE_WAVE
        };

        OpenALFlangerEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        WaveformType GetWaveformType() const;
        void SetWaveformType(WaveformType waveformType);

        int GetPhase() const;
        void SetPhase(int phase);

        float GetRate() const;
        void SetRate(float rate);

        float GetDepth() const;
        void SetDepth(float depth);

        float GetFeedback() const;
        void SetFeedback(float feedback);

        float GetDelay() const;
        void SetDelay(float delay);

    protected:
        WaveformType waveformType;
        int phase;
        float rate;
        float depth;
        float feedback;
        float delay;
    };

    class OpenALFrequencyShifterEffect : public OpenALEffect
    {
    public:
        enum Direction
        {
            DOWN_DIRECTION,
            UP_DIRECTION,
            OFF_DIRECTION
        };

        OpenALFrequencyShifterEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        float GetFrequency() const;
        void SetFrequency(float frequency);

        Direction GetLeftDirection() const;
        void SetLeftDirection(Direction leftDirection);

        Direction GetRightDirection() const;
        void SetRightDirection(Direction rightDirection);

    protected:
        float frequency;
        Direction leftDirection;
        Direction rightDirection;
    };

    class OpenALVocalMorpherEffect : public OpenALEffect
    {
    public:
        enum WaveformType
        {
            SIN_WAVE,
            TRIANGLE_WAVE,
            SAW_WAVE
        };

        OpenALVocalMorpherEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        int GetPhonemeA() const;
        void SetPhonemeA(int phonemeA);

        int GetPhonemeB() const;
        void SetPhonemeB(int phonemeB);

        int GetPhonemeACoarseTuning() const;
        void GetPhonemeACoarseTuning(int phonemeACoarseTuning);

        int GetPhonemeBCoarseTuning() const;
        void GetPhonemeBCoarseTuning(int phonemeBCoarseTuning);

        WaveformType GetWaveformType() const;
        void SetWaveformType(WaveformType waveformType);

        float GetRate() const;
        void SetRate(float rate);

    protected:
        int phonemeA;
        int phonemeB;
        int phonemeACoarseTuning;
        int phonemeBCoarseTuning;
        WaveformType waveformType;
        float rate;
    };

    class OpenALPitchShifterEffect : public OpenALEffect
    {
    public:
        OpenALPitchShifterEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        int GetCoarseTune() const;
        void SetCoarseTune(int coarseTune);

        int GetFineTune() const;
        void SetFineTune(int fineTune);

    protected:
        int coarseTune;
        int fineTune;
    };

    class OpenALRingModulatorEffect : public OpenALEffect
    {
    public:
        enum WaveformType
        {
            SIN_WAVE,
            TRIANGLE_WAVE,
            SAW_WAVE
        };

        OpenALRingModulatorEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        float GetFrequency() const;
        void SetFrequency(float frequency);

        float GetHighPassCutoff() const;
        void SetHighPassCutoff(float highPassCutoff);

        WaveformType GetWaveformType() const;
        void SetWaveformType(WaveformType waveformType);

    protected:
        float frequency;
        float highPassCutoff;
        WaveformType waveformType;
    };

    class OpenALAutoWahEffect : public OpenALEffect
    {
    public:
        OpenALAutoWahEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        float GetAttackTime() const;
        void SetAttackTime(float attackTime);

        float GetReleaseTime() const;
        void SetReleaseTime(float releaseTime);

        float GetResonance() const;
        void SetResonance(float resonance);

        float GetPeakGain() const;
        void SetPeakGain(float peakGain);

    protected:
        float attackTime;
        float releaseTime;
        float resonance;
        float peakGain;
    };

    class OpenALCompressorEffect : public OpenALEffect
    {
    public:
        OpenALCompressorEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        bool GetOn() const;
        void SetOn(bool on);

    protected:
        bool on;
    };

    class OpenALEqualizerEffect : public OpenALEffect
    {
    public:
        OpenALEqualizerEffect();

        ALenum _GetEffectType() const override;

        void Update() override;

        float GetLowGain() const;
        void SetLowGain(float lowGain);

        float GetLowCutoff() const;
        void SetLowCutoff(float lowCutoff);

        float GetMid1Gain() const;
        void SetMid1Gain(float mid1Gain);

        float GetMid1Center() const;
        void SetMid1Center(float mid1Center);

        float GetMid1Width() const;
        void SetMid1Width(float mid1Width);

        float GetMid2Gain() const;
        void SetMid2Gain(float mid2Gain);

        float GetMid2Center() const;
        void SetMid2Center(float mid2Center);

        float GetMid2Width() const;
        void SetMid2Width(float mid2Width);

        float GetHighGain() const;
        void SetHighGain(float highGain);

        float GetHighCutoff() const;
        void SetHighCutoff(float highCutoff);

    protected:
        float lowGain;
        float lowCutoff;
        float mid1Gain;
        float mid1Center;
        float mid1Width;
        float mid2Gain;
        float mid2Center;
        float mid2Width;
        float highGain;
        float highCutoff;
    };

} }
