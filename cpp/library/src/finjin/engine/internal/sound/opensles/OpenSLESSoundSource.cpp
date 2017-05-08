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
#include "OpenSLESSoundSource.hpp"
#include "OpenSLESContext.hpp"
#include "OpenSLESListener.hpp"
#include "OpenSLESSoundBuffer.hpp"
#include "OpenSLESSoundGroup.hpp"
#include "OpenSLESSoundSourceVoice.hpp"
#include "OpenSLESUtilities.hpp"

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
    PLAYBACK_RATE = 1 << 5,
    VOLUME = 1 << 6,
    POSITION = 1 << 7,
    ORIENTATION = 1 << 8,
    ALL = VOICE | CONE | AMBIENT | STATE | BUFFER | PLAYBACK_RATE | VOLUME | POSITION | ORIENTATION
};
FINJIN_ENUM_BITWISE_OPERATIONS(SoundSourceChange)

struct OpenSLESSoundSource::Impl
{
    Impl();

    void SetDefaults();

    OpenSLESContext* context;

    SoundFormat soundFormat;
    OpenSLESSoundSourceVoice* sourceVoice;

    OpenSLESSoundGroup* group;
    OpenSLESSoundBuffer* buffer;

    uint8_t priority;

    SoundSourceState requestedState;

    bool firstUpdate;
    SoundSourceChange invalidated;
    bool isAmbient;
    bool isLooping;
    SimpleTimeCounter timeOffset;
    MathVector3 position;
    MathVector3 forward, up;
    float playbackRate;
    float volume;

    Utf8String debugName;
};


//Implementation----------------------------------------------------------------

//OpenSLESSoundSource::Impl
OpenSLESSoundSource::Impl::Impl()
{
    this->context = nullptr;

    this->sourceVoice = nullptr;

    this->group = nullptr;
    this->buffer = nullptr;

    SetDefaults();
}

void OpenSLESSoundSource::Impl::SetDefaults()
{
    this->priority = SoundConstants::DEFAULT_SOURCE_PRIORITY;
    this->requestedState = SoundSourceState::NONE;
    this->firstUpdate = true;
    this->invalidated = SoundSourceChange::ALL;
    this->isAmbient = true;
    this->isLooping = false;
    this->timeOffset = 0;
    this->playbackRate = 1.0f;
    this->volume = 1.0f;
    this->position = MathVector3::Zero();
    this->forward = MathVector3::Zero();
    this->up = MathVector3::Zero();

    this->debugName.clear();
}

//OpenSLESSoundSource
OpenSLESSoundSource::OpenSLESSoundSource() : impl(new Impl)
{
    this->soundBufferNext = nullptr;
    this->soundGroupNext = nullptr;
}

OpenSLESSoundSource::~OpenSLESSoundSource()
{
    delete impl;
}

void OpenSLESSoundSource::Create(OpenSLESContext* context)
{
    impl->context = context;
}

void OpenSLESSoundSource::SetDefaults()
{
    impl->SetDefaults();
}

void OpenSLESSoundSource::Update(SimpleTimeDelta elapsedTime)
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

