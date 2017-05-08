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
#include "OpenALSoundBuffer.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "OpenALContext.hpp"
#include "OpenALContextImpl.hpp"
#include "OpenALSoundSource.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
OpenALSoundBuffer::OpenALSoundBuffer()
{
    this->context = nullptr;
    this->bufferID.Reset(0);
    this->data = 0;
    this->dataLength = 0;
    this->timeLength = 0;
    this->requestedBytesPerChannel = 2;
    this->createTimeStamp = 0;
    this->sourcesHead = nullptr;
}

void OpenALSoundBuffer::Create(OpenALContext* context)
{
    this->context = context;
    this->decompressedBuffer.Create(0, context->GetAllocator(), MemoryResizeStrategy::REALLOCATE);
}

const Utf8String& OpenALSoundBuffer::GetDebugName() const
{
    return this->debugName;
}

void OpenALSoundBuffer::SetDebugName(const Utf8String& value)
{
    this->debugName = value;
}

const SoundFormat& OpenALSoundBuffer::GetSoundFormat() const
{
    return this->soundFormat;
}

SimpleTimeCounter OpenALSoundBuffer::GetTimeLength() const
{
    return this->timeLength;
}

int OpenALSoundBuffer::GetCreateTimeStamp() const
{
    return this->createTimeStamp;
}

void OpenALSoundBuffer::LoadFromMemory(const void* data, size_t length, const SoundFormat& soundFormat)
{
    this->data = data;
    this->dataLength = length;
    this->soundFormat = soundFormat;

    this->timeLength = this->soundFormat.CalculateTimeLength(this->dataLength);

    this->createTimeStamp = this->context->GetImpl()->createID++;

    //Delete existing sound buffer
    if (this->bufferID.IsSet())
        alDeleteBuffers(1, &this->bufferID.value);

    //Create new sound buffer
    ALuint newBufferID;
    alGenBuffers(1, &newBufferID);
    this->bufferID = newBufferID;

    //Put the data in the sound buffer
    ALenum format = this->context->SoundFormatToEnum(this->soundFormat);
    alBufferData(this->bufferID.value, format, this->data, this->dataLength, (ALsizei)this->soundFormat.samplesPerSecond);
    alGetError();
}

int OpenALSoundBuffer::GetRequestedBytesPerChannel() const
{
    return this->requestedBytesPerChannel;
}

void OpenALSoundBuffer::SetRequestedBytesPerChannel(int requestedBytesPerChannel)
{
    this->requestedBytesPerChannel = requestedBytesPerChannel;
}

bool OpenALSoundBuffer::HasSources() const
{
    return this->sourcesHead != nullptr;
}

void OpenALSoundBuffer::AddSource(OpenALSoundSource* source)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->sourcesHead, soundBufferNext, source)
}

void OpenALSoundBuffer::RemoveSource(OpenALSoundSource* source)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_REMOVE_ITEM(this->sourcesHead, soundBufferNext, source)
}

const uint8_t* OpenALSoundBuffer::GetDataBytes() const
{
    return static_cast<const uint8_t*>(this->data);
}

size_t OpenALSoundBuffer::GetDataByteCount() const
{
    return this->dataLength;
}

ByteBuffer& OpenALSoundBuffer::GetDecompressedByteBuffer()
{
    return this->decompressedBuffer;
}

const Setting<unsigned int>& OpenALSoundBuffer::_GetBufferID() const
{
    return this->bufferID;
}
