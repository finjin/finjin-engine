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
#include "OpenSLESContext.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/UsableDynamicVector.hpp"
#include "finjin/common/WrappedFileReader.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "finjin/engine/WAVReader.hpp"
#include "OpenSLESListener.hpp"
#include "OpenSLESSoundBuffer.hpp"
#include "OpenSLESSoundGroup.hpp"
#include "OpenSLESSoundSource.hpp"
#include "OpenSLESSoundSourceVoice.hpp"
#include "OpenSLESUtilities.hpp"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static SLuint32 SoundFormatToOpenSLChannelMask(const SoundFormat& soundFormat)
{
    switch (soundFormat.GetChannelCount())
    {
        case 2: return SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
        default: return SL_SPEAKER_FRONT_CENTER;
    }
}


//Local types-------------------------------------------------------------------
struct OpenSLESContext::Impl : public AllocatedClass
{
    Impl(Allocator* allocator, OpenSLESSystem* soundSystem);

    OpenSLESSoundBuffer* CreateBuffer(const ByteBuffer& bytes, Error& error);

    bool CreateSourceVoice(OpenSLESSoundSourceVoice& sourceVoice, const SoundFormat& soundFormat);
    void DestroySourceVoice(OpenSLESSoundSourceVoice& sourceVoice);

    struct SoundSourceLookup
    {
        SoundFormat format;

        UsableDynamicVector<OpenSLESSoundSourceVoice> sourceVoices;

        UsableDynamicVectorOfPointers<OpenSLESSoundSource*> sources;

        DynamicVector<OpenSLESSoundSource*> playingList;

        bool Virtualize(OpenSLESSoundSource* source)
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

        bool Realize(OpenSLESSoundSource* source)
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
    };

    SoundSourceLookup* GetSoundSourceLookup(const SoundFormat& soundFormat);
    SoundSourceLookup* AddSoundSourceLookup(size_t& count, const SoundFormat& soundFormat, size_t maxVoiceCount);

    OpenSLESSystem* soundSystem;

    OpenSLESContext::Settings settings;

    AssetClassFileReader settingsAssetFileReader;

    SLObjectItf engineObject;
    SLEngineItf engineInterface;
    SLEngineCapabilitiesItf engineCapsInterface;
    SLObjectItf outputMixObject;
    SLOutputMixItf outputMixOutputMixInterface;
    SLVolumeItf outputMixVolumeInterface;
    float volume;

    struct SoundSystemInfo
    {
        SLint16 versionMajor;
        SLint16 versionMinor;
        SLint16 versionStep;

        SLuint32 numExtensions;
        DynamicVector<Utf8String> extensions;

        int androidSdkLevelExtension;

        SoundSystemInfo()
        {
            this->versionMajor = this->versionMinor = this->versionStep = 0;

            this->numExtensions = 0;
            this->extensions.clear();

            this->androidSdkLevelExtension = 0;
        }
    };
    SoundSystemInfo info;

    OpenSLESListener listener;

    OpenSLESSoundGroup masterSoundGroup;

    DynamicVector<SoundSourceLookup> soundSourceLookups;
    DynamicVector<OpenSLESSoundSource> sources;

    UsableDynamicVector<OpenSLESSoundBuffer> buffers;

    SoundSorter<OpenSLESSoundSource>* soundSorter;
    bool soundSortingEnabled;

    float metersPerUnit;
    float dopplerFactor;
    float referenceDistance;
    float maxDistance;
    float rolloffFactor;
    SoundDistanceModel distanceModel;
    int createID;

    ByteBuffer readBuffer;
};


//Implementation----------------------------------------------------------------

//OpenSLESContext::Impl
OpenSLESContext::Impl::Impl(Allocator* allocator, OpenSLESSystem* soundSystem) :
    AllocatedClass(allocator),
    settings(allocator)
{
    this->soundSystem = soundSystem;

    this->engineObject = nullptr;
    this->engineInterface = nullptr;
    this->engineCapsInterface = nullptr;
    this->outputMixObject = nullptr;
    this->outputMixOutputMixInterface = nullptr;
    this->outputMixVolumeInterface = nullptr;
    this->volume = 1;

    this->soundSorter = nullptr;
    this->soundSortingEnabled = true;
    this->metersPerUnit = 1;
    this->dopplerFactor = 0;
    this->referenceDistance = 1;
    this->maxDistance = 1;
    this->rolloffFactor = 1;
    this->distanceModel = SoundDistanceModel::INVERSE_DISTANCE_CLAMPED;
    this->createID = 1;
}

