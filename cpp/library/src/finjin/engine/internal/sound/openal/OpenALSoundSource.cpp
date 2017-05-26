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
#include "OpenALSoundSource.hpp"
#include "OpenALContext.hpp"
#include "OpenALContextImpl.hpp"
#include "OpenALEffect.hpp"
#include "OpenALEffectSlot.hpp"
#include "OpenALFilter.hpp"
#include "OpenALSoundBuffer.hpp"
#include "OpenALSoundGroup.hpp"

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
    LOOPING = 1 << 5,
    PLAYBACK_RATE = 1 << 8,
    VOLUME = 1 << 9,
    POSITION = 1 << 10,
    VELOCITY = 1 << 11,
    ORIENTATION = 1 << 12,

    ALL = 0xffff
};
FINJIN_ENUM_BITWISE_OPERATIONS(SoundSourceChange)

struct OpenALSoundSource::Impl
{
    Impl();

    void SetDefaults();

    void CommitDirectFilter(ALuint sourceID);
    void CommitEffectSlotFilters(ALuint sourceID);

    ALuint GetSourceID() const
    {
        assert(this->sourceVoice != nullptr);
        return (*this->sourceVoice)[0];
    }

    OpenALContext* context;

    SoundFormat soundFormat;
    OpenALSourceVoice* sourceVoice;

    OpenALSoundGroup* group;

    OpenALSoundBuffer* buffer;

    OpenALFilter* filter; //Direct filter

    struct EffectSlotInstance
    {
        EffectSlotInstance()
        {
            this->effectSlot = nullptr;
            this->filter = nullptr;
        }

        OpenALEffectSlot* effectSlot;
        OpenALFilter* filter;
    };

    /** Effect slots and the corresponding filter */
    StaticVector<EffectSlotInstance, 8> effectSlotInstances;

    uint8_t priority;

    SoundSourceState requestedState;

    bool firstUpdate;
    SoundSourceChange invalidated;
    bool isAmbient;
    bool isLooping;
    SimpleTimeCounter timeOffset;
    MathVector3 position;
    MathVector3 velocity;
    MathVector3 forward, up;
    float playbackRate;
    float volume;
    float coneMinVolume;
    float coneMaxVolume;
    Angle coneInnerAngle;
    Angle coneOuterAngle;
    float coneOuterVolume;

    Utf8String debugName;
};


//Implementation----------------------------------------------------------------

//OpenALSoundSource::Impl
OpenALSoundSource::Impl::Impl()
{
    this->context = nullptr;

    this->sourceVoice = nullptr;

    this->group = nullptr;
    this->buffer = nullptr;

    this->filter = nullptr;

    SetDefaults();
}

void OpenALSoundSource::Impl::SetDefaults()
{
    this->priority = SoundConstants::DEFAULT_SOURCE_PRIORITY;
    this->requestedState = SoundSourceState::NONE;
    this->firstUpdate = true;
    this->invalidated = SoundSourceChange::ALL;
    this->isAmbient = true;
    this->isLooping = false;
    this->timeOffset = 0;

    //These defaults are defined by the OpenAL specification
    this->playbackRate = 1.0f;
    this->volume = 1.0f;
    this->position = MathVector3::Zero();
    this->velocity = MathVector3::Zero();
    this->forward = MathVector3::Zero();
    this->up = MathVector3::Zero();
    this->coneMinVolume = 0.0f;
    this->coneMaxVolume = 1.0f;
    this->coneInnerAngle = Degrees(360);
    this->coneOuterAngle = Degrees(360);
    this->coneOuterVolume = 0;

    this->debugName.clear();
}

void OpenALSoundSource::Update(SimpleTimeDelta elapsedTime)
{
    //Update time offset if sound is playing
    if (AnySet(impl->requestedState & SoundSourceState::PLAYING))
    {
        auto newTimeOffset = impl->timeOffset;

        //The first update should not include the elapsed time in its calculation
        if (!impl->firstUpdate)
            newTimeOffset += elapsedTime;
        impl->firstUpdate = false;

        //std::cout << "Update '" << impl->debugName << "': it is playing, elapsed time: " << newTimeOffset << ", out of total: " << GetTimeLength() << std::endl;

        SetTimeOffset(newTimeOffset, false);
    }
}

