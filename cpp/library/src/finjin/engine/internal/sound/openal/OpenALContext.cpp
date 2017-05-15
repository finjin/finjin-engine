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
#include "OpenALContext.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "OpenALContextImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
OpenALContext::OpenALContext(Allocator* allocator, OpenALSystem* soundSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<OpenALContextImpl>(allocator, FINJIN_CALLER_ARGUMENTS, soundSystem))
{
}

OpenALContext::~OpenALContext()
{
}

void OpenALContext::Create(const Settings& settings, Error& error)
{
    assert(settings.assetFileReader != nullptr);

    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    //Copy and validate settings-------------------------------
    impl->settings = settings;

    if (!impl->readBuffer.Create(impl->settings.tempReadBufferSize, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create temp read buffer");
        return;
    }

    impl->settingsAssetFileReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::SETTINGS, impl->GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create settings asset file reader.");
        return;
    }

    for (auto& settingsFileName : impl->settings.settingsFileNames)
    {
        impl->settings.ReadSettings(impl->settingsAssetFileReader, settingsFileName, impl->readBuffer, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There was an error reading the sound context '%1%' settings file.", settingsFileName.ToUriString()));
            return;
        }
    }

    auto totalSoundSourceCount = impl->settings.GetTotalSoundSourceCount();
    if (totalSoundSourceCount == 0)
    {
        FINJIN_SET_ERROR(error, "A zero sound source count was specified.");
        return;
    }

    if (impl->settings.maxSoundBufferCount == 0)
    {
        FINJIN_SET_ERROR(error, "A zero sound buffer count was specified.");
        return;
    }

    //Create engine-------------------
    impl->aldevice = alcOpenDevice(!impl->settings.deviceID.empty() ? impl->settings.deviceID.c_str() : nullptr);

    //Get device attributes
    ALCint almaxMonoSourceCount = 0;
    ALCint almaxStereoSourceCount = 0;
    {
        ALCint attributeCount;
        alcGetIntegerv(impl->aldevice, ALC_ATTRIBUTES_SIZE, 1, &attributeCount);
        if (attributeCount > 0)
        {
            StaticVector<ALCint, 64> attrs;
            attrs.resize(static_cast<size_t>(attributeCount));
            alcGetIntegerv(impl->aldevice, ALC_ALL_ATTRIBUTES, static_cast<ALCint>(attrs.size()), &attrs[0]);
            for (size_t i = 0; i < attrs.size(); i += 2)
            {
                switch (attrs[i])
                {
                    case ALC_MONO_SOURCES: almaxMonoSourceCount = attrs[i + 1]; break;
                    case ALC_STEREO_SOURCES: almaxStereoSourceCount = attrs[i + 1]; break;
                    case ALC_FREQUENCY:
                    {
                        if (impl->settings.playbackSampleRate == 0)
                            impl->settings.playbackSampleRate = attrs[i + 1];
                        break;
                    }
                    case ALC_REFRESH:
                    {
                        //Ignore
                        break;
                    }
                    case ALC_SYNC:
                    {
                        //Ignore
                        break;
                    }
                }
            }
        }
    }

    auto maxSimultaneousChannels = std::numeric_limits<size_t>::max();
    if (almaxMonoSourceCount > 0)
        maxSimultaneousChannels = almaxMonoSourceCount; //Ignore the stereo count

    //Create context
    {
        //Create the attributes list for initialization
        StaticVector<int, 20> attributes;
        {
            attributes.push_back(ALC_MONO_SOURCES);
            attributes.push_back(std::min(almaxMonoSourceCount, FINJIN_OPENAL_MAX_SOURCE_IDS));

            //attributes.push_back(ALC_STEREO_SOURCES);
            //attributes.push_back(???);

            attributes.push_back(ALC_MAX_AUXILIARY_SENDS);
            attributes.push_back(8); //Creative Labs X-Fi has 4, so this seems a reasonable upper limit

            if (impl->settings.playbackSampleRate != 0)
            {
                attributes.push_back(ALC_FREQUENCY);
                attributes.push_back(impl->settings.playbackSampleRate);
            }

            //Terminate list
            attributes.push_back(0);
        }

        impl->alcontext = alcCreateContext(impl->aldevice, attributes.data());
        if (impl->alcontext == nullptr || alcGetError(impl->aldevice) != ALC_NO_ERROR)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Couldn't create sound context.");
            return;
        }
        alcMakeContextCurrent(impl->alcontext);
    }

    //Set up speed of sound. This never changes
    alSpeedOfSound(impl->settings.speedOfSound.ToMeters());

    //Initialize extensions
    {
        //auto extensionsText = alGetString(AL_EXTENSIONS);
        //std::cout << "Extensions: " << extensionsText << std::endl;

        if (AnySet(impl->settings.requestedExtensions & OpenALKnownExtension::EFX) && impl->efxExtensions.Initialize(impl->aldevice))
            impl->extensions |= OpenALKnownExtension::EFX;

        if (AnySet(impl->settings.requestedExtensions & OpenALKnownExtension::EXTENDED_CHANNEL_FORMATS) && impl->extendedChannelFormats.Initialize(impl->aldevice))
            impl->extensions |= OpenALKnownExtension::EXTENDED_CHANNEL_FORMATS;
    }

    //Set up for EFX if possible
    if (AnySet(impl->extensions & OpenALKnownExtension::EFX))
    {
        //Get the maximum number of auxiliary sends for each sound source
        alcGetIntegerv(impl->aldevice, ALC_MAX_AUXILIARY_SENDS, 1, &impl->maxAuxSendsPerSource);

        //std::cout << "Max aux sends per source: " << impl->maxAuxSendsPerSource << std::endl;

        //Generate as many effect slots as possible
        ALuint effectSlotID;
        for (size_t i = 0; i < !impl->effectSlots.full(); i++)
        {
            impl->efxExtensions.alGenAuxiliaryEffectSlots(1, &effectSlotID);
            if (alGetError() != AL_NO_ERROR)
                break;

            //std::cout << "Generated effect slot" << std::endl;

            auto effectSlotIndex = impl->effectSlots.size();
            impl->effectSlots.push_back();
            impl->effectSlots[effectSlotIndex].Create(this, effectSlotID);
        }
    }

    //Listener
    impl->listener.Create(this);

    //Master sound source group
    impl->masterSoundGroup.Create(this);

    //Preallocate various things---------------------------------

    //Sound IDs
    impl->sources.Create(totalSoundSourceCount, GetAllocator());
    for (auto& source : impl->sources)
        source.Create(this);

    //Generate as many sound source IDs as possible
    ALuint sourceID;
    for (size_t i = 0; i < maxSimultaneousChannels && !impl->sourceIDs.full(); i++)
    {
        alGenSources(1, &sourceID);
        if (alGetError() != AL_NO_ERROR)
            break;
        impl->sourceIDs.push_back(sourceID);
    }

    //std::cout << "Detected mono sources: " << impl->sourceIDs.size() << std::endl;

    size_t sourceOffset = 0;
    size_t usedSourceIDCount = 0;

    //Sound source lookups
    {
        OpenALSourceVoice sourceVoice;

        size_t lookupCount = 0;
        impl->soundSourceLookups.Create(impl->settings.soundFormats.size(), GetAllocator());
        for (size_t soundSourceDescriptorIndex = 0; soundSourceDescriptorIndex < impl->settings.soundSourcePools.size(); soundSourceDescriptorIndex++)
        {
            auto& soundSourceDescriptor = impl->settings.soundSourcePools[soundSourceDescriptorIndex];

            //Get or add the lookup
            auto soundSourceLookup = impl->GetSoundSourceLookup(impl->settings.soundFormats[soundSourceDescriptor.format]);
            if (soundSourceLookup == nullptr)
            {
                soundSourceLookup = impl->AddSoundSourceLookup(lookupCount, impl->settings.soundFormats[soundSourceDescriptor.format], impl->settings.GetTotalSoundSourceCount(impl->settings.soundFormats[soundSourceDescriptor.format]), error);
                if (error)
                {
                    Destroy();

                    FINJIN_SET_ERROR(error, "Failed to add sound source lookup.");
                    return;
                }
            }
            if (soundSourceLookup != nullptr)
            {
                //Try to create "soundSourceDescriptor.count" voices, each of which requires "soundSourceLookup->format.channelCount" sourceIDs
                for (size_t sourceIndex = 0;
                    sourceIndex < soundSourceDescriptor.count && (impl->sourceIDs.size() - usedSourceIDCount) >= impl->settings.soundFormats[soundSourceDescriptor.format].channelCount;
                    sourceIndex++)
                {
                    sourceVoice.assign(&impl->sourceIDs[usedSourceIDCount], impl->settings.soundFormats[soundSourceDescriptor.format].channelCount);
                    usedSourceIDCount += impl->settings.soundFormats[soundSourceDescriptor.format].channelCount;

                    soundSourceLookup->sourceVoices.items.push_back(sourceVoice);
                }

                //Add "soundSourceDescriptor.count" sources
                for (size_t sourceIndex = 0; sourceIndex < soundSourceDescriptor.count; sourceIndex++)
                {
                    auto source = &impl->sources[sourceOffset++];
                    source->SetSoundFormat(impl->settings.soundFormats[soundSourceDescriptor.format]);
                    soundSourceLookup->sources.items.push_back(source);
                }
            }
        }
    }

    //Sound buffers
    if (!impl->buffers.Create(impl->settings.maxSoundBufferCount, GetAllocator()))
    {
        Destroy();

        FINJIN_SET_ERROR(error, "Failed to create sound buffers lookup.");
        return;
    }
    for (auto& buffer : impl->buffers.items)
        buffer.Create(this);

    impl->lowPassFilter.Create(this);
    //impl->lowPassFilter.SetGain(.25f);
    //impl->lowPassFilter.SetGainHighFrequency(.25f);

    impl->highPassFilter.Create(this);
    //impl->highPassFilter.SetGain(.25f);
    //impl->highPassFilter.SetGainLowFrequency(.25f);

    impl->bandPassFilter.Create(this);
}

