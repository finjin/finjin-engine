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
#include "XAudio2SoundSource.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/StaticVector.hpp"
#include "XAudio2Context.hpp"
#include "XAudio2ContextImpl.hpp"
#include "XAudio2Includes.hpp"
#include "XAudio2Listener.hpp"
#include "XAudio2ListenerImpl.hpp"
#include "XAudio2SoundBuffer.hpp"
#include "XAudio2SoundGroup.hpp"
#include "XAudio2SoundSourceVoice.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
enum class SoundSourceChange
{
    NONE = 0,
    VOICE = 1 << 0,
    CONE = 1 << 1,
    AMBIENT = 1 << 2,
    STATE = 1 << 3,
    BUFFER = 1 << 4,
    PLAYBACK_RATE = 1 << 7,
    VOLUME = 1 << 8,
    POSITION = 1 << 9,
    VELOCITY = 1 << 10,
    ORIENTATION = 1 << 11,
    ALL = 0xffff
};
FINJIN_ENUM_BITWISE_OPERATIONS(SoundSourceChange)

struct XAudio2SoundSource::Impl
{
    Impl();

    void SetDefaults();

    void SetDefaultOutputCoefs()
    {
        for (auto& item : this->x3dOutputChannelCoefs)
            item = 1.0f;
    }

    XAudio2Context* context;

    SoundFormat soundFormat;
    XAudio2SoundSourceVoice* sourceVoice;

    X3DAUDIO_EMITTER x3dEmitter;
    X3DAUDIO_CONE x3dCone;
    StaticVector<float, SoundFormat::MAX_CHANNELS * SoundFormat::MAX_CHANNELS> x3dOutputChannelCoefs; //src channels * dest channels
    X3DAUDIO_DSP_SETTINGS dspSettings;

    XAudio2SoundGroup* group;
    XAudio2SoundBuffer* buffer;

    uint8_t priority;

    SoundSourceState requestedState;

    bool firstUpdate;
    SoundSourceChange invalidated;
    bool isAmbient;
    bool isLooping;
    SimpleTimeCounter timeOffset;
    float playbackRate;
    float volume;

    Utf8String debugName;
};


//Implementation----------------------------------------------------------------

//XAudio2SoundSource::Impl
XAudio2SoundSource::Impl::Impl()
{
    this->context = nullptr;

    this->sourceVoice = nullptr;

    this->group = nullptr;
    this->buffer = nullptr;

    SetDefaults();
}

void XAudio2SoundSource::Impl::SetDefaults()
{
    FINJIN_ZERO_ITEM(this->x3dEmitter);
    FINJIN_ZERO_ITEM(this->x3dCone);
    FINJIN_ZERO_ITEM(this->dspSettings);
    SetDefaultOutputCoefs();
    this->x3dCone.InnerVolume = 1.0f;

    this->priority = SoundConstants::DEFAULT_SOURCE_PRIORITY;
    this->requestedState = SoundSourceState::NONE;
    this->firstUpdate = true;
    this->invalidated = SoundSourceChange::ALL;
    this->isAmbient = true;
    this->isLooping = false;
    this->timeOffset = 0;

    this->playbackRate = 1.0f;
    this->volume = 1.0f;

    this->debugName.clear();
}

//XAudio2SoundSource
XAudio2SoundSource::XAudio2SoundSource() : impl(new Impl)
{
    this->soundBufferNext = nullptr;
    this->soundGroupNext = nullptr;
}

XAudio2SoundSource::~XAudio2SoundSource()
{
    delete impl;
}

void XAudio2SoundSource::Create(XAudio2Context* context)
{
    impl->context = context;
}

void XAudio2SoundSource::SetDefaults()
{
    impl->SetDefaults();
}

void XAudio2SoundSource::Update(SimpleTimeDelta elapsedTime)
{
    //Update time offset if sound is playing
    if (AnySet(impl->requestedState & SoundSourceState::PLAYING))
    {
        //std::cout << "Update '" << impl->debugName << "': it is playing, elapsed time: " << elapsedTime << ", out of total: " << GetTimeLength() << std::endl;

        auto newTimeOffset = impl->timeOffset;

        //The first update should not include the elapsed time in its calculation
        if (!impl->firstUpdate)
            newTimeOffset += elapsedTime;
        impl->firstUpdate = false;

        SetTimeOffset(newTimeOffset, false);
    }
}

