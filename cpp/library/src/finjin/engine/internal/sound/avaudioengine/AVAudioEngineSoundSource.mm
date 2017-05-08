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
#include "AVAudioEngineSoundSource.hpp"
#include "finjin/common/DebugLog.hpp"
#include "AVAudioEngineSoundBuffer.hpp"
#include "AVAudioEngineSoundBufferImpl.hpp"
#include "AVAudioEngineSoundContextImpl.hpp"
#include "AVAudioEngineSoundGroup.hpp"
#include "AVAudioEngineSoundListener.hpp"
#include "AVAudioEngineSoundSourceImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
AVAudioEngineSoundSource::AVAudioEngineSoundSource() : impl(new AVAudioEngineSoundSourceImpl)
{
    this->soundBufferNext = nullptr;
    this->soundGroupNext = nullptr;
}

AVAudioEngineSoundSource::~AVAudioEngineSoundSource()
{
    delete impl;
}

void AVAudioEngineSoundSource::Create(AVAudioEngineSoundContextImpl* context)
{
    impl->context = context;
}

void AVAudioEngineSoundSource::SetDefaults()
{
    impl->SetDefaults();
}

void AVAudioEngineSoundSource::Update(SimpleTimeDelta elapsedTime)
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

void AVAudioEngineSoundSource::Commit(bool force)
{
    if (impl->sourceVoice == nullptr)
        return;

    if (force || impl->invalidated != SoundSourceChange::NONE || !impl->isAmbient)
    {
        //std::cout << "Commit '" << impl->debugName << "': it is invalidated" << std::endl;

        if (impl->isAmbient)
        {
            //2D
            if (force || AnySet(impl->invalidated & SoundSourceChange::AMBIENT))
            {
                //Disable 3D
                impl->sourceVoice->player.renderingAlgorithm = AVAudio3DMixingRenderingAlgorithmStereoPassThrough;
            }

            if (force || AnySet(impl->invalidated & (SoundSourceChange::VOLUME | SoundSourceChange::AMBIENT)))
                impl->sourceVoice->player.volume = impl->volume;
            if (force || AnySet(impl->invalidated & (SoundSourceChange::PLAYBACK_RATE | SoundSourceChange::AMBIENT)))
                impl->sourceVoice->player.rate = impl->playbackRate;
        }
        else
        {
            //3D
            if (force || AnySet(impl->invalidated & SoundSourceChange::AMBIENT))
            {
                switch (impl->context->nsimpl->settings.speakerSetup)
                {
                    case SoundSpeakerSetup::MONO:
                    case SoundSpeakerSetup::STEREO:
                    {
                        //Simulate 3D
                        switch (impl->context->nsimpl->settings.sound3DQuality)
                        {
                            case Sound3DQuality::LOW: impl->sourceVoice->player.renderingAlgorithm = AVAudio3DMixingRenderingAlgorithmEqualPowerPanning; break;
                            case Sound3DQuality::MEDIUM: impl->sourceVoice->player.renderingAlgorithm = AVAudio3DMixingRenderingAlgorithmSphericalHead; break;
                            case Sound3DQuality::HIGH: impl->sourceVoice->player.renderingAlgorithm = AVAudio3DMixingRenderingAlgorithmHRTF; break;
                        }

                        break;
                    }
                    default:
                    {
                        //Output 3D sounds to surround speakers
                        impl->sourceVoice->player.renderingAlgorithm = AVAudio3DMixingRenderingAlgorithmSoundField;
                        break;
                    }
                }
            }

            if (force || AnySet(impl->invalidated & SoundSourceChange::POSITION | SoundSourceChange::AMBIENT))
                impl->sourceVoice->player.position = AVAudioMake3DPoint(impl->position[0], impl->position[1], impl->position[2]);
        }

        if (force || AnySet(impl->invalidated & (SoundSourceChange::VOICE | SoundSourceChange::BUFFER)))
        {
            EnqueueBufferAndPlay();
        }

        impl->invalidated = SoundSourceChange::NONE;
    }
}

const Utf8String& AVAudioEngineSoundSource::GetDebugName() const
{
    return impl->debugName;
}

