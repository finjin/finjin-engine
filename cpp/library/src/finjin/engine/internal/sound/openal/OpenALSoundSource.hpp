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
#include "finjin/common/Angle.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/SoundContextCommonSettings.hpp"
#include "finjin/engine/SoundFormat.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OpenALSoundBuffer;
    class OpenALFilter;
    class OpenALEffectSlot;
    class OpenALSoundGroup;
    class OpenALContext;

    using OpenALSourceVoice = StaticVector<ALuint, FINJIN_OPENAL_MAX_SOURCE_IDS>;

    class OpenALSoundSource
    {
    public:
        OpenALSoundSource();
        ~OpenALSoundSource();

        void Create(OpenALContext* context);

        void SetDefaults();

        void Update(SimpleTimeDelta elapsedTime);
        void Commit(bool force = false);

        const Utf8String& GetDebugName() const;
        void SetDebugName(const Utf8String& value);

        int GetBufferCreateTimeStamp() const;

        OpenALSoundGroup* GetGroup();
        void SetGroup(OpenALSoundGroup* group);

        OpenALSoundBuffer* GetBuffer();
        bool SetBuffer(OpenALSoundBuffer* soundBuffer);

        const SoundFormat& GetSoundFormat() const;
        void SetSoundFormat(const SoundFormat& format);

        SoundSourceState GetState() const;

        void Play();
        void Pause();
        void Stop();
        void Rewind();

        SimpleTimeCounter GetTimeOffset() const;
        void SetTimeOffset(SimpleTimeCounter offset, bool adjustPlayCursor);

        SimpleTimeCounter GetTimeLength() const;

        bool IsLooping() const;
        void SetLooping(bool loop = true);

        float GetVolume() const;
        void SetVolume(float volume); //0 = silent. 1 = full volume

        bool IsAmbient() const; //Determines if the sound is 2D (ambient).
        void SetAmbient(bool ambient = true);

        void MakeOmniDirectional();
        void MakeDirectional();

        Angle GetConeInnerAngle() const;
        void SetConeInnerAngle(Angle innerAngle);

        Angle GetConeOuterAngle() const;
        void SetConeOuterAngle(Angle outerAngle);

        float GetConeOuterVolume() const;
        void SetConeOuterVolume(float outerGain);

        float GetConeMinVolume() const;
        void SetConeMinVolume(float minVolume);

        float GetConeMaxVolume() const;
        void SetConeMaxVolume(float maxVolume);

        void GetOrientation(MathVector3& forward, MathVector3& up) const;
        void SetOrientation(const MathVector3& forward, const MathVector3& up);

        void GetPosition(MathVector3& value) const;
        void SetPosition(const MathVector3& value);

        void GetVelocity(MathVector3& value) const;
        void SetVelocity(const MathVector3& value);

        uint8_t GetPriority() const;
        void SetPriority(uint8_t priority);

        float GetPlaybackRate() const;
        void SetPlaybackRate(float playbackRate);

        //OpenAL-specific methods---------------------
        OpenALSourceVoice* GetVoice();
        OpenALSourceVoice* DetachVoice();
        void SetVoice(OpenALSourceVoice* sourceVoice);

        OpenALFilter* GetFilter();
        void SetFilter(OpenALFilter* filter);

        size_t GetEffectSlotCount() const;
        OpenALFilter* GetEffectSlotFilter(size_t index);
        void GetEffectSlot(size_t index, OpenALEffectSlot** effectSlot = nullptr, OpenALFilter** filter = nullptr);
        void SetEffectSlot(size_t index, OpenALEffectSlot* effectSlot, OpenALFilter* filter = nullptr);

    private:
        struct Impl;
        Impl* impl;

    public:
        OpenALSoundSource* soundBufferNext; //Used to create a linked list of sound sources that share the same sound buffer
        OpenALSoundSource* soundGroupNext; //Used to create a linked list of sound sources that share the same sound group
        OpenALSoundSource* filterNext; //Used to create a linked list of sound sources that share the same filter
    };

} }