void OpenALSoundSource::Commit(bool force)
{
    if (impl->sourceVoice == nullptr)
        return;

    if (force || impl->invalidated != SoundSourceChange::NONE || !impl->isAmbient)
    {
        //std::cout << "Commit '" << impl->debugName << "': it is invalidated" << std::endl;

        auto sourceID = impl->GetSourceID();
        auto contextImpl = impl->context->GetImpl();

        if (impl->isAmbient)
        {
            //2D
            if (force || AnySet(impl->invalidated & (SoundSourceChange::VOLUME | SoundSourceChange::AMBIENT)))
                alSourcef(sourceID, AL_GAIN, impl->volume);
            if (force || AnySet(impl->invalidated & (SoundSourceChange::PLAYBACK_RATE | SoundSourceChange::AMBIENT)))
                alSourcef(sourceID, AL_PITCH, impl->playbackRate);

            if (force || AnySet(impl->invalidated & SoundSourceChange::AMBIENT))
            {
                //Disable 3D
                alSourcei(sourceID, AL_SOURCE_RELATIVE, AL_TRUE);
                alSource3f(sourceID, AL_POSITION, 0, 0, 0);
                alSource3f(sourceID, AL_DIRECTION, 0, 0, 0);
                alSource3f(sourceID, AL_VELOCITY, 0, 0, 0);
                alSourcef(sourceID, AL_ROLLOFF_FACTOR, 1);
                alSourcef(sourceID, AL_REFERENCE_DISTANCE, 1);
                alSourcef(sourceID, AL_MAX_DISTANCE, 1);
                alSourcef(sourceID, AL_MIN_GAIN, 0);
                alSourcef(sourceID, AL_MAX_GAIN, 1);
                alSourcef(sourceID, AL_CONE_INNER_ANGLE, 360);
                alSourcef(sourceID, AL_CONE_OUTER_ANGLE, 360);
                alSourcef(sourceID, AL_CONE_OUTER_GAIN, 1);
            }
        }
        else
        {
            //3D
            if (force || AnySet(impl->invalidated & SoundSourceChange::AMBIENT))
            {
                alSourcef(sourceID, AL_GAIN, 1.0f);
                alSourcef(sourceID, AL_PITCH, 1.0f);
                alSourcei(sourceID, AL_SOURCE_RELATIVE, AL_FALSE);
            }

            if (force || AnySet(impl->invalidated & (SoundSourceChange::POSITION | SoundSourceChange::AMBIENT)))
                alSource3f(sourceID, AL_POSITION, impl->position(0), impl->position(1), impl->position(2));
            if (force || AnySet(impl->invalidated & (SoundSourceChange::ORIENTATION | SoundSourceChange::AMBIENT)))
                alSource3f(sourceID, AL_DIRECTION, impl->forward(0), impl->forward(1), impl->forward(2));
            if (force || AnySet(impl->invalidated & (SoundSourceChange::VELOCITY | SoundSourceChange::AMBIENT)))
                alSource3f(sourceID, AL_VELOCITY, impl->velocity(0), impl->velocity(1), impl->velocity(2));
            if (force || AnySet(impl->invalidated & SoundSourceChange::AMBIENT) || AnySet(contextImpl->invalidated & SoundContextChange::ROLLOFF_FACTOR))
                alSourcef(sourceID, AL_ROLLOFF_FACTOR, impl->context->GetRolloffFactor());
            if (force || AnySet(impl->invalidated & SoundSourceChange::AMBIENT) || AnySet(contextImpl->invalidated & SoundContextChange::REFERENCE_DISTANCE))
                alSourcef(sourceID, AL_REFERENCE_DISTANCE, impl->context->GetReferenceDistance());
            if (force || AnySet(impl->invalidated & SoundSourceChange::AMBIENT) || AnySet(contextImpl->invalidated & SoundContextChange::MAX_DISTANCE))
                alSourcef(sourceID, AL_MAX_DISTANCE, impl->context->GetMaxDistance());

            if (force || AnySet(impl->invalidated & (SoundSourceChange::CONE | SoundSourceChange::AMBIENT)))
            {
                alSourcef(sourceID, AL_MIN_GAIN, impl->coneMinVolume);
                alSourcef(sourceID, AL_MAX_GAIN, impl->coneMaxVolume);
                alSourcef(sourceID, AL_CONE_INNER_ANGLE, impl->coneInnerAngle.ToDegreesValue());
                alSourcef(sourceID, AL_CONE_OUTER_ANGLE, impl->coneOuterAngle.ToDegreesValue());
                alSourcef(sourceID, AL_CONE_OUTER_GAIN, impl->coneOuterVolume);
            }
        }

        if (force || AnySet(impl->invalidated & SoundSourceChange::LOOPING))
            alSourcei(sourceID, AL_LOOPING, impl->isLooping);// && impl->streamingBuffer == nullptr);

        impl->CommitDirectFilter(sourceID);
        impl->CommitEffectSlotFilters(sourceID);

        if (force || AnySet(impl->invalidated & (SoundSourceChange::VOICE | SoundSourceChange::BUFFER)))
        {
            SetTimeOffset(impl->timeOffset, true);
            if (impl->requestedState == SoundSourceState::PLAYING)
            {
                //If the sound is non-streaming, set its buffer into the source
                //Streaming buffers aren't set in this way because they are constantly queued elsewhere
                if (impl->buffer != nullptr)
                    alSourcei(sourceID, AL_BUFFER, impl->buffer->_GetBufferID());

                //Actually play the source
                alSourcePlay(sourceID);
            }
        }

        impl->invalidated = SoundSourceChange::NONE;
    }
}