void AVAudioEngineSoundSource::SetDebugName(const Utf8String& value)
{
    impl->debugName = value;
}

int AVAudioEngineSoundSource::GetBufferCreateTimeStamp() const
{
    if (impl->buffer != nullptr)
        return impl->buffer->GetCreateTimeStamp();
    else
        return 0;
}

AVAudioEngineSoundGroup* AVAudioEngineSoundSource::GetGroup()
{
    return impl->group;
}

void AVAudioEngineSoundSource::SetGroup(AVAudioEngineSoundGroup* group)
{
    if (group == nullptr)
        group = &impl->context->nsimpl->masterSoundGroup;

    if (group != impl->group)
    {
        if (impl->group != nullptr)
            impl->group->RemoveSource(this);

        impl->group = group;

        if (impl->group != nullptr)
            impl->group->AddSource(this);
    }
}

AVAudioEngineSoundBuffer* AVAudioEngineSoundSource::GetBuffer()
{
    return impl->buffer;
}

bool AVAudioEngineSoundSource::SetBuffer(AVAudioEngineSoundBuffer* soundBuffer)
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

const SoundFormat& AVAudioEngineSoundSource::GetSoundFormat() const
{
    return impl->soundFormat;
}

void AVAudioEngineSoundSource::SetSoundFormat(const SoundFormat& format)
{
    impl->soundFormat = format;
}

SoundSourceState AVAudioEngineSoundSource::GetState() const
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

void AVAudioEngineSoundSource::Play()
{
    if (impl->buffer != nullptr)
    {
        if (impl->requestedState == SoundSourceState::PLAYING)
            Rewind();

        EnqueueBufferAndPlay();

        impl->requestedState = SoundSourceState::PLAYING;
    }
}

void AVAudioEngineSoundSource::Pause()
{
    if (impl->requestedState == SoundSourceState::PLAYING)
    {
        impl->requestedState = SoundSourceState::PAUSED;

        if (impl->sourceVoice != nullptr)
            [impl->sourceVoice->player stop];
    }
}

void AVAudioEngineSoundSource::Stop()
{
    if (impl->requestedState == SoundSourceState::PLAYING || impl->requestedState == SoundSourceState::PAUSED)
    {
        if (impl->sourceVoice != nullptr)
            [impl->sourceVoice->player stop];

        impl->requestedState = SoundSourceState::STOPPED;

        impl->timeOffset = 0;
        impl->firstUpdate = true;
    }
}

void AVAudioEngineSoundSource::Rewind()
{
    if (impl->buffer != nullptr)
    {
        if (impl->sourceVoice != nullptr)
            [impl->sourceVoice->player stop];

        impl->timeOffset = 0;
        impl->firstUpdate = true;
    }
}

SimpleTimeCounter AVAudioEngineSoundSource::GetTimeOffset() const
{
    return impl->timeOffset;
}

void AVAudioEngineSoundSource::SetTimeOffset(SimpleTimeCounter offset, bool adjustPlayingSource)
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

SimpleTimeCounter AVAudioEngineSoundSource::GetTimeLength() const
{
    SimpleTimeCounter timeLength = 0;
    if (impl->buffer != nullptr)
        timeLength = impl->buffer->GetTimeLength();
    return timeLength;
}

bool AVAudioEngineSoundSource::IsLooping() const
{
    return impl->isLooping;
}

void AVAudioEngineSoundSource::SetLooping(bool loop)
{
    impl->isLooping = loop;
}

float AVAudioEngineSoundSource::GetVolume() const
{
    return impl->volume;
}

void AVAudioEngineSoundSource::SetVolume(float volume)
{
    Limit(volume, 0.0f, 1.0f);
    if (impl->volume != volume)
    {
        impl->volume = volume;

        if (impl->sourceVoice != nullptr && impl->isAmbient)
            impl->sourceVoice->player.volume = volume;
        else
            impl->invalidated |= SoundSourceChange::VOLUME;
    }
}

bool AVAudioEngineSoundSource::IsAmbient() const
{
    return impl->isAmbient;
}

void AVAudioEngineSoundSource::SetAmbient(bool ambient)
{
    if (impl->isAmbient != ambient)
    {
        impl->isAmbient = ambient;

        impl->invalidated |= SoundSourceChange::AMBIENT;
    }
}

