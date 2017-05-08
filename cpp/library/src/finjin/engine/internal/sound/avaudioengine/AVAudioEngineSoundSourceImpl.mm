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
#include "AVAudioEngineSoundSourceImpl.hpp"
#include "AVAudioEngineSoundBuffer.hpp"
#include "AVAudioEngineSoundBufferImpl.hpp"
#include "AVAudioEngineSoundContextImpl.hpp"
#include "AVAudioEngineSoundGroup.hpp"
#include "AVAudioEngineSoundListener.hpp"
#include "AVAudioEngineSoundSourceImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
AVAudioEngineSoundSourceImpl::AVAudioEngineSoundSourceImpl()
{
    this->context = nullptr;

    this->sourceVoice = nullptr;

    this->group = nullptr;
    this->buffer = nullptr;

    SetDefaults();
}

void AVAudioEngineSoundSourceImpl::SetDefaults()
{
    this->priority = SoundConstants::DEFAULT_SOURCE_PRIORITY;
    this->requestedState = SoundSourceState::NONE;
    this->firstUpdate = true;
    this->invalidated = SoundSourceChange::ALL;
    this->isAmbient = true;
    this->isLooping = false;
    this->timeOffset = 0;

    this->playbackRate = 1.0f;
    this->volume = 1.0f;
    this->position = MathVector3::Zero();

    this->debugName.clear();
}

AVAudioEngineSoundSourceVoice* AVAudioEngineSoundSourceImpl::DetachVoice()
{
    auto result = this->sourceVoice;
    this->sourceVoice = nullptr;
    return result;
}

void AVAudioEngineSoundSourceImpl::SetVoice(AVAudioEngineSoundSourceVoice* voice)
{
    if (this->sourceVoice != voice)
    {
        this->sourceVoice = voice;

        this->invalidated |= SoundSourceChange::VOICE;
    }
}
