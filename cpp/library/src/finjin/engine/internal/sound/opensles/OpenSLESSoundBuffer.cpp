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
#include "OpenSLESSoundBuffer.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "OpenSLESContext.hpp"
#include "OpenSLESSoundSource.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
OpenSLESSoundBuffer::OpenSLESSoundBuffer()
{
    this->context = nullptr;
    this->data = nullptr;
    this->dataLength = 0;
    this->timeLength = 0;
    this->requestedBytesPerChannel = 2;
    this->createTimeStamp = 0;
    this->sourcesHead = nullptr;
}

void OpenSLESSoundBuffer::Create(OpenSLESContext* context)
{
    this->context = context;
    this->decompressedBuffer.Create(0, context->GetAllocator(), MemoryResizeStrategy::REALLOCATE);
}

const Utf8String& OpenSLESSoundBuffer::GetDebugName() const
{
    return this->debugName;
}

void OpenSLESSoundBuffer::SetDebugName(const Utf8String& value)
{
    this->debugName = value;
}

const SoundFormat& OpenSLESSoundBuffer::GetSoundFormat() const
{
    return this->soundFormat;
}

SimpleTimeCounter OpenSLESSoundBuffer::GetTimeLength() const
{
    return this->timeLength;
}

int OpenSLESSoundBuffer::GetCreateTimeStamp() const
{
    return this->createTimeStamp;
}

void OpenSLESSoundBuffer::LoadFromMemory(const void* data, size_t length, const SoundFormat& soundFormat)
{
    this->data = static_cast<const uint8_t*>(data);
    this->dataLength = length;
    this->soundFormat = soundFormat;

    this->timeLength = this->soundFormat.CalculateTimeLength(this->dataLength);

    this->createTimeStamp = this->context->NewCreateTimeStamp();
}

int OpenSLESSoundBuffer::GetRequestedBytesPerChannel() const
{
    return this->requestedBytesPerChannel;
}

void OpenSLESSoundBuffer::SetRequestedBytesPerChannel(int requestedBytesPerChannel)
{
    this->requestedBytesPerChannel = requestedBytesPerChannel;
}

bool OpenSLESSoundBuffer::HasSources() const
{
    return this->sourcesHead != nullptr;
}

void OpenSLESSoundBuffer::AddSource(OpenSLESSoundSource* source)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->sourcesHead, soundBufferNext, source)
}

void OpenSLESSoundBuffer::RemoveSource(OpenSLESSoundSource* source)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_REMOVE_ITEM(this->sourcesHead, soundBufferNext, source)
}

const uint8_t* OpenSLESSoundBuffer::GetDataBytes() const
{
    return static_cast<const uint8_t*>(this->data);
}

size_t OpenSLESSoundBuffer::GetDataByteCount() const
{
    return this->dataLength;
}

ByteBuffer& OpenSLESSoundBuffer::GetDecompressedByteBuffer()
{
    return this->decompressedBuffer;
}