void AVAudioEngineSoundSource::MakeOmniDirectional()
{
    //Do nothing. Source is always omnidirectional
}

void AVAudioEngineSoundSource::MakeDirectional()
{
    //Do nothing. Source is always omnidirectional
}

Angle AVAudioEngineSoundSource::GetConeInnerAngle() const
{
    return Radians(0);
}

void AVAudioEngineSoundSource::SetConeInnerAngle(Angle innerAngle)
{
}

Angle AVAudioEngineSoundSource::GetConeOuterAngle() const
{
    return Radians(0);
}

void AVAudioEngineSoundSource::SetConeOuterAngle(Angle outerAngle)
{
}

float AVAudioEngineSoundSource::GetConeOuterVolume() const
{
    return 0;
}

void AVAudioEngineSoundSource::SetConeOuterVolume(float outerVolume)
{
}

float AVAudioEngineSoundSource::GetConeMinVolume() const
{
    return 0;
}

void AVAudioEngineSoundSource::SetConeMinVolume(float minVolume)
{
}

float AVAudioEngineSoundSource::GetConeMaxVolume() const
{
    return 0;
}

void AVAudioEngineSoundSource::SetConeMaxVolume(float maxVolume)
{
}

float AVAudioEngineSoundSource::GetPlaybackRate() const
{
    return impl->playbackRate;
}

void AVAudioEngineSoundSource::SetPlaybackRate(float playbackRate)
{
    Limit(playbackRate, .5f, 2.0f);
    if (impl->playbackRate != playbackRate)
    {
        impl->playbackRate = playbackRate;

        if (impl->sourceVoice != nullptr && impl->isAmbient)
            impl->sourceVoice->player.rate = playbackRate;
        else
            impl->invalidated |= SoundSourceChange::PLAYBACK_RATE;
    }
}

void AVAudioEngineSoundSource::EnqueueBufferAndPlay()
{
    if (impl->sourceVoice != nullptr && impl->buffer != nullptr)
    {
        //Calculate frame position of the current time
        //Use a negative offset with initWithSampleTime to shift the play cursor left
        AVAudioFramePosition framePosition = impl->soundFormat.GetSampleFrameOffset(impl->timeOffset);
        auto newTime = [[AVAudioTime alloc] initWithSampleTime:-framePosition atRate:impl->soundFormat.samplesPerSecond];

        //Play
        AVAudioPlayerNodeBufferOptions avoptions = AVAudioPlayerNodeBufferInterrupts;
        if (impl->isLooping)
            avoptions |= AVAudioPlayerNodeBufferLoops;
        [impl->sourceVoice->player scheduleBuffer:impl->buffer->impl->avbuffer atTime:newTime options:avoptions completionHandler:nullptr];
        [impl->sourceVoice->player play];
    }
}

void AVAudioEngineSoundSource::GetOrientation(MathVector3& forward, MathVector3& up) const
{
    forward = MathVector3::Zero();
    up = MathVector3::Zero();
}

void AVAudioEngineSoundSource::SetOrientation(const MathVector3& forward, const MathVector3& up)
{
}

void AVAudioEngineSoundSource::GetPosition(MathVector3& value) const
{
    auto pos = impl->sourceVoice->player.position;
    value = MathVector3(pos.x, pos.y, pos.z);
}

void AVAudioEngineSoundSource::SetPosition(const MathVector3& value)
{
    if (impl->sourceVoice != nullptr)
        impl->sourceVoice->player.position = AVAudioMake3DPoint(value(0), value(1), value(2));
    else
    {
        impl->position = value;

        impl->invalidated |= SoundSourceChange::POSITION;
    }
}

void AVAudioEngineSoundSource::GetVelocity(MathVector3& value) const
{
    value = MathVector3::Zero();
}

void AVAudioEngineSoundSource::SetVelocity(const MathVector3& value)
{
}

uint8_t AVAudioEngineSoundSource::GetPriority() const
{
    return impl->priority;
}

void AVAudioEngineSoundSource::SetPriority(uint8_t priority)
{
    impl->priority = priority;
}
