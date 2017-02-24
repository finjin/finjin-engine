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
#include "XAudio2Context.hpp"
#include "XAudio2ContextImpl.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"

using namespace Finjin::Engine;


//Local functions--------------------------------------------------------------
static DWORD SpeakerConfigurationToXAudio(SoundSpeakerSetup speakerSetup)
{
    switch (speakerSetup)
    {
        case SoundSpeakerSetup::MONO: return SPEAKER_MONO;
        case SoundSpeakerSetup::STEREO: return SPEAKER_STEREO;
        case SoundSpeakerSetup::STEREO_PLUS: return SPEAKER_2POINT1;
        case SoundSpeakerSetup::SURROUND_4: return SPEAKER_QUAD;
        case SoundSpeakerSetup::SURROUND_4_1: return SPEAKER_4POINT1;
        case SoundSpeakerSetup::SURROUND_5_1: return SPEAKER_5POINT1;
        case SoundSpeakerSetup::SURROUND_7_1: return SPEAKER_7POINT1;
        default: return SPEAKER_STEREO;
    }
}


//Implementation---------------------------------------------------------------
XAudio2Context::XAudio2Context(Allocator* allocator, XAudio2System* soundSystem) : 
    AllocatedClass(allocator), 
    impl(AllocatedClass::New<XAudio2ContextImpl>(allocator, FINJIN_CALLER_ARGUMENTS, soundSystem))
{
}

XAudio2Context::~XAudio2Context()
{
}

void XAudio2Context::Create(const Settings& settings, Error& error)
{
    assert(settings.assetFileReader != nullptr);

    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->initializationStatus.SetStatus(OperationStatus::STARTED);

    //Copy and validate settings-------------------------------
    impl->settings = settings;

    if (!impl->readBuffer.Create(impl->settings.tempReadBufferSize, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create temp read buffer");
        return;
    }

    impl->settingsAssetFileReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::SETTINGS, GetAllocator(), error);
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
        impl->initializationStatus.SetStatus(OperationStatus::FAILURE);
        
        FINJIN_SET_ERROR(error, "A zero sound source count was specified.");
        return;
    }

    if (impl->settings.maxSoundBufferCount == 0)
    {
        impl->initializationStatus.SetStatus(OperationStatus::FAILURE);
        
        FINJIN_SET_ERROR(error, "A zero sound buffer count was specified.");
        return;
    }

    //Create engine--------------------
    UINT32 flags = 0;
#if FINJIN_DEBUG
    flags |= XAUDIO2_DEBUG_ENGINE;
#endif

    XAUDIO2_PROCESSOR processor = XAUDIO2_DEFAULT_PROCESSOR;
    if (settings.processorID != 0)
        processor = static_cast<XAUDIO2_PROCESSOR>(1 << settings.processorID);
    if (FAILED(XAudio2Create(&impl->xaudioInterface, flags, processor)))
    {
        flags &= ~XAUDIO2_DEBUG_ENGINE;
        if (FAILED(XAudio2Create(&impl->xaudioInterface, flags, processor)))
        {
            impl->initializationStatus.SetStatus(OperationStatus::FAILURE);
            
            FINJIN_SET_ERROR(error, "Failed to create XAudio2 interface.");
            return;
        }        
    }

    if (flags & XAUDIO2_DEBUG_ENGINE)
    {
        XAUDIO2_DEBUG_CONFIGURATION debugConfig = {};
        debugConfig.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
        impl->xaudioInterface->SetDebugConfiguration(&debugConfig, 0);
    }
    
    //Create mastering voice
    UINT32 voiceInputChannelCount = XAUDIO2_DEFAULT_CHANNELS;
    LPCWSTR deviceID = nullptr; //Use null so that the system uses a virtual audio client
    UINT32 deviceIndex = 0;
    const XAUDIO2_EFFECT_CHAIN* voiceEffectChain = nullptr;
    UINT32 masteringVoiceFlags = 0;
#if _WIN32_WINNT <= _WIN32_WINNT_WIN7
    if (FAILED(impl->xaudioInterface->CreateMasteringVoice(&impl->masteringVoice, voiceInputChannelCount, impl->settings.playbackSampleRate, masteringVoiceFlags, deviceIndex, voiceEffectChain)))
