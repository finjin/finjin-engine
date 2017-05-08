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
#include "AVAudioEngineSoundBuffer.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "AVAudioEngineSoundBufferImpl.hpp"
#include "AVAudioEngineSoundContextImpl.hpp"
#include "AVAudioEngineSoundSource.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
AVAudioEngineSoundBuffer::AVAudioEngineSoundBuffer() : impl(new AVAudioEngineSoundBufferImpl)
{
    this->sourcesHead = nullptr;
}

AVAudioEngineSoundBuffer::~AVAudioEngineSoundBuffer()
{
    delete impl;
}

void AVAudioEngineSoundBuffer::Create(AVAudioEngineSoundContextImpl* context)
{
    impl->context = context;
    impl->decompressedBuffer.Create(0, context->GetAllocator(), MemoryResizeStrategy::REALLOCATE);
}

const Utf8String& AVAudioEngineSoundBuffer::GetDebugName() const
{
    return impl->debugName;
}

void AVAudioEngineSoundBuffer::SetDebugName(const Utf8String& value)
{
    impl->debugName = value;
}

const SoundFormat& AVAudioEngineSoundBuffer::GetSoundFormat() const
{
    return impl->soundFormat;
}

SimpleTimeCounter AVAudioEngineSoundBuffer::GetTimeLength() const
{
    return impl->timeLength;
}

int AVAudioEngineSoundBuffer::GetCreateTimeStamp() const
{
    return impl->createTimeStamp;
}

bool AVAudioEngineSoundBuffer::HasSources() const
{
    return this->sourcesHead != nullptr;
}

void AVAudioEngineSoundBuffer::AddSource(AVAudioEngineSoundSource* source)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->sourcesHead, soundBufferNext, source)
}

void AVAudioEngineSoundBuffer::RemoveSource(AVAudioEngineSoundSource* source)
{
    FINJIN_INTRUSIVE_SINGLE_LIST_ADD_ITEM(this->sourcesHead, soundBufferNext, source)
}
