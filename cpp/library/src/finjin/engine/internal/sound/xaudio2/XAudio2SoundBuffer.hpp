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
#include "finjin/engine/SoundFormat.hpp"
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class XAudio2SoundSource;
    class XAudio2Context;

    class XAudio2SoundBuffer
    {
    public:
        XAudio2SoundBuffer();

        void Create(XAudio2Context* context);

        const Utf8String& GetDebugName() const;
        void SetDebugName(const Utf8String& value);

        const SoundFormat& GetSoundFormat() const;

        SimpleTimeCounter GetTimeLength() const;

        //XAudio-related methods----------------
        int GetCreateTimeStamp() const;

        void LoadFromMemory(const void* data, size_t length, const SoundFormat& soundFormat);

        int GetRequestedBytesPerChannel() const;
        void SetRequestedBytesPerChannel(int requestedBytesPerChannel);

        bool HasSources() const;
        void AddSource(XAudio2SoundSource* source);
        void RemoveSource(XAudio2SoundSource* source);

        const uint8_t* GetDataBytes() const;
        size_t GetDataByteCount() const;

        ByteBuffer& GetDecompressedByteBuffer();

    private:
        XAudio2Context* context;

        const void* data;
        size_t dataLength;
        SoundFormat soundFormat;

        //Total length of the sound, in seconds
        SimpleTimeCounter timeLength;

        //The desired number of bytes per channel. Used when loading/decompressing OGG data
        int requestedBytesPerChannel;

        int createTimeStamp;

        Utf8String debugName;

        ByteBuffer decompressedBuffer;

    public:
        XAudio2SoundSource* sourcesHead; //Linked list of sources that use this sound buffer. Use sourcesHead->soundBufferNext to iterate through list
    };

} }
