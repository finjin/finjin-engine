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
#include "XAudio2SoundBuffer.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "XAudio2Context.hpp"
#include "XAudio2ContextImpl.hpp"
#include "XAudio2SoundSource.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
XAudio2SoundBuffer::XAudio2SoundBuffer()
{
    this->context = nullptr;
    this->data = nullptr;
    this->dataLength = 0;
    this->timeLength = 0;
    this->requestedBytesPerChannel = 2;
    this->createTimeStamp = 0;
    this->sourcesHead = nullptr;
}

void XAudio2SoundBuffer::Create(XAudio2Context* context)
{
    this->context = context;
    this->decompressedBuffer.Create(0, context->GetAllocator(), MemoryResizeStrategy::REALLOCATE);
}

const Utf8String& XAudio2SoundBuffer::GetDebugName() const
{
    return this->debugName;
}

void XAudio2SoundBuffer::SetDebugName(const Utf8String& value)
{
    this->debugName = value;
}

const SoundFormat& XAudio2SoundBuffer::GetSoundFormat() const
{
    return this->soundFormat;
}

SimpleTimeCounter XAudio2SoundBuffer::GetTimeLength() const
{
    return this->timeLength;
}

int XAudio2SoundBuffer::GetCreateTimeStamp() const
{
    return this->createTimeStamp;
}

void XAudio2SoundBuffer::LoadFromMemory(const void* data, size_t length, const SoundFormat& soundFormat)
{
    this->data = data;
    this->dataLength = length;
    this->soundFormat = soundFormat;

    this->timeLength = this->soundFormat.CalculateTimeLength(this->dataLength);

    this->createTimeStamp = this->context->GetImpl()->createID++;
}

int XAudio2SoundBuffer::GetRequestedBytesPerChannel() const
{
    return this->requestedBytesPerChannel;
}

void XAudio2SoundBuffer::SetRequestedBytesPerChannel(int requestedBytesPerChannel)
{
    this->requestedBytesPerChannel = requestedBytesPerChannel;
}

bool XAudio2SoundBuffer::HasSources() const
{
    return this->sourcesHead != nullptr;
}

void XAudio2SoundBuffer::AddSource(XAudio2SoundSource* source)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->sourcesHead, soundBufferNext, source)
}

void XAudio2SoundBuffer::RemoveSource(XAudio2SoundSource* source)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_REMOVE_ITEM(this->sourcesHead, soundBufferNext, source)
}

const uint8_t* XAudio2SoundBuffer::GetDataBytes() const
{
    return static_cast<const uint8_t*>(this->data);
}

size_t XAudio2SoundBuffer::GetDataByteCount() const
{
    return this->dataLength;
}

ByteBuffer& XAudio2SoundBuffer::GetDecompressedByteBuffer()
{
    return this->decompressedBuffer;
}
