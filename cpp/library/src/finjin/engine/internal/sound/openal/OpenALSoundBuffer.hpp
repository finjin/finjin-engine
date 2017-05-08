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
#include "finjin/common/Setting.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/SoundFormat.hpp"
#include "OpenALIncludes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OpenALSoundSource;
    class OpenALContext;

    /**
     * A sound buffer holds an uncompressed sound's data in its entirety
     */
    class OpenALSoundBuffer
    {
        friend class OpenALContext;

    public:
        OpenALSoundBuffer();

        void Create(OpenALContext* context);

        const Utf8String& GetDebugName() const;
        void SetDebugName(const Utf8String& value);

        const SoundFormat& GetSoundFormat() const;

        SimpleTimeCounter GetTimeLength() const;

        //OpenAL-specific methods------------------
        int GetCreateTimeStamp() const;

        void LoadFromMemory(const void* data, size_t length, const SoundFormat& soundFormat);

        int GetRequestedBytesPerChannel() const;
        void SetRequestedBytesPerChannel(int requestedBytesPerChannel);

        bool HasSources() const;
        void AddSource(OpenALSoundSource* source);
        void RemoveSource(OpenALSoundSource* source);

        const uint8_t* GetDataBytes() const;
        size_t GetDataByteCount() const;

        ByteBuffer& GetDecompressedByteBuffer();

        const Setting<unsigned int>& _GetBufferID() const;

    public:
        OpenALContext* context;

        Setting<unsigned int> bufferID;

        const void* data;
        ALsizei dataLength;
        SoundFormat soundFormat;

        /** Total length of the OGG sound, in seconds */
        SimpleTimeCounter timeLength;

        /** The desired number of bytes per channel. Used when loading/decompressing OGG data */
        int requestedBytesPerChannel;

        int createTimeStamp;

        Utf8String debugName;

        ByteBuffer decompressedBuffer;

    public:
        OpenALSoundSource* sourcesHead; //Linked list of sources that use this sound buffer. Use sourcesHead->soundBufferNext to iterate through list
    };

} }