void OpenALSoundSource::Impl::CommitDirectFilter(ALuint sourceID)
{
    if (this->context->HasExtension(OpenALKnownExtension::EFX))
    {
        ALuint filterID = (this->filter != nullptr) ? this->filter->_GetFilterID() : AL_FILTER_NULL;
        alSourcei(sourceID, AL_DIRECT_FILTER, filterID);
    }
}

void OpenALSoundSource::Impl::CommitEffectSlotFilters(ALuint sourceID)
{
    if (this->context->HasExtension(OpenALKnownExtension::EFX))
    {
        for (size_t i = 0; i < this->effectSlotInstances.size(); i++)
        {
            auto& instance = this->effectSlotInstances[i];
            auto effectSlotID = (instance.effectSlot != nullptr) ? instance.effectSlot->_GetEffectSlotID() : AL_EFFECTSLOT_NULL;
            auto filterID = (instance.filter != nullptr) ? instance.filter->_GetFilterID() : AL_FILTER_NULL;
            alSource3i(sourceID, AL_AUXILIARY_SEND_FILTER, effectSlotID, static_cast<int>(i), filterID);
        }
    }
}

//OpenALSoundSource
OpenALSoundSource::OpenALSoundSource() : impl(new Impl)
{
    this->soundBufferNext = nullptr;
    this->soundGroupNext = nullptr;
    this->filterNext = nullptr;
}

OpenALSoundSource::~OpenALSoundSource()
{
    delete impl;
}

void OpenALSoundSource::Create(OpenALContext* context)
{
    impl->context = context;

    impl->effectSlotInstances.resize(impl->context->GetMaxAuxSendsPerSource());
}

void OpenALSoundSource::SetDefaults()
{
    impl->SetDefaults();
}

const Utf8String& OpenALSoundSource::GetDebugName() const
{
    return impl->debugName;
}

void OpenALSoundSource::SetDebugName(const Utf8String& value)
{
    impl->debugName = value;
}

int OpenALSoundSource::GetBufferCreateTimeStamp() const
{
    if (impl->buffer != nullptr)
        return impl->buffer->GetCreateTimeStamp();
    else
        return 0;
}

OpenALSoundGroup* OpenALSoundSource::GetGroup()
{
    return impl->group;
}

void OpenALSoundSource::SetGroup(OpenALSoundGroup* group)
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

OpenALSoundBuffer* OpenALSoundSource::GetBuffer()
{
    return impl->buffer;
}

bool OpenALSoundSource::SetBuffer(OpenALSoundBuffer* soundBuffer)
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

const SoundFormat& OpenALSoundSource::GetSoundFormat() const
{
    return impl->soundFormat;
}

void OpenALSoundSource::SetSoundFormat(const SoundFormat& format)
{
    impl->soundFormat = format;
}

SoundSourceState OpenALSoundSource::GetState() const
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