void XAudio2SoundSource::Commit(bool force)
{
    if (impl->sourceVoice == nullptr)
        return;

    if (force || impl->invalidated != SoundSourceChange::NONE || !impl->isAmbient)
    {
        //std::cout << "Commit '" << impl->debugName << "': it is invalidated" << std::endl;

        auto contextImpl = impl->context->GetImpl();

        if (impl->isAmbient)
        {
            //2D
            if (force || AnySet(impl->invalidated & SoundSourceChange::AMBIENT))
            {
                //Disable 3D
                impl->SetDefaultOutputCoefs();
                impl->sourceVoice->xaudioSourceVoice->SetOutputMatrix(contextImpl->masteringVoice, 1, static_cast<UINT32>(impl->x3dOutputChannelCoefs.size()), &impl->x3dOutputChannelCoefs[0]);

                //impl->sourceVoice->xaudioSourceVoice->SetOutputMatrix(pSubmixVoice, 1, 1, &nullReverb);

                XAUDIO2_FILTER_PARAMETERS filterParameters = { LowPassFilter , 1.0f, 1.0f };
                impl->sourceVoice->xaudioSourceVoice->SetFilterParameters(&filterParameters);
            }

            if (force || AnySet(impl->invalidated & (SoundSourceChange::VOLUME | SoundSourceChange::AMBIENT)))
                impl->sourceVoice->xaudioSourceVoice->SetVolume(impl->volume, XAUDIO2_COMMIT_NOW);
            if (force || AnySet(impl->invalidated & (SoundSourceChange::PLAYBACK_RATE | SoundSourceChange::AMBIENT)))
                impl->sourceVoice->xaudioSourceVoice->SetFrequencyRatio(impl->playbackRate, XAUDIO2_COMMIT_NOW);
        }
        else
        {
            //Recalculate 3D
            impl->x3dEmitter.CurveDistanceScaler = contextImpl->referenceDistance;
            impl->x3dEmitter.DopplerScaler = contextImpl->dopplerFactor;

            X3DAudioCalculate
            (
                static_cast<BYTE*>(contextImpl->x3dAudioInterface),
                &impl->context->GetListener()->GetImpl()->x3dListener,
                &impl->x3dEmitter,
                X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_REVERB,
                &impl->dspSettings
            );

            impl->sourceVoice->xaudioSourceVoice->SetOutputMatrix(contextImpl->masteringVoice, 1, static_cast<UINT32>(impl->x3dOutputChannelCoefs.size()), &impl->x3dOutputChannelCoefs[0]);
            impl->sourceVoice->xaudioSourceVoice->SetFrequencyRatio(impl->dspSettings.DopplerFactor); //impl->context->GetDopplerFactor()

                                                                                                      //impl->sourceVoice->SetOutputMatrix(pSubmixVoice, 1, 1, &impl->dspSettings.ReverbLevel);

            XAUDIO2_FILTER_PARAMETERS filterParameters = { LowPassFilter, 2.0f * sinf(X3DAUDIO_PI / 6.0f * impl->dspSettings.LPFDirectCoefficient), 1.0f };
            impl->sourceVoice->xaudioSourceVoice->SetFilterParameters(&filterParameters);
        }

        if (force || AnySet(impl->invalidated & (SoundSourceChange::VOICE | SoundSourceChange::BUFFER)))
        {
            EnqueueBufferAndPlay();
        }

        impl->invalidated = SoundSourceChange::NONE;
    }
}

const Utf8String& XAudio2SoundSource::GetDebugName() const
{
    return impl->debugName;
}

void XAudio2SoundSource::SetDebugName(const Utf8String& value)
{
    impl->debugName = value;
}

int XAudio2SoundSource::GetBufferCreateTimeStamp() const
{
    if (impl->buffer != nullptr)
        return impl->buffer->GetCreateTimeStamp();
    else
        return 0;
}

XAudio2SoundGroup* XAudio2SoundSource::GetGroup()
{
    return impl->group;
}

void XAudio2SoundSource::SetGroup(XAudio2SoundGroup* group)
{
    if (group == nullptr)
        group = impl->context->GetMasterGroup();

    if (group != impl->group)
    {
        if (impl->group != nullptr)
            impl->group->RemoveSource(this);

        impl->group = group;

        if (impl->group != nullptr)
            impl->group->AddSource(this);
    }
}

XAudio2SoundBuffer* XAudio2SoundSource::GetBuffer()
{
    return impl->buffer;
}