void OpenALContext::Destroy()
{
    //Delete sources
    impl->sources.Destroy();
    for (size_t i = 0; i < impl->sourceIDs.size(); i++)
        alDeleteSources(1, &impl->sourceIDs[i]);
    impl->sourceIDs.clear();
    impl->soundSourceLookups.Destroy();

    //Delete buffers
    for (auto& buffer : impl->buffers.items)
    {
        if (buffer.bufferID.IsSet())
        {
            alDeleteBuffers(1, &buffer.bufferID.value);
            buffer.bufferID.Reset(0);
        }
    }
    impl->buffers.Destroy();

    //Delete effect slots
    for (auto& effectSlot : impl->effectSlots)
    {
        if (effectSlot.effectSlotID == AL_EFFECTSLOT_NULL)
            break;

        impl->efxExtensions.alDeleteAuxiliaryEffectSlots(1, &effectSlot.effectSlotID);
        effectSlot.effectSlotID = AL_EFFECTSLOT_NULL;
    }
    impl->effectSlots.clear();

    //Delete filters
    impl->DestroyFilter(&impl->lowPassFilter, true);
    impl->DestroyFilter(&impl->highPassFilter, true);
    impl->DestroyFilter(&impl->bandPassFilter, true);

    //Delete context
    if (impl->alcontext != nullptr)
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(impl->alcontext);
        impl->alcontext = nullptr;
    }

    //Delete device
    if (impl->aldevice != nullptr)
    {
        alcCloseDevice(impl->aldevice);
        impl->aldevice = nullptr;
    }
}

void OpenALContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const OpenALContext::Settings& OpenALContext::GetSettings() const
{
    return impl->settings;
}

size_t OpenALContext::GetExternalAssetFileExtensions(StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS>& extensions, AssetClass assetClass, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    size_t count = 0;

    switch (assetClass)
    {
        case AssetClass::SOUND:
        {
            for (auto ext : { "wav" })
            {
                if (extensions.push_back(ext).HasErrorOrValue(false))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' extension.", ext));
                    return count;
                }
                count++;
            }
            break;
        }
        default: break;
    }

    return count;
}

void OpenALContext::HandleApplicationViewportLostFocus()
{
}

void OpenALContext::HandleApplicationViewportGainedFocus()
{
}

void OpenALContext::Update(SimpleTimeDelta elapsedTime)
{
    PriorityListenerDistanceSoundSorter<OpenALSoundSource> priorityListenerDistanceSoundSorter;
    auto soundSorter = (impl->soundSorter != nullptr) ? impl->soundSorter : &priorityListenerDistanceSoundSorter;
    {
        MathVector3 position;
        impl->listener.GetPosition(position);
        soundSorter->SetListener(position);
    }

    //Update listener
    impl->listener.Commit();

    //Process each lookup
    for (auto& soundSourceLookup : impl->soundSourceLookups)
    {
        //Figure out playing list
        soundSourceLookup.playingList.clear();
        for (auto source : soundSourceLookup.sources.usedItems)
        {
            source->Update(elapsedTime);

            if (AnySet(source->GetState() & SoundSourceState::PLAYING))
                soundSourceLookup.playingList.push_back(source);
        }

        //Sort playing list
        if (impl->soundSortingEnabled)
        {
            std::stable_sort(soundSourceLookup.playingList.begin(), soundSourceLookup.playingList.end(), SoundSorterProxy<OpenALSoundSource>(soundSorter));
        }

        //Match up the sound sources with voices to play the sounds
        {
            //Pop sources off the back of the playing list until there are enough voices for the playing sources
            while (soundSourceLookup.playingList.size() > soundSourceLookup.sourceVoices.items.size() && !soundSourceLookup.playingList.empty())
            {
                auto source = soundSourceLookup.playingList.back();
                soundSourceLookup.playingList.pop_back();

                soundSourceLookup.Virtualize(source);
            }

            //The playing list is now composed of all the sounds that should have voices assigned
            for (auto source : soundSourceLookup.playingList)
            {
                auto isNewRealize = soundSourceLookup.Realize(source);
                source->Commit(isNewRealize);
            }
        }
    }

    //Update filters and effects
    impl->lowPassFilter.Commit();
    impl->highPassFilter.Commit();
    impl->bandPassFilter.Commit();

    impl->invalidated = SoundContextChange::NONE;
}