void OpenALSoundSource::Play()
{
    if (impl->buffer != nullptr)
    {
        //If the sound is already playing, then start it over
        if (impl->requestedState == SoundSourceState::PLAYING)
            Rewind();

        if (impl->sourceVoice != nullptr)
            alSourcePlay(impl->GetSourceID());

        impl->requestedState = SoundSourceState::PLAYING;
    }
}

void OpenALSoundSource::Pause()
{
    if (impl->requestedState == SoundSourceState::PLAYING)
    {
        if (impl->sourceVoice != nullptr)
            alSourcePause(impl->GetSourceID());

        impl->requestedState = SoundSourceState::PAUSED;
    }
}

void OpenALSoundSource::Stop()
{
    if (impl->requestedState == SoundSourceState::PLAYING || impl->requestedState == SoundSourceState::PAUSED)
    {
        if (impl->sourceVoice != nullptr)
            alSourceStop(impl->GetSourceID());

        impl->requestedState = SoundSourceState::STOPPED;

        impl->timeOffset = 0;
        impl->firstUpdate = true;
    }
}

void OpenALSoundSource::Rewind()
{
    if (impl->buffer != nullptr)
    {
        if (impl->sourceVoice != nullptr)
        {
            alSourceStop(impl->GetSourceID());
            alSourceRewind(impl->GetSourceID());
        }

        impl->timeOffset = 0;
        impl->firstUpdate = true;
    }
}

SimpleTimeCounter OpenALSoundSource::GetTimeOffset() const
{
    return impl->timeOffset;
}

void OpenALSoundSource::SetTimeOffset(SimpleTimeCounter offset, bool adjustPlayCursor)
{
    if (adjustPlayCursor && impl->buffer == nullptr)
    {
        //Only sounds with regular buffers can have their play cursor adjusted
        return;
    }

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
            while (impl->timeOffset >= timeLength)
                impl->timeOffset -= timeLength;
        }
        else if (impl->timeOffset >= timeLength)
        {
            //The sound is done playing
            adjustPlayCursor = false;
            Stop();
        }
    }
    else
    {
        //There is no sound to be played
        adjustPlayCursor = false;
        impl->timeOffset = 0;
    }

    if (adjustPlayCursor)
    {
        //Ensure that the next update is considered the first so that we can avoid adjusting the cursor again before the sound is heard
        impl->firstUpdate = true;

        //Adjust the actual play cursor
        if (impl->sourceVoice != nullptr)
            alSourcef(impl->GetSourceID(), AL_SEC_OFFSET, impl->timeOffset);
    }
}

SimpleTimeCounter OpenALSoundSource::GetTimeLength() const
{
    SimpleTimeCounter timeLength = 0;
    if (impl->buffer != nullptr)
        timeLength = impl->buffer->GetTimeLength();
    return timeLength;
}

bool OpenALSoundSource::IsLooping() const
{
    return impl->isLooping;
}

void OpenALSoundSource::SetLooping(bool loop)
{
    if (impl->isLooping != loop)
    {
        impl->isLooping = loop;

        if (impl->sourceVoice != nullptr)
            alSourcei(impl->GetSourceID(), AL_LOOPING, impl->isLooping);
        else
            impl->invalidated |= SoundSourceChange::LOOPING;
    }
}

float OpenALSoundSource::GetVolume() const
{
    return impl->volume;
}

void OpenALSoundSource::SetVolume(float volume)
{
    Limit(volume, 0.0f, 1.0f);
    if (impl->volume != volume)
    {
        impl->volume = volume;
        if (impl->sourceVoice != nullptr && impl->isAmbient)
            alSourcef(impl->GetSourceID(), AL_GAIN, impl->volume);
        else
            impl->invalidated |= SoundSourceChange::VOLUME;
    }
}

bool OpenALSoundSource::IsAmbient() const
{
    return impl->isAmbient;
}

void OpenALSoundSource::SetAmbient(bool ambient)
{
    if (impl->isAmbient != ambient)
    {
        impl->isAmbient = ambient;

        impl->invalidated |= SoundSourceChange::AMBIENT;
    }
}

void OpenALSoundSource::MakeOmniDirectional()
{
    SetOrientation(MathVector3::Zero(), MathVector3::Zero());

    impl->invalidated |= SoundSourceChange::CONE;
}