bool XAudio2SoundSource::SetBuffer(XAudio2SoundBuffer* soundBuffer)
{
    if (soundBuffer != impl->buffer && (soundBuffer == nullptr || soundBuffer->GetSoundFormat() == impl->soundFormat))
    {
        Stop();

        if (impl->buffer != nullptr)
            impl->buffer->RemoveSource(this);

        impl->buffer = soundBuffer;
        impl->timeOffset = 0;

        if (impl->buffer != nullptr)
        {
            impl->buffer->AddSource(this);

            impl->x3dEmitter.ChannelCount = impl->buffer->GetSoundFormat().channelCount;
        }

        impl->invalidated |= SoundSourceChange::BUFFER;

        return true;
    }

    return false;
}

const SoundFormat& XAudio2SoundSource::GetSoundFormat() const
{
    return impl->soundFormat;
}

void XAudio2SoundSource::SetSoundFormat(const SoundFormat& format)
{
    impl->soundFormat = format;

    auto contextImpl = impl->context->GetImpl();

    impl->dspSettings.SrcChannelCount = static_cast<UINT32>(impl->soundFormat.channelCount);
    impl->dspSettings.DstChannelCount = static_cast<UINT32>(contextImpl->masteringVoiceChannelCount);

    auto coefCount = impl->dspSettings.SrcChannelCount * impl->dspSettings.DstChannelCount;
    assert(coefCount > 0);
    assert(coefCount <= impl->x3dOutputChannelCoefs.max_size());
    impl->x3dOutputChannelCoefs.resize(coefCount);
    impl->SetDefaultOutputCoefs();
    impl->dspSettings.pMatrixCoefficients = &impl->x3dOutputChannelCoefs[0];
}

SoundSourceState XAudio2SoundSource::GetState() const
{
    SoundSourceState state = SoundSourceState::NONE;

    if (impl->buffer != nullptr)
    {
        state = impl->requestedState;

        //There is a sound buffer
        if (impl->sourceVoice == nullptr)
        {
            //No sound sources, it's virtualized
            state |= SoundSourceState::VIRTUALIZED;
        }

        if (impl->requestedState == SoundSourceState::PLAYING || impl->requestedState == SoundSourceState::PAUSED)
        {
            //Sound is in a non-stopped state
            auto timeLength = GetTimeLength();
            if (timeLength == 0 || (impl->timeOffset >= timeLength && !impl->isLooping))
            {
                //Sound should be stopped
                state |= SoundSourceState::STOPPED;
            }
        }
    }

    if (impl->isLooping)
        state |= SoundSourceState::LOOPING;

    return state;
}

void XAudio2SoundSource::Play()
{
    if (impl->buffer != nullptr)
    {
        if (impl->requestedState == SoundSourceState::PLAYING)
            Rewind();

        EnqueueBufferAndPlay();

        impl->requestedState = SoundSourceState::PLAYING;
    }
}

void XAudio2SoundSource::Pause()
{
    if (impl->requestedState == SoundSourceState::PLAYING)
    {
        impl->requestedState = SoundSourceState::PAUSED;

        if (impl->sourceVoice != nullptr)
            impl->sourceVoice->Stop();
    }
}

void XAudio2SoundSource::Stop()
{
    if (impl->requestedState == SoundSourceState::PLAYING || impl->requestedState == SoundSourceState::PAUSED)
    {
        if (impl->sourceVoice != nullptr)
            impl->sourceVoice->Stop();

        impl->requestedState = SoundSourceState::STOPPED;

        impl->timeOffset = 0;
        impl->firstUpdate = true;
    }
}

void XAudio2SoundSource::Rewind()
{
    if (impl->buffer != nullptr)
    {
        if (impl->sourceVoice != nullptr)
            impl->sourceVoice->Stop();

        impl->timeOffset = 0;
        impl->firstUpdate = true;
    }
}

SimpleTimeCounter XAudio2SoundSource::GetTimeOffset() const
{
    return impl->timeOffset;
}

void XAudio2SoundSource::SetTimeOffset(SimpleTimeCounter offset, bool adjustPlayingSource)
{
    //Only sounds with regular buffers can have their time offset adjusted
    if (impl->buffer == nullptr)
        return;

    //Ensure offset is non-negative
    if (offset < 0)
        offset = 0;

    //Get sound's time length. A zero length indicates there is no sound to play
    auto timeLength = GetTimeLength();
    if (timeLength > 0)
    {
        //Sound is valid, so use the offset
        impl->timeOffset = offset;

        if (impl->isLooping)
        {
            //The sound is looping. Ensure the offset is within range
            while (impl->timeOffset > timeLength)
                impl->timeOffset -= timeLength;
        }
        else if (impl->timeOffset > timeLength)
        {
            //The sound is done playing
            adjustPlayingSource = false;
            Stop();
        }
    }
    else
    {
        //There is no sound to be played
        adjustPlayingSource = false;
        impl->timeOffset = 0;
    }

    if (adjustPlayingSource)
    {
        //Ensure that the next update is considered the first so that we can avoid adjusting the cursor again before the sound is heard
        impl->firstUpdate = true;

        EnqueueBufferAndPlay();
    }
}

