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
#include "XAudio2ContextImpl.hpp"
#include "finjin/common/WrappedFileReader.hpp"
#include "finjin/engine/WAVReader.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//XAudio2ContextImpl::SoundSourceLookup
bool XAudio2ContextImpl::SoundSourceLookup::Virtualize(XAudio2SoundSource* source)
{
    //Remove voice from source
    auto sourceVoice = source->DetachVoice();
    if (sourceVoice != nullptr)
    {
        sourceVoice->Stop();

        //Remove from used list
        this->sourceVoices.Unuse(sourceVoice);

        return true;
    }

    return false;
}

bool XAudio2ContextImpl::SoundSourceLookup::Realize(XAudio2SoundSource* source)
{
    auto sourceVoice = source->GetVoice();
    if (sourceVoice == nullptr)
    {
        //Find unused. This should not fail
        sourceVoice = this->sourceVoices.FindUnused();
        assert(sourceVoice != nullptr);

        //Set voice into source
        source->SetVoice(sourceVoice);

        //Add to used list
        this->sourceVoices.Use(sourceVoice);

        return true;
    }

    return false;
}

//XAudio2ContextImpl
XAudio2ContextImpl::XAudio2ContextImpl(Allocator* allocator, XAudio2System* soundSystem) : AllocatedClass(allocator), settings(allocator)
{
    this->soundSystem = soundSystem;

    this->xaudioInterface = nullptr;
    this->masteringVoice = nullptr;
    this->masteringVoiceChannelCount = 0;

    FINJIN_ZERO_ITEM(this->x3dAudioInterface);

    this->soundSorter = nullptr;
    this->soundSortingEnabled = true;
    this->metersPerUnit = 1;
    this->dopplerFactor = 0;
    this->referenceDistance = 1;
    this->maxDistance = 1;
    this->rolloffFactor = 1;
    this->createID = 1;
}

bool XAudio2ContextImpl::CreateSourceVoice(XAudio2SoundSourceVoice& sourceVoice, const SoundFormat& soundFormat)
{
    //Configure wave format
    WAVEFORMATEX wfx;
    FINJIN_ZERO_ITEM(wfx);
    wfx.cbSize = sizeof(wfx);
    wfx.nChannels = soundFormat.GetChannelCount();
    wfx.nSamplesPerSec = soundFormat.samplesPerSecond;
    wfx.wBitsPerSample = soundFormat.GetBytesPerSample() * 8;
    wfx.nAvgBytesPerSec = soundFormat.GetBytesPerSecond();
    wfx.nBlockAlign = soundFormat.GetBytesPerSample();
    wfx.wFormatTag = WAVE_FORMAT_PCM;

    UINT32 flags = 0;
#if _WIN32_WINNT <= _WIN32_WINNT_WIN7
    flags = XAUDIO2_VOICE_USEFILTER;
#endif
    float maxFrequencyRatio = XAUDIO2_DEFAULT_FREQ_RATIO;
    IXAudio2VoiceCallback* voiceCallback = nullptr;
    XAUDIO2_VOICE_SENDS* sendList = nullptr;
    XAUDIO2_EFFECT_CHAIN* effectChain = nullptr;

    //Configure audio source
    if (FAILED(this->xaudioInterface->CreateSourceVoice(&sourceVoice.xaudioSourceVoice, &wfx, flags, maxFrequencyRatio, voiceCallback, sendList, effectChain)))
        return false;

    return true;
}

XAudio2ContextImpl::SoundSourceLookup* XAudio2ContextImpl::GetSoundSourceLookup(const SoundFormat& soundFormat)
{
    for (auto& soundSourceLookup : this->soundSourceLookups)
    {
        if (soundSourceLookup.format == soundFormat)
            return &soundSourceLookup;
    }

    return nullptr;
}

XAudio2ContextImpl::SoundSourceLookup* XAudio2ContextImpl::AddSoundSourceLookup(const SoundFormat& soundFormat, size_t maxVoiceCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Exit if there aren't any more slots left
    if (this->soundSourceLookups.full())
    {
        FINJIN_SET_ERROR(error, "All sound source lookup slots are in use.");
        return nullptr;
    }

    //Add new lookup item
    this->soundSourceLookups.push_back();
    auto soundSourceLookup = &soundSourceLookups.back();

    //Initialize
    soundSourceLookup->format = soundFormat;
    if (!soundSourceLookup->sourceVoices.CreateEmpty(maxVoiceCount, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create source voices lookup.");
        return soundSourceLookup;
    }
    if (!soundSourceLookup->sources.CreateEmpty(maxVoiceCount, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create sources lookup.");
        return soundSourceLookup;
    }
    if (!soundSourceLookup->playingList.CreateEmpty(maxVoiceCount, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create playing list lookup.");
        return soundSourceLookup;
    }

    return soundSourceLookup;
}

XAudio2SoundSource* XAudio2ContextImpl::CreateSource(const SoundFormat& soundFormat, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto soundSourceLookup = GetSoundSourceLookup(soundFormat);
    if (soundSourceLookup == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("The specified sound format '%1%' doesn't have any precreated instances.", soundFormat.ToString()));
        return nullptr;
    }

    auto source = soundSourceLookup->sources.FindUnused();
    if (source == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("The maximum number of sound sources of the specified format '%1%' have already been created.", soundFormat.ToString()));
        return nullptr;
    }

    source->SetDefaults();

    source->SetGroup(&this->masterSoundGroup);

    soundSourceLookup->sources.Use(source);

    return source;
}

XAudio2SoundBuffer* XAudio2ContextImpl::CreateBuffer(const ByteBuffer& bytes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Find unused item
    auto item = this->buffers.FindUnused();
    if (item == nullptr)
    {
        FINJIN_SET_ERROR(error, "The maximum number of sound buffers have already been created.");
        return nullptr;
    }

    ByteBufferReader reader(bytes);

    WrappedFileReader wrappedFileReader;
    auto wrappedReadHeaderResult = wrappedFileReader.ReadHeader(reader);
    if (wrappedReadHeaderResult == WrappedFileReader::ReadHeaderResult::SUCCESS)
    {
        if (wrappedFileReader.GetHeader().fileExtension != "wav")
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid sound format '%1%'.", wrappedFileReader.GetHeader().fileExtension));
            return nullptr;
        }
    }
    else if (wrappedReadHeaderResult == WrappedFileReader::ReadHeaderResult::INVALID_SIGNATURE_VALUE)
    {
        //Not a wrapped file
    }

    //Try to read the bytes as a WAV file
    WAVReader wavReader;
    auto wavData = wavReader.Read(reader, item->GetDecompressedByteBuffer(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to decode sound buffer file bytes.");
        return nullptr;
    }

    //Finish
    this->buffers.Use(item);
    item->LoadFromMemory(wavData.data, wavData.length, wavData.soundFormat);
    return item;
}
