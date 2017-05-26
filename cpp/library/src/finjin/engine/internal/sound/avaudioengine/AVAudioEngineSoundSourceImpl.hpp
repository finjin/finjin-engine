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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/Utf8String.hpp"
#include "SoundFormat.hpp"
#include "SoundSourceCommon.hpp"
#import <AVFoundation/AVFoundation.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class AVAudioEngineSoundBuffer;
    class AVAudioEngineSoundContextImpl;
    class AVAudioEngineSoundGroup;

    enum class SoundSourceChange
    {
        NONE = 0,

        VOICE = 1 << 0,
        AMBIENT = 1 << 2,
        BUFFER = 1 << 4,
        PLAYBACK_RATE = 1 << 7,
        VOLUME = 1 << 8,
        POSITION = 1 << 9,

        ALL = 0xffff
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(SoundSourceChange)

    class AVAudioEngineSoundSourceVoice
    {
    public:
        AVAudioEngineSoundSourceVoice()
        {
            this->player = nullptr;
        }

    public:
        AVAudioPlayerNode* player;
    };

    class AVAudioEngineSoundSourceImpl
    {
    public:
        AVAudioEngineSoundSourceImpl();

        void SetDefaults();

        AVAudioEngineSoundSourceVoice* DetachVoice();

        void SetVoice(AVAudioEngineSoundSourceVoice* voice);

    public:
        AVAudioEngineSoundContextImpl* context;

        SoundFormat soundFormat;
        AVAudioEngineSoundSourceVoice* sourceVoice;

        AVAudioEngineSoundGroup* group;
        AVAudioEngineSoundBuffer* buffer;

        uint8_t priority;

        SoundSourceState requestedState;

        bool firstUpdate;
        SoundSourceChange invalidated;
        bool isAmbient;
        bool isLooping;
        SimpleTimeCounter timeOffset;
        MathVector3 position;
        float playbackRate;
        float volume;

        Utf8String debugName;
    };

} }
