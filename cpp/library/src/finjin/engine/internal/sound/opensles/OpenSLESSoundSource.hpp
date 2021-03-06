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
#include "finjin/common/Angle.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/SoundContextCommonSettings.hpp"
#include "finjin/engine/SoundFormat.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OpenSLESContext;
    class OpenSLESSoundBuffer;
    class OpenSLESSoundGroup;
    class OpenSLESSoundSourceVoice;

    class OpenSLESSoundSource
    {
        friend class OpenSLESSoundBuffer;
        friend class OpenSLESSoundGroup;

    public:
        OpenSLESSoundSource();
        ~OpenSLESSoundSource();

        void Create(OpenSLESContext* context);

        void SetDefaults();

        void Update(SimpleTimeDelta elapsedTime);
        void Commit(bool force = false);

        const Utf8String& GetDebugName() const;
        void SetDebugName(const Utf8String& value);

        int GetBufferCreateTimeStamp() const;

        OpenSLESSoundGroup* GetGroup();
        void SetGroup(OpenSLESSoundGroup* group);

        OpenSLESSoundBuffer* GetBuffer();
        bool SetBuffer(OpenSLESSoundBuffer* soundBuffer);

        const SoundFormat& GetSoundFormat() const;
        void SetSoundFormat(const SoundFormat& format);

        SoundSourceState GetState() const;

        void Play();
        void Pause();
        void Stop();
        void Rewind();

        SimpleTimeCounter GetTimeOffset() const;
        void SetTimeOffset(SimpleTimeCounter offset, bool adjustPlayingSource);

        SimpleTimeCounter GetTimeLength() const;

        bool IsLooping() const;
        void SetLooping(bool loop = true); //Only applies when a sound is started. Won't have any effect if the sound is already playing

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
        void SetConeOuterVolume(float outerVolume);

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

        //OpenSL-related methods-----------------
        OpenSLESSoundSourceVoice* GetVoice();
        OpenSLESSoundSourceVoice* DetachVoice();
        void SetVoice(OpenSLESSoundSourceVoice* voice);

        void EnqueueBufferAndPlay();

    private:
        struct Impl;
        Impl* impl;

    public:
        OpenSLESSoundSource* soundBufferNext; //Used to create a linked list of sound sources that share the same sound buffer
        OpenSLESSoundSource* soundGroupNext; //Used to create a linked list of sound sources that share the same sound group
    };

} }