void OpenALContext::Execute(SoundEvents& events, SoundCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
}

OpenALListener* OpenALContext::GetListener()
{
    return &impl->listener;
}

OpenALSoundGroup* OpenALContext::GetMasterGroup()
{
    return &impl->masterSoundGroup;
}

/*OpenALSoundBuffer* CreateBuffer(const AssetReference& assetRef, Error& error);
OpenALSoundBuffer* OpenALContext::CreateBuffer(const AssetReference& assetRef, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto item = impl->CreateBuffer(assetRef, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);

    return item;
}*/

void OpenALContext::DestroyBuffer(OpenALSoundBuffer* buffer, bool shuttingDown)
{
    if (!shuttingDown)
    {
        if (impl->buffers.IsUsed(buffer))
        {
            while (buffer->sourcesHead != nullptr)
                buffer->sourcesHead->SetBuffer(nullptr);

            impl->buffers.Unuse(buffer);
        }
    }
}

OpenALSoundSource* OpenALContext::CreateSource(OpenALSoundBuffer* soundBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto source = impl->CreateSource(soundBuffer->GetSoundFormat(), error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    source->SetBuffer(soundBuffer);
    return source;
}

OpenALSoundSource* OpenALContext::CreateSource(const SoundFormat& soundFormat, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto source = impl->CreateSource(soundFormat, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);

    return source;
}

void OpenALContext::DestroySource(OpenALSoundSource* source, bool shuttingDown)
{
    auto soundSourceLookup = impl->GetSoundSourceLookup(source->GetSoundFormat());
    if (soundSourceLookup != nullptr)
    {
        if (soundSourceLookup->sources.IsUsed(source))
        {
            soundSourceLookup->Virtualize(source);

            source->SetBuffer(nullptr);

            source->SetGroup(&impl->masterSoundGroup);

            auto filter = source->GetFilter();
            if (filter != nullptr)
                filter->RemoveSource(source);

            for (size_t i = 0; i < source->GetEffectSlotCount(); i++)
            {
                auto filter = source->GetEffectSlotFilter(i);
                if (filter != nullptr)
                    filter->RemoveSource(source);
            }

            soundSourceLookup->sources.Unuse(source);
        }
    }
}

SoundSorter<OpenALSoundSource>* OpenALContext::GetSoundSorter()
{
    return impl->soundSorter;
}

void OpenALContext::SetSoundSorter(SoundSorter<OpenALSoundSource>* soundSorter)
{
    impl->soundSorter = soundSorter;
}

bool OpenALContext::IsSoundSortingEnabled() const
{
    return impl->soundSortingEnabled;
}

void OpenALContext::EnableSoundSorting(bool enable)
{
    impl->soundSortingEnabled = enable;
}

float OpenALContext::GetDopplerFactor() const
{
    ALfloat(factor);
    alGetFloatv(AL_DOPPLER_FACTOR, &factor);
    return factor;
}

void OpenALContext::SetDopplerFactor(float factor)
{
    alDopplerFactor(factor);
}

float OpenALContext::GetVolume() const
{
    ALfloat gain;
    alGetListenerf(AL_GAIN, &gain);
    return gain;
}

void OpenALContext::SetVolume(float volume)
{
    Limit(volume, 0.0f, 1.0f);
    alListenerf(AL_GAIN, volume);
}

float OpenALContext::GetMetersPerUnit() const
{
    return impl->metersPerUnit;
}

void OpenALContext::SetMetersPerUnit(float metersPerUnit)
{
    impl->metersPerUnit = metersPerUnit <= 0 ? 1 : metersPerUnit;
    if (AnySet(impl->extensions & OpenALKnownExtension::EFX))
        alListenerf(AL_METERS_PER_UNIT, impl->metersPerUnit);
}

float OpenALContext::GetReferenceDistance() const
{
    return impl->referenceDistance;
}

void OpenALContext::SetReferenceDistance(float distance)
{
    if (distance < 0)
        distance = 0;
    if (impl->referenceDistance != distance)
    {
        impl->referenceDistance = distance;
        impl->invalidated |= SoundContextChange::REFERENCE_DISTANCE;
    }
}

float OpenALContext::GetMaxDistance() const
{
    return impl->maxDistance;
}

void OpenALContext::SetMaxDistance(float distance)
{
    if (distance < 0)
        distance = 0;
    if (impl->maxDistance != distance)
    {
        impl->maxDistance = distance;
        impl->invalidated |= SoundContextChange::MAX_DISTANCE;
    }
}

float OpenALContext::GetRolloffFactor() const
{
    return impl->rolloffFactor;
}

void OpenALContext::SetRolloffFactor(float factor)
{
    Limit(factor, 0.0f, 100.0f);
    if (impl->rolloffFactor != factor)
    {
        impl->rolloffFactor = factor;
        impl->invalidated |= SoundContextChange::ROLLOFF_FACTOR;
    }
}

SoundDistanceModel OpenALContext::GetDistanceModel() const
{
    int model;
    alGetIntegerv(AL_DISTANCE_MODEL, &model);
    switch (model)
    {
        case AL_INVERSE_DISTANCE: return SoundDistanceModel::INVERSE_DISTANCE_CLAMPED; //Should not happen
        case AL_INVERSE_DISTANCE_CLAMPED: return SoundDistanceModel::INVERSE_DISTANCE_CLAMPED;
        case AL_LINEAR_DISTANCE: return SoundDistanceModel::LINEAR_DISTANCE_CLAMPED; //Should not happen
        case AL_LINEAR_DISTANCE_CLAMPED: return SoundDistanceModel::LINEAR_DISTANCE_CLAMPED;
        case AL_EXPONENT_DISTANCE: return SoundDistanceModel::EXPONENT_DISTANCE_CLAMPED; //Should not happen
        case AL_EXPONENT_DISTANCE_CLAMPED: return SoundDistanceModel::EXPONENT_DISTANCE_CLAMPED;
        default: return SoundDistanceModel::EXPONENT_DISTANCE_CLAMPED;
    }
}

void OpenALContext::SetDistanceModel(SoundDistanceModel model)
{
    switch (model)
    {
        case SoundDistanceModel::INVERSE_DISTANCE_CLAMPED: alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED); break;
        case SoundDistanceModel::LINEAR_DISTANCE_CLAMPED: alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED); break;
        case SoundDistanceModel::EXPONENT_DISTANCE_CLAMPED: alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED); break;
    }
}

