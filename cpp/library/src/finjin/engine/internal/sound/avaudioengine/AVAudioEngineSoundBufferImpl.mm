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
#include "AVAudioEngineSoundBufferImpl.hpp"
#include "AVAudioEngineSoundContext.hpp"
#include "AVAudioEngineSoundContextImpl.hpp"
#include "AVAudioEngineSoundSourceImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
AVAudioEngineSoundBufferImpl::AVAudioEngineSoundBufferImpl()
{
    this->context = nullptr;
    this->avbuffer = nullptr;
    this->timeLength = 0;
    this->createTimeStamp = 0;
}

void AVAudioEngineSoundBufferImpl::Set(AVAudioPCMBuffer* avbuffer, size_t length, const SoundFormat& soundFormat)
{
    this->avbuffer = avbuffer;
    this->soundFormat = soundFormat;
    this->timeLength = this->soundFormat.CalculateTimeLength(length);
    this->createTimeStamp = this->context->nsimpl->createID++;
}

AVAudioPCMBuffer* AVAudioEngineSoundBufferImpl::DetachBuffer()
{
    auto result = this->avbuffer;
    this->avbuffer = nullptr;
    return result;
}