SimpleTimeCounter XAudio2SoundSource::GetTimeLength() const
{
    SimpleTimeCounter timeLength = 0;
    if (impl->buffer != nullptr)
        timeLength = impl->buffer->GetTimeLength();
    return timeLength;
}

bool XAudio2SoundSource::IsLooping() const
{
    return impl->isLooping;
}

void XAudio2SoundSource::SetLooping(bool loop)
{
    impl->isLooping = loop;
}

float XAudio2SoundSource::GetVolume() const
{
    return impl->volume;
}

void XAudio2SoundSource::SetVolume(float volume)
{
    Limit(volume, 0.0f, 1.0f);
    if (impl->volume != volume)
    {
        impl->volume = volume;
        if (impl->sourceVoice != nullptr && impl->isAmbient)
            impl->sourceVoice->xaudioSourceVoice->SetVolume(volume, XAUDIO2_COMMIT_NOW);
        else
            impl->invalidated |= SoundSourceChange::VOLUME;
    }
}

bool XAudio2SoundSource::IsAmbient() const
{
    return impl->isAmbient;
}

void XAudio2SoundSource::SetAmbient(bool ambient)
{
    if (impl->isAmbient != ambient)
    {
        impl->isAmbient = ambient;

        impl->invalidated |= SoundSourceChange::AMBIENT;
    }
}

void XAudio2SoundSource::MakeOmniDirectional()
{
    if (impl->x3dEmitter.pCone != nullptr)
    {
        impl->x3dEmitter.pCone = nullptr;

        impl->invalidated |= SoundSourceChange::CONE;
    }
}

void XAudio2SoundSource::MakeDirectional()
{
    if (impl->x3dEmitter.pCone == nullptr)
    {
        impl->x3dEmitter.pCone = &impl->x3dCone;

        impl->invalidated |= SoundSourceChange::CONE;
    }
}

Angle XAudio2SoundSource::GetConeInnerAngle() const
{
    return Radians(impl->x3dCone.InnerAngle);
}

void XAudio2SoundSource::SetConeInnerAngle(Angle innerAngle)
{
    impl->x3dCone.InnerAngle = innerAngle.ToRadiansValue();

    impl->invalidated |= SoundSourceChange::CONE;
}

Angle XAudio2SoundSource::GetConeOuterAngle() const
{
    return Radians(impl->x3dCone.OuterAngle);
}

void XAudio2SoundSource::SetConeOuterAngle(Angle outerAngle)
{
    impl->x3dCone.OuterAngle = outerAngle.ToRadiansValue();

    impl->invalidated |= SoundSourceChange::CONE;
}

float XAudio2SoundSource::GetConeOuterVolume() const
{
    return impl->x3dCone.OuterVolume;
}

void XAudio2SoundSource::SetConeOuterVolume(float outerVolume)
{
    impl->x3dCone.OuterVolume = outerVolume;

    impl->invalidated |= SoundSourceChange::CONE;
}

float XAudio2SoundSource::GetConeMinVolume() const
{
    return impl->x3dCone.InnerLPF;
}

void XAudio2SoundSource::SetConeMinVolume(float minVolume)
{
    impl->x3dCone.InnerLPF = minVolume;

    impl->invalidated |= SoundSourceChange::CONE;
}

float XAudio2SoundSource::GetConeMaxVolume() const
{
    return impl->x3dCone.OuterLPF;
}

void XAudio2SoundSource::SetConeMaxVolume(float maxVolume)
{
    impl->x3dCone.OuterLPF = maxVolume;

    impl->invalidated |= SoundSourceChange::CONE;
}

float XAudio2SoundSource::GetPlaybackRate() const
{
    return impl->playbackRate;
}

void XAudio2SoundSource::SetPlaybackRate(float playbackRate)
{
    Limit(playbackRate, XAUDIO2_MIN_FREQ_RATIO, XAUDIO2_MAX_FREQ_RATIO);
    if (impl->playbackRate != playbackRate)
    {
        impl->playbackRate = playbackRate;
        if (impl->sourceVoice != nullptr && impl->isAmbient)
            impl->sourceVoice->xaudioSourceVoice->SetFrequencyRatio(playbackRate, XAUDIO2_COMMIT_NOW);
        else
            impl->invalidated |= SoundSourceChange::PLAYBACK_RATE;
    }
}