void OpenSLESSoundSource::Commit(bool force)
{
    if (impl->sourceVoice == nullptr)
        return;

    auto listener = impl->context->GetListener();

    if (force ||
        impl->invalidated != SoundSourceChange::NONE ||
        !impl->isAmbient)
    {
        //std::cout << "Commit '" << impl->debugName << "': it is invalidated" << std::endl;

        if (impl->isAmbient)
        {
            //2D
            if (force || AnySet(impl->invalidated & SoundSourceChange::AMBIENT))
                OpenSLESUtilities::EnableStereoPanning(impl->sourceVoice->volumeInterface, false);

            if (force || AnySet(impl->invalidated & (SoundSourceChange::VOLUME | SoundSourceChange::AMBIENT)))
                OpenSLESUtilities::SetVolume(impl->sourceVoice->volumeInterface, impl->volume);
            if (force || AnySet(impl->invalidated & (SoundSourceChange::PLAYBACK_RATE | SoundSourceChange::AMBIENT)))
                OpenSLESUtilities::SetPlaybackRate(impl->sourceVoice->playbackRateInterface, impl->playbackRate);
        }
        else
        {
            //Recalculate 3D if there is a volume interface, which is needed to fake 3D sound with OpenSLES on Android
            if (impl->sourceVoice->volumeInterface != nullptr)
            {
                if (force || AnySet(impl->invalidated & SoundSourceChange::AMBIENT))
                    OpenSLESUtilities::EnableStereoPanning(impl->sourceVoice->volumeInterface, true);

                //This is an extremely crude model that treats the listener and sound source as omnidirectional

                //Get sound souce position relative to listener
                MathVector4 soundSourceLocalPosition4(impl->position(0), impl->position(1), impl->position(2), 1.0f);
                soundSourceLocalPosition4 = listener->worldToListener * soundSourceLocalPosition4;
                MathVector3 soundSourceLocalPosition(soundSourceLocalPosition4(0), soundSourceLocalPosition4(1), soundSourceLocalPosition4(2));
                auto soundSourceDistanceSquared = soundSourceLocalPosition.squaredNorm();

                //Calculate volume
                auto minSoundSourceDistanceSquared = impl->context->GetReferenceDistance();
                minSoundSourceDistanceSquared *= minSoundSourceDistanceSquared;

                auto maxSoundSourceDistanceSquared = impl->context->GetMaxDistance();
                maxSoundSourceDistanceSquared *= maxSoundSourceDistanceSquared;

                float volumeLevel;
                if (soundSourceDistanceSquared == maxSoundSourceDistanceSquared)
                    volumeLevel = 1.0f;
                else if (soundSourceDistanceSquared >= maxSoundSourceDistanceSquared)
                    volumeLevel = 0.0f;
                else if (soundSourceDistanceSquared <= minSoundSourceDistanceSquared)
                    volumeLevel = 1.0f;
                else
                {
                    //Clamping is always done by the above logic
                    switch (impl->context->GetDistanceModel())
                    {
                        case SoundDistanceModel::INVERSE_DISTANCE_CLAMPED:
                        {
                            auto denom = 1 + (soundSourceDistanceSquared - minSoundSourceDistanceSquared);
                            volumeLevel = 1.0f / denom;
                            break;
                        }
                        case SoundDistanceModel::LINEAR_DISTANCE_CLAMPED:
                        {
                            volumeLevel = 1.0f - (soundSourceDistanceSquared - minSoundSourceDistanceSquared) / (maxSoundSourceDistanceSquared - minSoundSourceDistanceSquared);
                            break;
                        }
                        case SoundDistanceModel::EXPONENT_DISTANCE_CLAMPED:
                        {
                            auto denom = std::exp(soundSourceDistanceSquared - minSoundSourceDistanceSquared);
                            volumeLevel = 1.0f / denom;
                            break;
                        }
                    }
                }
                OpenSLESUtilities::SetVolume(impl->sourceVoice->volumeInterface, volumeLevel);

                //Calculate panning
                MathVector3 vectorToSoundSource(soundSourceLocalPosition[0], 0, soundSourceLocalPosition[2]);
                static const MathVector3 xAxis(1, 0, 0);
                auto cosAngle = vectorToSoundSource.dot(xAxis) / vectorToSoundSource.norm();
                auto panning = (1.0f + cosAngle) * 0.5f;
                OpenSLESUtilities::SetPanning(impl->sourceVoice->volumeInterface, panning);
            }
        }

        if (force || AnySet(impl->invalidated & (SoundSourceChange::VOICE | SoundSourceChange::BUFFER)))
        {
            EnqueueBufferAndPlay();
        }

        impl->invalidated = SoundSourceChange::NONE;
    }
}

const Utf8String& OpenSLESSoundSource::GetDebugName() const
{
    return impl->debugName;
}

void OpenSLESSoundSource::SetDebugName(const Utf8String& value)
{
    impl->debugName = value;
}

int OpenSLESSoundSource::GetBufferCreateTimeStamp() const
{
    if (impl->buffer != nullptr)
        return impl->buffer->GetCreateTimeStamp();
    else
        return 0;
}