void OpenALSoundSource::MakeDirectional()
{
    SetOrientation(FINJIN_OPENAL_DEFAULT_FORWARD, FINJIN_OPENAL_DEFAULT_UP);

    impl->invalidated |= SoundSourceChange::CONE;
}

Angle OpenALSoundSource::GetConeInnerAngle() const
{
    return impl->coneInnerAngle;
}

void OpenALSoundSource::SetConeInnerAngle(Angle innerAngle)
{
    auto value = innerAngle.ToDegreesValue();
    Limit(value, 0, 360);
    if (impl->coneInnerAngle.ToDegreesValue() != value)
    {
        impl->coneInnerAngle = Degrees(value);

        impl->invalidated |= SoundSourceChange::CONE;
    }
}

Angle OpenALSoundSource::GetConeOuterAngle() const
{
    return impl->coneOuterAngle;
}

void OpenALSoundSource::SetConeOuterAngle(Angle outerAngle)
{
    auto value = outerAngle.ToDegreesValue();
    Limit(value, 0, 360);
    if (impl->coneOuterAngle.ToDegreesValue() != value)
    {
        impl->coneOuterAngle = Degrees(value);

        impl->invalidated |= SoundSourceChange::CONE;
    }
}

float OpenALSoundSource::GetConeOuterVolume() const
{
    return impl->coneOuterVolume;
}

void OpenALSoundSource::SetConeOuterVolume(float outerGain)
{
    Limit(outerGain, 0.0f, 1.0f);
    if (impl->coneOuterVolume != outerGain)
    {
        impl->coneOuterVolume = outerGain;

        impl->invalidated |= SoundSourceChange::CONE;
    }
}

float OpenALSoundSource::GetConeMinVolume() const
{
    return impl->coneMinVolume;
}

void OpenALSoundSource::SetConeMinVolume(float minVolume)
{
    Limit(minVolume, 0.0f, 1.0f);
    if (impl->coneMinVolume != minVolume)
    {
        impl->coneMinVolume = minVolume;

        impl->invalidated |= SoundSourceChange::CONE;
    }
}

float OpenALSoundSource::GetConeMaxVolume() const
{
    return impl->coneMaxVolume;
}

void OpenALSoundSource::SetConeMaxVolume(float maxVolume)
{
    Limit(maxVolume, 0.0f, 1.0f);
    if (impl->coneMaxVolume != maxVolume)
    {
        impl->coneMaxVolume = maxVolume;

        impl->invalidated |= SoundSourceChange::CONE;
    }
}

void OpenALSoundSource::GetOrientation(MathVector3& forward, MathVector3& up) const
{
    forward = MathVector3(impl->forward[0], impl->forward[1], impl->forward[2]);
    up = MathVector3(impl->up[0], impl->up[1], impl->up[2]);
}

void OpenALSoundSource::SetOrientation(const MathVector3& forward, const MathVector3& up)
{
    impl->forward = forward;
    impl->up = up;

    if (impl->sourceVoice != nullptr && !impl->isAmbient)
        alSource3f(impl->GetSourceID(), AL_DIRECTION, impl->forward(0), impl->forward(1), impl->forward(2));
    else
        impl->invalidated |= SoundSourceChange::ORIENTATION;
}

void OpenALSoundSource::GetPosition(MathVector3& value) const
{
    value = impl->position;
}

void OpenALSoundSource::SetPosition(const MathVector3& value)
{
    if (value != impl->position)
    {
        impl->position = value;

        if (impl->sourceVoice != nullptr && !impl->isAmbient)
            alSource3f(impl->GetSourceID(), AL_POSITION, impl->position(0), impl->position(1), impl->position(2));
        else
            impl->invalidated |= SoundSourceChange::POSITION;
    }
}

void OpenALSoundSource::GetVelocity(MathVector3& value) const
{
    value = impl->velocity;
}

void OpenALSoundSource::SetVelocity(const MathVector3& value)
{
    if (value != impl->velocity)
    {
        impl->velocity = value;

        if (impl->sourceVoice != nullptr && !impl->isAmbient)
            alSource3f(impl->GetSourceID(), AL_VELOCITY, impl->velocity(0), impl->velocity(1), impl->velocity(2));
        else
            impl->invalidated |= SoundSourceChange::VELOCITY;
    }
}

