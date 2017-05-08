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
#include "OpenALContextImpl.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/WrappedFileReader.hpp"
#include "finjin/engine/WAVReader.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//OpenALContextImpl::SoundSourceLookup
bool OpenALContextImpl::SoundSourceLookup::Virtualize(OpenALSoundSource* source)
{
    //Remove voice from source
    auto sourceVoice = source->DetachVoice();
    if (sourceVoice != nullptr)
    {
        auto& sourceIDs = *sourceVoice;

        alSourceStop(sourceIDs[0]);

        //Remove from used list
        this->sourceVoices.Unuse(sourceVoice);

        return true;
    }

    return false;
}

bool OpenALContextImpl::SoundSourceLookup::Realize(OpenALSoundSource* source)
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

//OpenALContextImpl
OpenALContextImpl::OpenALContextImpl(Allocator* allocator, OpenALSystem* soundSystem) : AllocatedClass(allocator), settings(allocator)
{
    this->soundSystem = soundSystem;

    this->aldevice = nullptr;
    this->alcontext = nullptr;

    this->maxAuxSendsPerSource = 0;
    this->extensions = OpenALKnownExtension::NONE;

    this->invalidated = SoundContextChange::ALL;
    this->soundSorter = nullptr;
    this->soundSortingEnabled = true;
    this->metersPerUnit = 1;
    this->referenceDistance = 1;
    this->maxDistance = 1;
    this->rolloffFactor = 1;
    this->createID = 1;
}

void OpenALContextImpl::DestroyFilter(OpenALFilter* filter, bool shuttingDown)
{
    if (filter->filterID != AL_FILTER_NULL)
    {
        this->efxExtensions.alDeleteFilters(1, &filter->filterID);
        filter->filterID = AL_FILTER_NULL;
    }
}

OpenALContextImpl::SoundSourceLookup* OpenALContextImpl::GetSoundSourceLookup(const SoundFormat& soundFormat)
{
    for (auto& soundSourceLookup : this->soundSourceLookups)
    {
        if (soundSourceLookup.format == soundFormat)
            return &soundSourceLookup;
    }

    return nullptr;
}

OpenALContextImpl::SoundSourceLookup* OpenALContextImpl::AddSoundSourceLookup(size_t& count, const SoundFormat& soundFormat, size_t maxVoiceCount, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Exit if there aren't any more slots left
    if (count >= this->soundSourceLookups.size())
        return nullptr;

    //Add new lookup item
    auto newSoundSourceIndex = count++;
    auto soundSourceLookup = &this->soundSourceLookups[newSoundSourceIndex];

    //Initialize
    soundSourceLookup->format = soundFormat;
    if (!soundSourceLookup->sourceVoices.CreateEmpty(maxVoiceCount, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create sound source voice lookup for format.");
        return nullptr;
    }

    if (!soundSourceLookup->sources.CreateEmpty(maxVoiceCount, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create sound source lookup for format.");
        return nullptr;
    }

    if (!soundSourceLookup->playingList.CreateEmpty(maxVoiceCount, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create playing list for for format.");
        return nullptr;
    }

    return soundSourceLookup;
}

OpenALSoundBuffer* OpenALContextImpl::CreateBuffer(const ByteBuffer& bytes, Error& error)
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

OpenALSoundSource* OpenALContextImpl::CreateSource(const SoundFormat& soundFormat, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto soundSourceLookup = this->GetSoundSourceLookup(soundFormat);
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