OpenSLESSoundGroup* OpenSLESSoundSource::GetGroup()
{
    return impl->group;
}

void OpenSLESSoundSource::SetGroup(OpenSLESSoundGroup* group)
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

OpenSLESSoundBuffer* OpenSLESSoundSource::GetBuffer()
{
    return impl->buffer;
}

bool OpenSLESSoundSource::SetBuffer(OpenSLESSoundBuffer* soundBuffer)
{
    if (soundBuffer != impl->buffer && (soundBuffer == nullptr || soundBuffer->GetSoundFormat() == impl->soundFormat))
    {
        Stop();

        if (impl->buffer != nullptr)
            impl->buffer->RemoveSource(this);

        impl->buffer = soundBuffer;
        impl->timeOffset = 0;

        if (impl->buffer != nullptr)
            impl->buffer->AddSource(this);

        impl->invalidated |= SoundSourceChange::BUFFER;

        return true;
    }

    return false;
}

const SoundFormat& OpenSLESSoundSource::GetSoundFormat() const
{
    return impl->soundFormat;
}

void OpenSLESSoundSource::SetSoundFormat(const SoundFormat& format)
{
    impl->soundFormat = format;
}

SoundSourceState OpenSLESSoundSource::GetState() const
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

void OpenSLESSoundSource::Play()
{
    if (impl->buffer != nullptr)
    {
        if (impl->requestedState == SoundSourceState::PLAYING)
            Rewind();

        EnqueueBufferAndPlay();

        impl->requestedState = SoundSourceState::PLAYING;
    }
}

void OpenSLESSoundSource::Pause()
{
    if (impl->requestedState == SoundSourceState::PLAYING)
    {
        impl->requestedState = SoundSourceState::PAUSED;

        if (impl->sourceVoice != nullptr)
            impl->sourceVoice->Stop();
    }
}

void OpenSLESSoundSource::Stop()
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

void OpenSLESSoundSource::Rewind()
{
    if (impl->buffer != nullptr)
    {
        if (impl->sourceVoice != nullptr)
            impl->sourceVoice->Stop();

        impl->timeOffset = 0;
        impl->firstUpdate = true;
    }
}

SimpleTimeCounter OpenSLESSoundSource::GetTimeOffset() const
{
    return impl->timeOffset;
}

void OpenSLESSoundSource::SetTimeOffset(SimpleTimeCounter offset, bool adjustPlayingSource)
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

SimpleTimeCounter OpenSLESSoundSource::GetTimeLength() const
{
    SimpleTimeCounter timeLength = 0;
    if (impl->buffer != nullptr)
        timeLength = impl->buffer->GetTimeLength();
    return timeLength;
}

bool OpenSLESSoundSource::IsLooping() const
{
    return impl->isLooping;
}

void OpenSLESSoundSource::SetLooping(bool loop)
{
    impl->isLooping = loop;
}

float OpenSLESSoundSource::GetVolume() const
{
    return impl->volume;
}

void OpenSLESSoundSource::SetVolume(float volume)
{
    Limit(volume, 0.0f, 1.0f);
    if (impl->volume != volume)
    {
        impl->volume = volume;
        if (impl->sourceVoice != nullptr && impl->isAmbient)
            OpenSLESUtilities::SetVolume(impl->sourceVoice->volumeInterface, impl->volume);
        else
            impl->invalidated |= SoundSourceChange::VOLUME;
    }
}

bool OpenSLESSoundSource::IsAmbient() const
{
    return impl->isAmbient;
}

void OpenSLESSoundSource::SetAmbient(bool ambient)
{
    if (impl->isAmbient != ambient)
    {
        impl->isAmbient = ambient;

        impl->invalidated |= SoundSourceChange::AMBIENT;
    }
}

void OpenSLESSoundSource::MakeOmniDirectional()
{
}

void OpenSLESSoundSource::MakeDirectional()
{
}

Angle OpenSLESSoundSource::GetConeInnerAngle() const
{
    return Angle(0);
}

void OpenSLESSoundSource::SetConeInnerAngle(Angle innerAngle)
{
    impl->invalidated |= SoundSourceChange::CONE;
}