OpenALContextImpl* OpenALContext::GetImpl()
{
    return impl.get();
}

void OpenALContext::AddSourceLowPassFilter(OpenALSoundSource* source)
{
    source->SetFilter(&impl->lowPassFilter);
}

void OpenALContext::AddSourceHighPassFilter(OpenALSoundSource* source)
{
    source->SetFilter(&impl->highPassFilter);
}

void OpenALContext::AddSourceBandPassFilter(OpenALSoundSource* source)
{
    source->SetFilter(&impl->bandPassFilter);
}

size_t OpenALContext::GetMaxSourceChannelCount() const
{
    //Return the greater of 2 (for stereo) or the extended channel format with the largest channel count
    return std::max((size_t)2, impl->extendedChannelFormats.GetMaxChannelCount());
}

size_t OpenALContext::GetEffectSlotCount() const
{
    return impl->effectSlots.size();
}

OpenALEffectSlot* OpenALContext::GetEffectSlot(size_t index)
{
    return &impl->effectSlots[index];
}

bool OpenALContext::HasExtension(OpenALKnownExtension extension) const
{
    return AnySet(impl->extensions & extension);
}

size_t OpenALContext::GetMaxAuxSendsPerSource() const
{
    return impl->maxAuxSendsPerSource;
}

ALenum OpenALContext::SoundFormatToEnum(const SoundFormat& soundFormat)
{
    if (soundFormat.sampleType != SoundFormat::SampleType::INTEGER)
        return -1;

    if (soundFormat.channelCount == 1 && soundFormat.bytesPerChannel == 1)
        return AL_FORMAT_MONO8;
    else if (soundFormat.channelCount == 1 && soundFormat.bytesPerChannel == 2)
        return AL_FORMAT_MONO16;
    else if (soundFormat.channelCount == 2 && soundFormat.bytesPerChannel == 1)
        return AL_FORMAT_STEREO8;
    else if (soundFormat.channelCount == 2 && soundFormat.bytesPerChannel == 2)
        return AL_FORMAT_STEREO16;
    else if (soundFormat.channelCount == 4 && soundFormat.bytesPerChannel == 1)
        return impl->extendedChannelFormats.AL_FORMAT_QUAD8;
    else if (soundFormat.channelCount == 4 && soundFormat.bytesPerChannel == 2)
        return impl->extendedChannelFormats.AL_FORMAT_QUAD16;
    else if (soundFormat.channelCount == 6 && soundFormat.bytesPerChannel == 1)
        return impl->extendedChannelFormats.AL_FORMAT_51CHN8;
    else if (soundFormat.channelCount == 6 && soundFormat.bytesPerChannel == 2)
        return impl->extendedChannelFormats.AL_FORMAT_51CHN16;
    else if (soundFormat.channelCount == 7 && soundFormat.bytesPerChannel == 1)
        return impl->extendedChannelFormats.AL_FORMAT_61CHN8;
    else if (soundFormat.channelCount == 7 && soundFormat.bytesPerChannel == 2)
        return impl->extendedChannelFormats.AL_FORMAT_61CHN16;
    else if (soundFormat.channelCount == 8 && soundFormat.bytesPerChannel == 1)
        return impl->extendedChannelFormats.AL_FORMAT_71CHN8;
    else if (soundFormat.channelCount == 8 && soundFormat.bytesPerChannel == 2)
        return impl->extendedChannelFormats.AL_FORMAT_71CHN16;

    return -1;
}

const OpenALEFXExtensions& OpenALContext::GetEfxExtensions() const
{
    return impl->efxExtensions;
}