OpenSLESSoundBuffer* OpenSLESContext::Impl::CreateBuffer(const ByteBuffer& bytes, Error& error)
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

bool OpenSLESContext::Impl::CreateSourceVoice(OpenSLESSoundSourceVoice& sourceVoice, const SoundFormat& soundFormat)
{
    SLresult result;

    //Configure audio source
    /*
    TODO: Switch to floating point format to unify sound formats between Android and Apple?
    SLAndroidDataFormat_PCM_EX formatPCM = {};
    formatPCM.formatType = SL_ANDROID_DATAFORMAT_PCM_EX;
    formatPCM.numChannels = soundFormat.GetChannelCount();
    formatPCM.sampleRate = soundFormat.samplesPerSecond * 1000;
    formatPCM.containerSize = formatPCM.bitsPerSample = sizeof(float) * 8;
    formatPCM.channelMask = SoundFormatToOpenSLChannelMask(soundFormat);
    formatPCM.endianness = SL_BYTEORDER_LITTLEENDIAN;
    formatPCM.representation = SL_ANDROID_PCM_REPRESENTATION_FLOAT;
    */

    SLDataFormat_PCM formatPCM = {};
    formatPCM.formatType = SL_DATAFORMAT_PCM;
    formatPCM.numChannels = soundFormat.GetChannelCount();
    formatPCM.samplesPerSec = soundFormat.samplesPerSecond * 1000;
    formatPCM.containerSize = formatPCM.bitsPerSample = soundFormat.GetBytesPerSample() * 8;
    formatPCM.endianness = SL_BYTEORDER_LITTLEENDIAN;
    formatPCM.channelMask = SoundFormatToOpenSLChannelMask(soundFormat);

    SLDataLocator_AndroidSimpleBufferQueue locatorBufferQueue;
    locatorBufferQueue.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    locatorBufferQueue.numBuffers = 1;

    SLDataSource audioSrc;
    audioSrc.pLocator = &locatorBufferQueue;
    audioSrc.pFormat = &formatPCM;

    //Configure audio sink
    SLDataLocator_OutputMix locatorOutputMix;
    locatorOutputMix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
    locatorOutputMix.outputMix = this->outputMixObject;

    SLDataSink audioSnk;
    audioSnk.pLocator = &locatorOutputMix;
    audioSnk.pFormat = nullptr;

    //Create audio player
    const int flagCount = 5;
    const SLInterfaceID ids[flagCount] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME, SL_IID_PLAYBACKRATE };
    const SLboolean req[flagCount] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE, SL_BOOLEAN_FALSE };
    result = (*this->engineInterface)->CreateAudioPlayer(this->engineInterface, &sourceVoice.playerInterface, &audioSrc, &audioSnk, flagCount, ids, req);
    if (result != SL_RESULT_SUCCESS)
        return false;

    //Realize the player
    result = (*sourceVoice.playerInterface)->Realize(sourceVoice.playerInterface, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        DestroySourceVoice(sourceVoice);
        return false;
    }

    //Get the play interface
    result = (*sourceVoice.playerInterface)->GetInterface(sourceVoice.playerInterface, SL_IID_PLAY, &sourceVoice.playerPlayInterface);
    if (result != SL_RESULT_SUCCESS)
    {
        DestroySourceVoice(sourceVoice);
        return false;
    }

    //Get the buffer queue interface
    result = (*sourceVoice.playerInterface)->GetInterface(sourceVoice.playerInterface, SL_IID_BUFFERQUEUE, &sourceVoice.bufferQueueInterface);
    if (result != SL_RESULT_SUCCESS)
    {
        DestroySourceVoice(sourceVoice);
        return false;
    }

    //Get the effect send interface
    result = (*sourceVoice.playerInterface)->GetInterface(sourceVoice.playerInterface, SL_IID_EFFECTSEND, &sourceVoice.effectSendInterface);
    if (result != SL_RESULT_SUCCESS)
    {
        //Do nothing
    }

    //Get the volume interface
    result = (*sourceVoice.playerInterface)->GetInterface(sourceVoice.playerInterface, SL_IID_VOLUME, &sourceVoice.volumeInterface);
    if (result != SL_RESULT_SUCCESS)
    {
        //Do nothing
    }

    //Get the playback rate interface
    result = (*sourceVoice.playerInterface)->GetInterface(sourceVoice.playerInterface, SL_IID_PLAYBACKRATE, &sourceVoice.playbackRateInterface);
    if (result != SL_RESULT_SUCCESS)
    {
        //Do nothing
    }

    return true;
}