#else
    AUDIO_STREAM_CATEGORY voiceStreamCategory = AudioCategory_GameEffects;
    if (FAILED(impl->xaudioInterface->CreateMasteringVoice(&impl->masteringVoice, voiceInputChannelCount, impl->settings.playbackSampleRate, masteringVoiceFlags, deviceID, voiceEffectChain, voiceStreamCategory)))
#endif
    {
        impl->initializationStatus.SetStatus(OperationStatus::FAILURE);
        
        FINJIN_SET_ERROR(error, "Failed to create XAudio2 mastering voice interface.");
        return;
    }

    //Create 3D stuff
    DWORD speakerChannelMask = 0;
    if (impl->settings.speakerSetup == SoundSpeakerSetup::DEFAULT)
    {
    #if _WIN32_WINNT >= _WIN32_WINNT_WIN8
        impl->masteringVoice->GetChannelMask(&speakerChannelMask);
    #else
        XAUDIO2_DEVICE_DETAILS deviceDetails = {};
        impl->xaudioInterface->GetDeviceDetails(deviceIndex, &deviceDetails);
        speakerChannelMask = deviceDetails.OutputFormat.dwChannelMask;
    #endif
    }
    else
        speakerChannelMask = SpeakerConfigurationToXAudio(impl->settings.speakerSetup);
    X3DAudioInitialize(speakerChannelMask, impl->settings.speedOfSound.ToMeters(), impl->x3dAudioInterface);

    XAUDIO2_VOICE_DETAILS details = {};
    impl->masteringVoice->GetVoiceDetails(&details);
    impl->masteringVoiceChannelCount = details.InputChannels;

    if (impl->settings.playbackSampleRate == 0)
        impl->settings.playbackSampleRate = details.InputSampleRate;

    //Listener
    impl->listener.Create(this);

    //Master sound source group
    impl->masterSoundGroup.Create(this);

    //Preallocate various things------------------

    //Sound sources
    if (!impl->sources.Create(totalSoundSourceCount, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create sound sources.");
        return;
    }
    for (auto& source : impl->sources)
        source.Create(this);

    size_t sourceOffset = 0;
    
    //Sound source lookups
    impl->soundSourceLookups.CreateEmpty(impl->settings.soundFormats.size(), GetAllocator());
    for (size_t soundSourceDescriptorIndex = 0; soundSourceDescriptorIndex < impl->settings.soundSourcePools.size(); soundSourceDescriptorIndex++)
    {
        auto& soundSourceDescriptor = impl->settings.soundSourcePools[soundSourceDescriptorIndex];

        //Get or add the lookup
        auto soundSourceLookup = impl->GetSoundSourceLookup(impl->settings.soundFormats[soundSourceDescriptor.format]);
        if (soundSourceLookup == nullptr)
        {
            soundSourceLookup = impl->AddSoundSourceLookup(impl->settings.soundFormats[soundSourceDescriptor.format], impl->settings.GetTotalSoundSourceCount(impl->settings.soundFormats[soundSourceDescriptor.format]), error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to add sound source lookup.");
                return;
            }
        }

        if (soundSourceLookup != nullptr)
        {
            //Try to create "soundSourceDescriptor.count" voices
            for (size_t sourceIndex = 0; sourceIndex < soundSourceDescriptor.count; sourceIndex++)
            {
                XAudio2SoundSourceVoice sourceVoice;
                if (impl->CreateSourceVoice(sourceVoice, soundSourceLookup->format))
                    soundSourceLookup->sourceVoices.items.push_back(sourceVoice);
                else
                    break;
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
    
    //Sound buffers
    if (!impl->buffers.Create(impl->settings.maxSoundBufferCount, GetAllocator()))
    {
        impl->initializationStatus.SetStatus(OperationStatus::FAILURE);

        FINJIN_SET_ERROR(error, "Failed to create sound buffers.");
        return;
    }
    for (auto& buffer : impl->buffers.items)
        buffer.Create(this);

    impl->initializationStatus.SetStatus(OperationStatus::SUCCESS);
}

void XAudio2Context::Destroy()
{   
    impl->sources.Destroy();
    for (auto& soundSourceLookup : impl->soundSourceLookups)
    {
        soundSourceLookup.sourceVoices.ClearUsed();
        soundSourceLookup.sources.ClearUsed();

        for (auto& sourceVoice : soundSourceLookup.sourceVoices.items)
        {
            sourceVoice.xaudioSourceVoice->DestroyVoice();
            sourceVoice.xaudioSourceVoice = nullptr;
        }
    }    
    impl->soundSourceLookups.Destroy();
        
    impl->buffers.Destroy();
    
    if (impl->masteringVoice != nullptr)
    {
        impl->masteringVoice->DestroyVoice();
        impl->masteringVoice = nullptr;
    }

    if (impl->xaudioInterface != nullptr)
    {
        impl->xaudioInterface->Release();
        impl->xaudioInterface = nullptr;
    }
}

const OperationStatus& XAudio2Context::GetInitializationStatus() const
{
    return impl->initializationStatus;
}

void XAudio2Context::GetSelectorComponents(AssetPathSelector& result)
{
}

const XAudio2Context::Settings& XAudio2Context::GetSettings() const
{
    return impl->settings;
}

AssetCreationCapability XAudio2Context::GetAssetCreationCapabilities(AssetClass assetClass) const
{
    return AssetCreationCapability::MAIN_THREAD;
}

void XAudio2Context::Update(SimpleTimeDelta elapsedTime)
{
    impl->listener.Commit();

    PriorityListenerDistanceSoundSorter<XAudio2SoundSource> priorityListenerDistanceSoundSorter;
    auto soundSorter = (impl->soundSorter != nullptr) ? impl->soundSorter : &priorityListenerDistanceSoundSorter;
    {
        float x, y, z;
        impl->listener.GetPosition(x, y, z);
        soundSorter->SetListener(x, y, z);
    }

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
            std::stable_sort(soundSourceLookup.playingList.begin(), soundSourceLookup.playingList.end(), SoundSorterProxy<XAudio2SoundSource>(soundSorter));
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
}

void XAudio2Context::Execute(SoundEvents& events, SoundCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    for (auto& command : commands)
    {
        switch (command.type)
        {
            case SoundCommand::Type::NOTIFY:
            {
                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'notify' sound event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = SoundEvent::Type::NOTIFY;
                    event.eventInfo = command.eventInfo;
                }

                break;
            }
            case SoundCommand::Type::CREATE_STATIC_BUFFER:
            {
                auto sound = static_cast<FinjinSound*>(command.staticSound);
                assert(sound != nullptr);
                auto soundBuffer = impl->CreateBuffer(sound->fileBytes, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to create static sound buffer.");
                    return;
                }

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'create static buffer' sound event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = SoundEvent::Type::CREATED_STATIC_BUFFER;
                    event.eventInfo = command.eventInfo;
                    event.soundBuffer = soundBuffer;
                }

                break;
            }
            case SoundCommand::Type::DESTROY_STATIC_BUFFER:
            {
                auto soundBuffer = static_cast<XAudio2SoundBuffer*>(command.soundBuffer);
                assert(soundBuffer != nullptr);
                DestroyBuffer(soundBuffer);
                break;
            }
            case SoundCommand::Type::CREATE_SOURCE:
            {
                auto soundBuffer = static_cast<XAudio2SoundBuffer*>(command.soundBuffer);
                assert(soundBuffer != nullptr);
                auto soundSource = CreateSource(soundBuffer, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to create sound source.");
                    return;
                }

                if (command.eventInfo.HasEventID())
                {
                    if (!events.push_back())
                    {
                        FINJIN_SET_ERROR(error, "Failed to record 'create source' sound event.");
                        return;
                    }

                    auto& event = events.back();
                    event.type = SoundEvent::Type::CREATED_SOURCE;
                    event.eventInfo = command.eventInfo;
                    event.soundSource = soundSource;
                }

                break;
            }
            case SoundCommand::Type::DESTROY_SOURCE:
            {
                auto soundSource = static_cast<XAudio2SoundSource*>(command.soundSource);
                assert(soundSource != nullptr);
                DestroySource(soundSource);
                break;
            }
            case SoundCommand::Type::PLAY_SOURCE:
            {
                auto soundSource = static_cast<XAudio2SoundSource*>(command.soundSource);
                assert(soundSource != nullptr);
                soundSource->Play();
                break;
            }
            case SoundCommand::Type::PAUSE_SOURCE:
            {
                auto soundSource = static_cast<XAudio2SoundSource*>(command.soundSource);
                assert(soundSource != nullptr);
                soundSource->Pause();
                break;
            }
            case SoundCommand::Type::STOP_SOURCE:
            {
                auto soundSource = static_cast<XAudio2SoundSource*>(command.soundSource);
                assert(soundSource != nullptr);
                soundSource->Stop();
                break;
            }
        }
    }
}

void XAudio2Context::HandleApplicationViewportLostFocus()
{
}

void XAudio2Context::HandleApplicationViewportGainedFocus()
{
}

XAudio2Listener* XAudio2Context::GetListener()
{
    return &impl->listener;
}

XAudio2SoundGroup* XAudio2Context::GetMasterGroup()
{
    return &impl->masterSoundGroup;
}

XAudio2SoundBuffer* XAudio2Context::CreateBufferFromMainThread(const ByteBuffer& fileBytes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto soundBuffer = impl->CreateBuffer(fileBytes, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to create static sound buffer.");

    return soundBuffer;
}

void XAudio2Context::DestroyBuffer(XAudio2SoundBuffer* buffer)
{
    if (impl->buffers.IsUsed(buffer))
    {
        while (buffer->sourcesHead != nullptr)
            buffer->sourcesHead->SetBuffer(nullptr);

        impl->buffers.Unuse(buffer);
    }
}

XAudio2SoundSource* XAudio2Context::CreateSource(XAudio2SoundBuffer* soundBuffer, Error& error)
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

XAudio2SoundSource* XAudio2Context::CreateSource(const SoundFormat& soundFormat, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto source = impl->CreateSource(soundFormat, error);
    if (error)
        FINJIN_SET_ERROR_NO_MESSAGE(error);

    return source;
}

void XAudio2Context::DestroySource(XAudio2SoundSource* source)
{
    auto soundSourceLookup = impl->GetSoundSourceLookup(source->GetSoundFormat());
    if (soundSourceLookup != nullptr)
    {
        if (soundSourceLookup->sources.IsUsed(source))
        {
            soundSourceLookup->Virtualize(source);

            source->SetBuffer(nullptr);

            source->SetGroup(&impl->masterSoundGroup);

            soundSourceLookup->sources.Unuse(source);
        }
    }
}

SoundSorter<XAudio2SoundSource>* XAudio2Context::GetSoundSorter()
{
    return impl->soundSorter;
}

void XAudio2Context::SetSoundSorter(SoundSorter<XAudio2SoundSource>* soundSorter)
{
    impl->soundSorter = soundSorter;
}

bool XAudio2Context::IsSoundSortingEnabled() const
{
    return impl->soundSortingEnabled;
}

void XAudio2Context::EnableSoundSorting(bool enable)
{
    impl->soundSortingEnabled = enable;
}

float XAudio2Context::GetDopplerFactor() const
{
    return impl->dopplerFactor;
}

void XAudio2Context::SetDopplerFactor(float factor)
{
    impl->dopplerFactor = factor;
}

float XAudio2Context::GetVolume() const
{
    float volume;
    impl->masteringVoice->GetVolume(&volume);
    return volume;
}

void XAudio2Context::SetVolume(float volume)
{
    impl->masteringVoice->SetVolume(volume);
}

float XAudio2Context::GetMetersPerUnit() const
{
    return impl->metersPerUnit;
}

XAudio2ContextImpl* XAudio2Context::GetImpl()
{
    return impl.get();
}

void XAudio2Context::SetMetersPerUnit(float metersPerUnit)
{
    impl->metersPerUnit = metersPerUnit;
}

float XAudio2Context::GetReferenceDistance() const
{
    return impl->referenceDistance;
}

void XAudio2Context::SetReferenceDistance(float distance)
{
    impl->referenceDistance = distance;
}

float XAudio2Context::GetMaxDistance() const
{
    return impl->maxDistance;
}

void XAudio2Context::SetMaxDistance(float distance)
{
    impl->maxDistance = distance;
}

float XAudio2Context::GetRolloffFactor() const
{
    return impl->rolloffFactor;
}

void XAudio2Context::SetRolloffFactor(float factor)
{
    impl->rolloffFactor = factor;
}

SoundDistanceModel XAudio2Context::GetDistanceModel() const
{
    return SoundDistanceModel::INVERSE_DISTANCE_CLAMPED;
}

void XAudio2Context::SetDistanceModel(SoundDistanceModel model)
{
    //Not yet supported
}