XAudio2SoundSourceVoice* XAudio2SoundSource::GetVoice()
{
    return impl->sourceVoice;
}

XAudio2SoundSourceVoice* XAudio2SoundSource::DetachVoice()
{
    if (impl->sourceVoice != nullptr)
        impl->invalidated |= SoundSourceChange::VOICE;

    auto result = impl->sourceVoice;
    impl->sourceVoice = nullptr;
    return result;
}

void XAudio2SoundSource::SetVoice(XAudio2SoundSourceVoice* voice)
{
    if (impl->sourceVoice != voice)
    {
        impl->sourceVoice = voice;

        impl->invalidated |= SoundSourceChange::VOICE;
    }
}

void XAudio2SoundSource::EnqueueBufferAndPlay()
{
    if (impl->sourceVoice != nullptr && impl->buffer != nullptr)
    {
        impl->sourceVoice->Stop();

        XAUDIO2_BUFFER xbuffer;
        FINJIN_ZERO_ITEM(xbuffer);
        xbuffer.pAudioData = impl->buffer->GetDataBytes();
        xbuffer.AudioBytes = static_cast<UINT32>(impl->buffer->GetDataByteCount());
        xbuffer.LoopCount = impl->isLooping ? XAUDIO2_LOOP_INFINITE : 0;
        xbuffer.Flags = XAUDIO2_END_OF_STREAM;
        xbuffer.PlayBegin = static_cast<UINT32>(impl->soundFormat.GetSampleByteOffset(impl->timeOffset));
        impl->sourceVoice->xaudioSourceVoice->SubmitSourceBuffer(&xbuffer);

        UINT32 flags = 0;
        impl->sourceVoice->xaudioSourceVoice->Start(flags, XAUDIO2_COMMIT_NOW);
    }
}

void XAudio2SoundSource::GetOrientation(MathVector3& forward, MathVector3& up) const
{
    forward = MathVector3(impl->x3dEmitter.OrientFront.x, impl->x3dEmitter.OrientFront.y, impl->x3dEmitter.OrientFront.z);
    up = MathVector3(impl->x3dEmitter.OrientTop.x, impl->x3dEmitter.OrientTop.y, impl->x3dEmitter.OrientTop.z);
}

void XAudio2SoundSource::SetOrientation(const MathVector3& forward, const MathVector3& up)
{
    impl->x3dEmitter.OrientFront.x = forward(0);
    impl->x3dEmitter.OrientFront.y = forward(1);
    impl->x3dEmitter.OrientFront.z = forward(2);
    impl->x3dEmitter.OrientTop.x = up(0);
    impl->x3dEmitter.OrientTop.y = up(1);
    impl->x3dEmitter.OrientTop.z = up(2);

    impl->invalidated |= SoundSourceChange::ORIENTATION;
}

void XAudio2SoundSource::GetPosition(MathVector3& value) const
{
    value = MathVector3(impl->x3dEmitter.Position.x, impl->x3dEmitter.Position.y, impl->x3dEmitter.Position.z);
}

void XAudio2SoundSource::SetPosition(const MathVector3& value)
{
    if (value(0) != impl->x3dEmitter.Position.x || value(1) != impl->x3dEmitter.Position.y || value(2) != impl->x3dEmitter.Position.z)
    {
        impl->x3dEmitter.Position.x = value(0);
        impl->x3dEmitter.Position.y = value(1);
        impl->x3dEmitter.Position.z = value(2);

        impl->invalidated |= SoundSourceChange::POSITION;
    }
}

void XAudio2SoundSource::GetVelocity(MathVector3& value) const
{
    value = MathVector3(impl->x3dEmitter.Velocity.x, impl->x3dEmitter.Velocity.y, impl->x3dEmitter.Velocity.z);
}

void XAudio2SoundSource::SetVelocity(const MathVector3& value)
{
    if (value(0) != impl->x3dEmitter.Velocity.x || value(1) != impl->x3dEmitter.Velocity.y || value(2) != impl->x3dEmitter.Velocity.z)
    {
        impl->x3dEmitter.Velocity.x = value(0);
        impl->x3dEmitter.Velocity.y = value(1);
        impl->x3dEmitter.Velocity.z = value(2);

        impl->invalidated |= SoundSourceChange::VELOCITY;
    }
}

uint8_t XAudio2SoundSource::GetPriority() const
{
    return impl->priority;
}

void XAudio2SoundSource::SetPriority(uint8_t priority)
{
    impl->priority = priority;
}