void OpenSLESContext::Impl::DestroySourceVoice(OpenSLESSoundSourceVoice& sourceVoice)
{
    if (sourceVoice.playerInterface != nullptr)
    {
        (*sourceVoice.playerInterface)->Destroy(sourceVoice.playerInterface);
        sourceVoice.playerInterface = nullptr;
        sourceVoice.playerPlayInterface = nullptr;
        sourceVoice.bufferQueueInterface = nullptr;
        sourceVoice.effectSendInterface = nullptr;
        sourceVoice.volumeInterface = nullptr;
        sourceVoice.playbackRateInterface = nullptr;
    }
}

OpenSLESContext::Impl::SoundSourceLookup* OpenSLESContext::Impl::GetSoundSourceLookup(const SoundFormat& soundFormat)
{
    for (auto& soundSourceLookup : this->soundSourceLookups)
    {
        if (soundSourceLookup.format == soundFormat)
            return &soundSourceLookup;
    }

    return nullptr;
}

OpenSLESContext::Impl::SoundSourceLookup* OpenSLESContext::Impl::AddSoundSourceLookup(size_t& count, const SoundFormat& soundFormat, size_t maxVoiceCount)
{
    //Exit if there aren't any more slots left
    if (count >= this->soundSourceLookups.size())
        return nullptr;

    //Add new lookup item
    auto newSoundSourceIndex = count++;
    auto soundSourceLookup = &soundSourceLookups[newSoundSourceIndex];

    //Initialize
    soundSourceLookup->format = soundFormat;
    soundSourceLookup->sourceVoices.CreateEmpty(maxVoiceCount, GetAllocator());
    soundSourceLookup->sources.CreateEmpty(maxVoiceCount, GetAllocator());
    soundSourceLookup->playingList.CreateEmpty(maxVoiceCount, GetAllocator());
    return soundSourceLookup;
}

//OpenSLESContext
OpenSLESContext::OpenSLESContext(Allocator* allocator, OpenSLESSystem* soundSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS, soundSystem))
{
}

OpenSLESContext::~OpenSLESContext()
{
}