uint8_t OpenALSoundSource::GetPriority() const
{
    return impl->priority;
}

void OpenALSoundSource::SetPriority(uint8_t priority)
{
    impl->priority = priority;
}

float OpenALSoundSource::GetPlaybackRate() const
{
    return impl->playbackRate;
}

void OpenALSoundSource::SetPlaybackRate(float playbackRate)
{
    Limit(playbackRate, .5f, 2.0f);
    if (impl->playbackRate != playbackRate)
    {
        impl->playbackRate = playbackRate;
        if (impl->sourceVoice != nullptr && impl->isAmbient)
            alSourcef(impl->GetSourceID(), AL_PITCH, impl->playbackRate);
        else
            impl->invalidated |= SoundSourceChange::PLAYBACK_RATE;
    }
}

OpenALSourceVoice* OpenALSoundSource::GetVoice()
{
    return impl->sourceVoice;
}

OpenALSourceVoice* OpenALSoundSource::DetachVoice()
{
    if (impl->sourceVoice != nullptr)
        impl->invalidated |= SoundSourceChange::VOICE;

    auto result = impl->sourceVoice;
    impl->sourceVoice = nullptr;
    return result;
}

void OpenALSoundSource::SetVoice(OpenALSourceVoice* sourceVoice)
{
    if (impl->sourceVoice != sourceVoice)
    {
        impl->sourceVoice = sourceVoice;

        impl->invalidated |= SoundSourceChange::VOICE;
    }
}

OpenALFilter* OpenALSoundSource::GetFilter()
{
    return impl->filter;
}

void OpenALSoundSource::SetFilter(OpenALFilter* filter)
{
    //Remove this source from existing filter
    if (impl->filter != nullptr)
        impl->filter->RemoveSource(this);

    //Set new filter
    impl->filter = filter;

    //Add this source to new filter
    ALuint filterID = AL_FILTER_NULL;
    if (impl->filter != nullptr)
    {
        filterID = impl->filter->_GetFilterID();
        impl->filter->AddSource(this);
    }

    //Attach/detach OpenAL filter ID
    if (impl->sourceVoice != nullptr && impl->context->HasExtension(OpenALKnownExtension::EFX))
        alSourcei(impl->GetSourceID(), AL_DIRECT_FILTER, filterID);
}

size_t OpenALSoundSource::GetEffectSlotCount() const
{
    return impl->effectSlotInstances.size();
}

OpenALFilter* OpenALSoundSource::GetEffectSlotFilter(size_t index)
{
    return impl->effectSlotInstances[index].filter;
}

void OpenALSoundSource::GetEffectSlot(size_t index, OpenALEffectSlot** effectSlot, OpenALFilter** filter)
{
    auto& instance = impl->effectSlotInstances[index];

    if (effectSlot != nullptr)
        *effectSlot = instance.effectSlot;
    if (filter != nullptr)
        *filter = instance.filter;
}

void OpenALSoundSource::SetEffectSlot(size_t index, OpenALEffectSlot* effectSlot, OpenALFilter* filter)
{
    auto& instance = impl->effectSlotInstances[index];

    //Make sure there's no filter if there's no effect slot
    if (effectSlot == nullptr)
        filter = nullptr;

    //Remove this source from existing effect slot and filter
    if (instance.filter != nullptr)
        instance.filter->RemoveSource(this);

    //Set new effect slot and filter
    instance.effectSlot = effectSlot;
    instance.filter = filter;

    //Add this source to new effect slot and filter
    ALuint effectSlotID = AL_EFFECTSLOT_NULL;
    ALuint filterID = AL_FILTER_NULL;
    if (instance.effectSlot != nullptr)
    {
        effectSlotID = instance.effectSlot->_GetEffectSlotID();
    }
    if (instance.filter != nullptr)
    {
        filterID = instance.filter->_GetFilterID();
        instance.filter->AddSource(this);
    }

    //Attached/detach OpenAL effect slot and filter IDs
    if (impl->sourceVoice != nullptr && impl->context->HasExtension(OpenALKnownExtension::EFX))
        alSource3i(impl->GetSourceID(), AL_AUXILIARY_SEND_FILTER, effectSlotID, index, filterID);
}