Angle OpenSLESSoundSource::GetConeOuterAngle() const
{
    return Angle(0);
}

void OpenSLESSoundSource::SetConeOuterAngle(Angle outerAngle)
{
    impl->invalidated |= SoundSourceChange::CONE;
}

float OpenSLESSoundSource::GetConeOuterVolume() const
{
    return 0;
}

void OpenSLESSoundSource::SetConeOuterVolume(float outerVolume)
{
    impl->invalidated |= SoundSourceChange::CONE;
}

float OpenSLESSoundSource::GetConeMinVolume() const
{
    return 0;
}

void OpenSLESSoundSource::SetConeMinVolume(float minVolume)
{
    impl->invalidated |= SoundSourceChange::CONE;
}

float OpenSLESSoundSource::GetConeMaxVolume() const
{
    return 0;
}

void OpenSLESSoundSource::SetConeMaxVolume(float maxVolume)
{
    impl->invalidated |= SoundSourceChange::CONE;
}

float OpenSLESSoundSource::GetPlaybackRate() const
{
    return impl->playbackRate;
}

void OpenSLESSoundSource::SetPlaybackRate(float playbackRate)
{
    Limit(playbackRate, 0.5f, 2.0f);
    if (impl->playbackRate != playbackRate)
    {
        impl->playbackRate = playbackRate;
        if (impl->sourceVoice != nullptr && impl->isAmbient)
            OpenSLESUtilities::SetPlaybackRate(impl->sourceVoice->playbackRateInterface, impl->playbackRate);
        else
            impl->invalidated |= SoundSourceChange::PLAYBACK_RATE;
    }
}

OpenSLESSoundSourceVoice* OpenSLESSoundSource::GetVoice()
{
    return impl->sourceVoice;
}

OpenSLESSoundSourceVoice* OpenSLESSoundSource::DetachVoice()
{
    if (impl->sourceVoice != nullptr)
        impl->invalidated |= SoundSourceChange::VOICE;

    auto result = impl->sourceVoice;
    impl->sourceVoice = nullptr;
    return result;
}

void OpenSLESSoundSource::SetVoice(OpenSLESSoundSourceVoice* voice)
{
    if (impl->sourceVoice != voice)
    {
        impl->sourceVoice = voice;

        impl->invalidated |= SoundSourceChange::VOICE;
    }
}

void OpenSLESSoundSource::EnqueueBufferAndPlay()
{
    if (impl->sourceVoice != nullptr && impl->buffer != nullptr)
    {
        impl->sourceVoice->Stop();

        size_t offset = impl->soundFormat.GetSampleByteOffset(impl->timeOffset);
        (*impl->sourceVoice->bufferQueueInterface)->Enqueue(impl->sourceVoice->bufferQueueInterface, impl->buffer->data + offset, impl->buffer->dataLength - offset);
        (*impl->sourceVoice->playerPlayInterface)->SetPlayState(impl->sourceVoice->playerPlayInterface, SL_PLAYSTATE_PLAYING);
    }
}

void OpenSLESSoundSource::GetOrientation(MathVector3& forward, MathVector3& up) const
{
    forward = impl->forward;
    up = impl->up;
}

void OpenSLESSoundSource::SetOrientation(const MathVector3& forward, const MathVector3& up)
{
    impl->forward = forward;
    impl->up = up;

    impl->invalidated |= SoundSourceChange::ORIENTATION;
}

void OpenSLESSoundSource::GetPosition(MathVector3& value) const
{
    value = impl->position;
}

void OpenSLESSoundSource::SetPosition(const MathVector3& value)
{
    if (value != impl->position)
    {
        impl->position = value;

        impl->invalidated |= SoundSourceChange::POSITION;
    }
}

void OpenSLESSoundSource::GetVelocity(MathVector3& value) const
{
    value = MathVector3::Zero();
}

void OpenSLESSoundSource::SetVelocity(const MathVector3& value)
{
}

uint8_t OpenSLESSoundSource::GetPriority() const
{
    return impl->priority;
}

void OpenSLESSoundSource::SetPriority(uint8_t priority)
{
    impl->priority = priority;
}