void OpenSLESContext::Create(const Settings& settings, Error& error)
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
        FINJIN_SET_ERROR(error, "A zero sound source count was specified.");
        return;
    }

    if (impl->settings.maxSoundBufferCount == 0)
    {
        FINJIN_SET_ERROR(error, "A zero sound buffer count was specified.");
        return;
    }

    //Create engine--------------------
    SLresult result;

    FINJIN_DEBUG_LOG_INFO("About to slCreateEngine");
    const int engineFlagCount = 1;
    const SLInterfaceID engineFlagIds[engineFlagCount] = { SL_IID_ENGINECAPABILITIES };
    const SLboolean engineFlagIdsRequired[engineFlagCount] = { SL_BOOLEAN_FALSE };
    result = slCreateEngine(&impl->engineObject, 0, 0, engineFlagCount, engineFlagIds, engineFlagIdsRequired);
    if (result != SL_RESULT_SUCCESS)
    {
        FINJIN_SET_ERROR(error, "Failed to create OpenSLES engine.");

        Destroy();
        return;
    }

    //Realize the engine
    FINJIN_DEBUG_LOG_INFO("About to slCreateEngine.Realize()");
    result = (*impl->engineObject)->Realize(impl->engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        FINJIN_SET_ERROR(error, "Failed to realize OpenSLES engine.");

        Destroy();
        return;
    }

    //Get interface
    FINJIN_DEBUG_LOG_INFO("About to slCreateEngine.GetInterface()");
    result = (*impl->engineObject)->GetInterface(impl->engineObject, SL_IID_ENGINE, &impl->engineInterface);
    if (result != SL_RESULT_SUCCESS)
    {
        FINJIN_SET_ERROR(error, "Failed to get OpenSLES engine interface.");

        Destroy();
        return;
    }

    //Output mix--------------------------
    //const int mixFlagCount = 1;
    //const SLInterfaceID maxFlagIds[mixFlagCount] = { SL_IID_ENVIRONMENTALREVERB };
    //const SLboolean maxFlagIdsRequired[mixFlagCount] = { SL_BOOLEAN_FALSE };
    result = (*impl->engineInterface)->CreateOutputMix(impl->engineInterface, &impl->outputMixObject, 0, nullptr, nullptr);// mixFlagCount, maxFlagIds, maxFlagIdsRequired);
    if (result != SL_RESULT_SUCCESS)
    {
        FINJIN_SET_ERROR(error, "Failed to get output mix object.");

        Destroy();
        return;
    }

    //Realize the output mix
    result = (*impl->outputMixObject)->Realize(impl->outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
    {
        FINJIN_SET_ERROR(error, "Failed to realize output mix object.");

        Destroy();
        return;
    }

    result = (*impl->outputMixObject)->GetInterface(impl->outputMixObject, SL_IID_OUTPUTMIX, &impl->outputMixOutputMixInterface);
    if (result != SL_RESULT_SUCCESS)
    {
        //Not an error condition
    }

    result = (*impl->outputMixObject)->GetInterface(impl->outputMixObject, SL_IID_VOLUME, &impl->outputMixVolumeInterface);
    if (result != SL_RESULT_SUCCESS)
    {
        //Not an error condition
    }

    //Extensions-------------------------------------
    FINJIN_DEBUG_LOG_INFO("About to get extensions count");
    result = (*impl->engineInterface)->QueryNumSupportedExtensions(impl->engineInterface, &impl->info.numExtensions);
    if (result == SL_RESULT_SUCCESS && impl->info.numExtensions > 0)
    {
        impl->info.extensions.Create(impl->info.numExtensions, GetAllocator());

        for (SLuint32 index = 0; index < impl->info.numExtensions; index++)
        {
            SLint16 nameLength;
            result = (*impl->engineInterface)->QuerySupportedExtension(impl->engineInterface, index, nullptr, &nameLength);

            impl->info.extensions[index].resize(nameLength);
            result = (*impl->engineInterface)->QuerySupportedExtension(impl->engineInterface, index, reinterpret_cast<SLchar*>(impl->info.extensions[index].begin()), &nameLength);

            FINJIN_DEBUG_LOG_INFO("OpenSL extension %1%: %2%", index, impl->info.extensions[index]);

            if (impl->info.extensions[index].StartsWith("ANDROID_SDK_LEVEL_"))
            {
                Utf8String numberString;
                impl->info.extensions[index].substr(numberString, strlen("ANDROID_SDK_LEVEL_"));
                impl->info.androidSdkLevelExtension = Convert::ToInteger(numberString, 0);
            }
        }
    }

    //Get capabilities
    FINJIN_DEBUG_LOG_INFO("About to get engine caps interface");
    result = (*impl->engineObject)->GetInterface(impl->engineObject, SL_IID_ENGINECAPABILITIES, &impl->engineCapsInterface);
    if (result == SL_RESULT_SUCCESS)
    {
        FINJIN_DEBUG_LOG_INFO("About to query version");
        result = (*impl->engineCapsInterface)->QueryAPIVersion(impl->engineCapsInterface, &impl->info.versionMajor, &impl->info.versionMinor, &impl->info.versionStep);
        FINJIN_DEBUG_LOG_INFO("OpenSL version: %1%.%2%.%3%", impl->info.versionMajor, impl->info.versionMinor, impl->info.versionStep);
    }

    //Listener
    impl->listener.Create(this);

    //Master sound source group
    impl->masterSoundGroup.Create(this);

    //Preallocate various things------------------

    //Sound sources
    impl->sources.Create(totalSoundSourceCount, GetAllocator());
    for (auto& source : impl->sources)
        source.Create(this);

    size_t sourceOffset = 0;

    //Sound source lookups
    size_t lookupCount = 0;
    impl->soundSourceLookups.Create(impl->settings.soundFormats.size(), GetAllocator());
    for (size_t soundSourceDescriptorIndex = 0; soundSourceDescriptorIndex < impl->settings.soundSourcePools.size(); soundSourceDescriptorIndex++)
    {
        auto& soundSourceDescriptor = impl->settings.soundSourcePools[soundSourceDescriptorIndex];

        //Get or add the lookup
        auto soundSourceLookup = impl->GetSoundSourceLookup(impl->settings.soundFormats[soundSourceDescriptor.format]);
        if (soundSourceLookup == nullptr)
            soundSourceLookup = impl->AddSoundSourceLookup(lookupCount, impl->settings.soundFormats[soundSourceDescriptor.format], impl->settings.GetTotalSoundSourceCount(impl->settings.soundFormats[soundSourceDescriptor.format]));
        if (soundSourceLookup != nullptr)
        {
            //Try to create "soundSourceDescriptor.count" voices
            for (size_t sourceIndex = 0; sourceIndex < soundSourceDescriptor.count; sourceIndex++)
            {
                OpenSLESSoundSourceVoice sourceVoice;
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

    for (auto& soundSourceLookup : impl->soundSourceLookups)
    {
        FINJIN_DEBUG_LOG_INFO("Sound format %1% has %2% voices.", soundSourceLookup.format.ToString(), soundSourceLookup.sourceVoices.items.size());
    }

    //Sound buffers
    impl->buffers.Create(impl->settings.maxSoundBufferCount, GetAllocator());
    for (auto& buffer : impl->buffers.items)
        buffer.Create(this);
}

void OpenSLESContext::Destroy()
{
    impl->sources.Destroy();
    for (auto& soundSourceLookup : impl->soundSourceLookups)
    {
        soundSourceLookup.sourceVoices.ClearUsed();
        soundSourceLookup.sources.ClearUsed();

        for (auto& sourceVoice : soundSourceLookup.sourceVoices.items)
            impl->DestroySourceVoice(sourceVoice);
    }
    impl->soundSourceLookups.Destroy();

    for (auto item : impl->buffers.usedItems)
        DestroyBuffer(item, true);
    impl->buffers.Destroy();

    //Destroy engine object
    if (impl->engineObject != nullptr)
    {
        if (impl->outputMixObject != nullptr)
        {
            (*impl->outputMixObject)->Destroy(impl->outputMixObject);
            impl->outputMixObject = nullptr;
        }
        impl->outputMixOutputMixInterface = nullptr;
        impl->outputMixVolumeInterface = nullptr;

        (*impl->engineObject)->Destroy(impl->engineObject);
        impl->engineObject = nullptr;

        impl->engineInterface = nullptr;
        impl->engineCapsInterface = nullptr;
    }
}

void OpenSLESContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const OpenSLESContext::Settings& OpenSLESContext::GetSettings() const
{
    return impl->settings;
}

size_t OpenSLESContext::GetExternalAssetFileExtensions(StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS>& extensions, AssetClass assetClass, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    switch (assetClass)
    {
        case AssetClass::SOUND:
        {
            size_t count = 0;
            for (auto ext : { "wav" })
            {
                if (extensions.push_back(ext).HasErrorOrValue(false))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' extension.", ext));
                    return count;
                }
                count++;
            }
            return count;
        }
        default: break;
    }

    return 0;
}

void OpenSLESContext::HandleApplicationViewportLostFocus()
{
    for (auto& soundSourceLookup : impl->soundSourceLookups)
    {
        for (auto& sourceVoice : soundSourceLookup.sourceVoices.items)
            sourceVoice.Stop();
    }
}

void OpenSLESContext::HandleApplicationViewportGainedFocus()
{
    for (auto& soundSourceLookup : impl->soundSourceLookups)
    {
        for (auto& sourceVoice : soundSourceLookup.sourceVoices.items)
            sourceVoice.Restore();
    }
}

void OpenSLESContext::Update(SimpleTimeDelta elapsedTime)
{
    PriorityListenerDistanceSoundSorter<OpenSLESSoundSource> priorityListenerDistanceSoundSorter;
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
            std::stable_sort(soundSourceLookup.playingList.begin(), soundSourceLookup.playingList.end(), SoundSorterProxy<OpenSLESSoundSource>(soundSorter));
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

void OpenSLESContext::Execute(SoundEvents& events, SoundCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
}

OpenSLESListener* OpenSLESContext::GetListener()
{
    return &impl->listener;
}

OpenSLESSoundGroup* OpenSLESContext::GetMasterGroup()
{
    return &impl->masterSoundGroup;
}

/*OpenSLESSoundBuffer* CreateBuffer(const AssetReference& assetRef, Error& error);
OpenSLESSoundBuffer* OpenSLESContext::CreateBuffer(const AssetReference& assetRef, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Find unused item
    auto item = impl->buffers.FindUnused();
    if (item == nullptr)
    {
        FINJIN_SET_ERROR(error, "The maximum number of sound buffers have already been created.");
        return nullptr;
    }

    //Read asset
    auto readResult = impl->soundsAssetFileReader.ReadAsset(impl->readBuffer, assetRef);
    if (readResult == FileOperationResult::SUCCESS)
    {
        //Try to read the file as a WAV file
        WAVReader wavReader;
        auto wavData = wavReader.Read(impl->readBuffer, item->decompressedBuffer, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to decode sound buffer file '%1%'.", assetRef.ToUriString()));
            return nullptr;
        }

        //Finish
        impl->buffers.Use(item);
        item->LoadFromMemory(wavData.data, wavData.length, wavData.soundFormat);
        return item;
    }

    //Failed to find file
    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to find sound buffer file '%1%'.", assetRef.ToUriString()));
    return nullptr;
}*/

void OpenSLESContext::DestroyBuffer(OpenSLESSoundBuffer* buffer, bool shuttingDown)
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

OpenSLESSoundSource* OpenSLESContext::CreateSource(OpenSLESSoundBuffer* soundBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto source = CreateSource(soundBuffer->GetSoundFormat(), error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    source->SetBuffer(soundBuffer);
    return source;
}

OpenSLESSoundSource* OpenSLESContext::CreateSource(const SoundFormat& soundFormat, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto soundSourceLookup = impl->GetSoundSourceLookup(soundFormat);
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

    source->SetGroup(&impl->masterSoundGroup);

    soundSourceLookup->sources.Use(source);

    return source;
}

void OpenSLESContext::DestroySource(OpenSLESSoundSource* source, bool shuttingDown)
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

SoundSorter<OpenSLESSoundSource>* OpenSLESContext::GetSoundSorter()
{
    return impl->soundSorter;
}

void OpenSLESContext::SetSoundSorter(SoundSorter<OpenSLESSoundSource>* soundSorter)
{
    impl->soundSorter = soundSorter;
}

bool OpenSLESContext::IsSoundSortingEnabled() const
{
    return impl->soundSortingEnabled;
}

void OpenSLESContext::EnableSoundSorting(bool enable)
{
    impl->soundSortingEnabled = enable;
}

float OpenSLESContext::GetDopplerFactor() const
{
    return impl->dopplerFactor;
}

void OpenSLESContext::SetDopplerFactor(float factor)
{
    impl->dopplerFactor = factor;
}

float OpenSLESContext::GetVolume() const
{
    return impl->volume;
}

void OpenSLESContext::SetVolume(float volume)
{
    if (impl->outputMixVolumeInterface != nullptr)
    {
        Limit(volume, 0.0f, 1.0f);
        impl->volume = volume;
        OpenSLESUtilities::SetVolume(impl->outputMixVolumeInterface, volume);
    }
}

float OpenSLESContext::GetMetersPerUnit() const
{
    return impl->metersPerUnit;
}

void OpenSLESContext::SetMetersPerUnit(float metersPerUnit)
{
    impl->metersPerUnit = metersPerUnit;
}

float OpenSLESContext::GetReferenceDistance() const
{
    return impl->referenceDistance;
}

void OpenSLESContext::SetReferenceDistance(float distance)
{
    impl->referenceDistance = distance;
}

float OpenSLESContext::GetMaxDistance() const
{
    return impl->maxDistance;
}

void OpenSLESContext::SetMaxDistance(float distance)
{
    impl->maxDistance = distance;
}

float OpenSLESContext::GetRolloffFactor() const
{
    return impl->rolloffFactor;
}

void OpenSLESContext::SetRolloffFactor(float factor)
{
    impl->rolloffFactor = factor;
}

SoundDistanceModel OpenSLESContext::GetDistanceModel() const
{
    return impl->distanceModel;
}

void OpenSLESContext::SetDistanceModel(SoundDistanceModel model)
{
    impl->distanceModel = model;
}

int OpenSLESContext::NewCreateTimeStamp()
{
    return impl->createID++;
}
