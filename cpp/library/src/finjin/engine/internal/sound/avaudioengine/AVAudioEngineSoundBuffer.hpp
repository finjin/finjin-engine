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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/SoundFormat.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class AVAudioEngineSoundSource;
    class AVAudioEngineSoundBufferImpl;
    class AVAudioEngineSoundContextImpl;

    class AVAudioEngineSoundBuffer
    {
        friend class AVAudioEngineSoundSource;
        friend class AVAudioEngineSoundContextImpl;

    public:
        AVAudioEngineSoundBuffer();
        ~AVAudioEngineSoundBuffer();

        void Create(AVAudioEngineSoundContextImpl* context);

        const Utf8String& GetDebugName() const;
        void SetDebugName(const Utf8String& value);

        const SoundFormat& GetSoundFormat() const;

        SimpleTimeCounter GetTimeLength() const;

        //AVAudioEngine-related methods-----------------
        int GetCreateTimeStamp() const;

        bool HasSources() const;
        void AddSource(AVAudioEngineSoundSource* source);
        void RemoveSource(AVAudioEngineSoundSource* source);

    public:
        AVAudioEngineSoundBufferImpl* impl;

    public:
        AVAudioEngineSoundSource* sourcesHead; //Linked list of sources that use this sound buffer. Use sourcesHead->soundBufferNext to iterate through list
    };

} }
