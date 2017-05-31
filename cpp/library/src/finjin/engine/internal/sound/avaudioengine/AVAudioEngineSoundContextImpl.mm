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
#include "AVAudioEngineSoundContextImpl.hpp"
#include "finjin/common/DebugLog.hpp"
#include "AVAudioEngineSoundBufferImpl.hpp"
#include "WAVReader.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static AudioChannelLayoutTag SoundFormatToLayoutTag(const SoundFormat& soundFormat)
{
    if (soundFormat.channelCount == 1)
        return kAudioChannelLayoutTag_Mono;
    else if (soundFormat.channelCount == 2)
        return kAudioChannelLayoutTag_Stereo;
    else if (soundFormat.channelCount == 3)
        return kAudioChannelLayoutTag_MPEG_3_0_A;
    else if (soundFormat.channelCount == 4)
        return kAudioChannelLayoutTag_Quadraphonic;
    else if (soundFormat.channelCount == 5)
        return kAudioChannelLayoutTag_Pentagonal;
    else if (soundFormat.channelCount == 6)
        return kAudioChannelLayoutTag_Hexagonal;
    else if (soundFormat.channelCount == 8)
        return kAudioChannelLayoutTag_Octagonal;
    else
        return kAudioChannelLayoutTag_Stereo;
}

static SoundSpeakerSetup GetBestSpeakerSetup(AVAudioEngine* avengine)
{
    auto channelCount = [avengine.outputNode outputFormatForBus:0].channelCount;
    switch (channelCount)
    {
        case 1: return SoundSpeakerSetup::MONO;
        case 2: return SoundSpeakerSetup::STEREO;
        case 3: return SoundSpeakerSetup::STEREO_PLUS;
        case 4: return SoundSpeakerSetup::SURROUND_4;
        case 5: return SoundSpeakerSetup::SURROUND_4_1;
        case 6: return SoundSpeakerSetup::SURROUND_5_1;
        case 8: return SoundSpeakerSetup::SURROUND_7_1;
        default: return SoundSpeakerSetup::DEFAULT;
    }
}

#if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
static AudioDeviceID GetDefaultDeviceID()
{
    //Get the default output device ID
    AudioObjectPropertyAddress addr;
    addr.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
    addr.mScope = kAudioObjectPropertyScopeGlobal;
    addr.mElement = 0;
    UInt32 size = sizeof(AudioDeviceID);
    AudioDeviceID deviceID = 0;
    auto result = AudioHardwareServiceGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, &deviceID);
    if (result != 0)
        return 0;

    return deviceID;
}
#endif

/*static int GetDefaultOutputSampleRate()
{
#if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
    auto deviceID = GetDefaultDeviceID();

    AudioObjectPropertyAddress addr;
    addr.mSelector = kAudioDevicePropertyNominalSampleRate;
    addr.mScope = kAudioObjectPropertyScopeGlobal;
    addr.mElement = 0;
    UInt32 size = sizeof(Float64);
    Float64 outResult;
    auto result = AudioHardwareServiceGetPropertyData(deviceID, &addr, 0, nullptr, &size, &outResult);
    if (result != 0)
        return 0;

    return RoundToInt(outResult);
#else
    return RoundToInt([AVAudioSession sharedInstance].sampleRate);
#endif
}*/

static int GetDefaultMaxVoices()
{
#if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
    auto deviceID = GetDefaultDeviceID();

    AudioObjectPropertyAddress addr;
    UInt32 size;
    addr.mSelector = kAudioDevicePropertyStreams;
    addr.mScope = kAudioObjectPropertyScopeGlobal;
    addr.mElement = 0;
    size = sizeof(UInt32);
    UInt32 outResult;
    auto result = AudioHardwareServiceGetPropertyData(deviceID, &addr, 0, nullptr, &size, &outResult);
    if (result != 0)
        return 0;

    return outResult;
#else
    return 31;
#endif
}

static SoundFormat GenericSoundFormatToAVAudioSoundFormat(const SoundFormat& soundFormat)
{
    //All sound formats should are floating point
    auto fixed = soundFormat;
    fixed.sampleType = SoundFormat::SampleType::FLOATING_POINT;
    fixed.bytesPerChannel = sizeof(float);
    return fixed;
}


//Implementation----------------------------------------------------------------

//AVAudioEngineSoundContextImplNSObject
@implementation AVAudioEngineSoundContextImplNSObject

- (void)configureSession:(Error&)error
{
#if FINJIN_TARGET_PLATFORM != FINJIN_TARGET_PLATFORM_MACOS
    FINJIN_ERROR_METHOD_START(error);

    NSError* nserror = nullptr;
    auto sessionInstance = [AVAudioSession sharedInstance];

    if (![sessionInstance setCategory:AVAudioSessionCategoryPlayback withOptions:AVAudioSessionCategoryOptionDuckOthers error:&nserror])
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Error setting AVAudio session category to AVAudioSessionCategoryPlayback: '%1%'.", nserror.localizedDescription.UTF8String));
        return;
    }

    if (settings.speakerSetup != SoundSpeakerSetup::DEFAULT)
    {
        auto speakerOutputChannelCount = GetSoundSpeakerSetupChannelCount(settings.speakerSetup);
        if (sessionInstance.outputNumberOfChannels != speakerOutputChannelCount)
        {
            if (![sessionInstance setPreferredOutputNumberOfChannels:speakerOutputChannelCount error:&nserror])
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Error setting AVAudio session preffered output channels to '%1%'.", speakerOutputChannelCount));
                return;
            }
        }
    }

    if (settings.playbackSampleRate != 0)
    {
        if (![sessionInstance setPreferredSampleRate:settings.playbackSampleRate error:&nserror])
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Error setting preferred sample rate to '%1%': '%2%'.", settings.playbackSampleRate, nserror.localizedDescription.UTF8String));
            return;
        }
    }

    if (!_addedSessionObservers)
    {
        auto notificationCenter = [NSNotificationCenter defaultCenter];

        [notificationCenter addObserver:self selector:@selector(handleInterruption:) name:AVAudioSessionInterruptionNotification object:sessionInstance];
        [notificationCenter addObserver:self selector:@selector(handleRouteChange:) name:AVAudioSessionRouteChangeNotification object:sessionInstance];
        [notificationCenter addObserver:self selector:@selector(handleMediaServicesReset:) name:AVAudioSessionMediaServicesWereResetNotification object:sessionInstance];

        _addedSessionObservers = true;
    }

    if (![sessionInstance setActive:YES error:&nserror])
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Error setting active session: '%1%'.", nserror.localizedDescription.UTF8String));
        return;
    }
#endif
}

- (void)initialize:(Error&)error
{
    //Scenarios for calling this method:
    //1)This method is being called for the first time:
    //    The sound lookups are created, voices are created, attached, connections made, sound engine started
    //2)This method is called after an interruption or AVAudioEngineConfigurationChangeNotification:
    //    The connections are made, and sound engine started
    //3)This method is called after a media services reset:
    //    Voices are created, attached, connections made, sound engine started

    FINJIN_ERROR_METHOD_START(error);

    [self configureSession:error];
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to configure sound session.");
        return;
    }

    if (avengine == nullptr)
        avengine = [[AVAudioEngine alloc] init];
    if (avenv == nullptr)
    {
        avenv = [[AVAudioEnvironmentNode alloc] init];
        [avengine attachNode:avenv];
    }

    avenvOutputFormat = [self createOutputFormatForEnvironment];
    [avengine connect:avenv to:avengine.outputNode format:avenvOutputFormat];

    if (!soundSourceLookups.empty())
    {
        //Scenario 2) or 3)

        for (auto& soundSourceLookup : soundSourceLookups)
        {
            for (auto& sourceVoice : soundSourceLookup.sourceVoices.items)
            {
                if (sourceVoice.player == nullptr)
                {
                    //Scenario 3)
                    sourceVoice.player = [[AVAudioPlayerNode alloc] init];
                    [avengine attachNode:sourceVoice.player];
                }

                //Scenario 2) or 3)
                [avengine connect:sourceVoice.player to:avenv format:soundSourceLookup.avformat];
            }
        }
    }
    else
    {
        //Scenario 1)

        size_t sourceOffset = 0;
        size_t sourceVoiceCount = 0;
        auto maxSourceVoiceCount = GetDefaultMaxVoices();

        //Sound source lookups

        //First convert formats
        for (auto& soundFormat : settings.soundFormats)
            soundFormat = GenericSoundFormatToAVAudioSoundFormat(soundFormat);

        //Create as many voices as possible
        size_t lookupCount = 0;
        if (soundSourceLookups.max_size() == 0)
            soundSourceLookups.Create(settings.soundFormats.size(), allocator);
        else
            soundSourceLookups.resize(settings.soundFormats.size());
        for (size_t soundSourceDescriptorIndex = 0;
             soundSourceDescriptorIndex < settings.soundSourcePools.size() && sourceVoiceCount < maxSourceVoiceCount;
             soundSourceDescriptorIndex++)
        {
            auto& soundSourceDescriptor = settings.soundSourcePools[soundSourceDescriptorIndex];

            //Get or add the lookup
            auto soundSourceLookup = [self getSoundSourceLookup:settings.soundFormats[soundSourceDescriptor.format]];
            if (soundSourceLookup == nullptr)
            {
                auto& format = settings.soundFormats[soundSourceDescriptor.format];
                auto formatSourceCount = settings.GetTotalSoundSourceCount(format);
                soundSourceLookup = [self addSoundSourceLookup:lookupCount soundFormat:format maxVoiceCount:formatSourceCount error:error];
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to add sound source lookup.");
                    return;
                }
            }

            if (soundSourceLookup != nullptr)
            {
                //Try to create "soundSourceDescriptor.count" voices
                for (size_t sourceIndex = 0;
                     sourceIndex < soundSourceDescriptor.count && sourceVoiceCount < maxSourceVoiceCount;
                     sourceIndex++)
                {
                    AVAudioEngineSoundSourceVoice sourceVoice;
                    sourceVoice.player = [[AVAudioPlayerNode alloc] init];

                    [avengine attachNode:sourceVoice.player];
                    @try
                    {
                        [avengine connect:sourceVoice.player to:avenv format:soundSourceLookup->avformat];
                        soundSourceLookup->sourceVoices.items.push_back(sourceVoice);
                        sourceVoiceCount++;
                    }
                    @catch (NSException* e)
                    {
                        //This will likely never occur but if it does assume that no more players can be created
                        [avengine detachNode:sourceVoice.player];
                        break;
                    }
                }

                //Add "soundSourceDescriptor.count" sources
                for (size_t sourceIndex = 0; sourceIndex < soundSourceDescriptor.count; sourceIndex++)
                {
                    auto source = &sources[sourceOffset++];
                    source->SetSoundFormat(settings.soundFormats[soundSourceDescriptor.format]);
                    soundSourceLookup->sources.items.push_back(source);
                }
            }
        }
    }

    //Start engine
    if (!avengine.isRunning)
    {
        NSError* nserror = nullptr;
        if (![avengine startAndReturnError:&nserror])
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to start AV audio engine: '%1%'.", nserror.localizedDescription.UTF8String));
            return;
        }
    }
}

#if FINJIN_TARGET_PLATFORM != FINJIN_TARGET_PLATFORM_MACOS
- (void)handleInterruption:(NSNotification*)notification
{
    UInt8 theInterruptionType = [[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue];

    if (theInterruptionType == AVAudioSessionInterruptionTypeBegan)
    {
        _isSessionInterrupted = true;

        for (auto& soundSourceLookup : soundSourceLookups)
        {
            for (auto& sourceVoice : soundSourceLookup.sourceVoices.items)
            {
                if (sourceVoice.player != nullptr)
                    [sourceVoice.player stop];
            }
        }
    }
    else if (theInterruptionType == AVAudioSessionInterruptionTypeEnded)
    {
        NSError* nserror;
        if ([[AVAudioSession sharedInstance] setActive:YES error:&nserror])
        {
            _isSessionInterrupted = false;

            if (_isConfigChangePending)
            {
                //There is a pending config changed notification. Reinitialize all connections and start again
                FINJIN_DECLARE_ERROR(error);
                [self initialize:error];
                if (error)
                {
                    FINJIN_DEBUG_LOG_ERROR("Error reinitializing AV audio engine after interruption.");
                }

                _isConfigChangePending = false;
            }
            else
            {
                //Start the engine once again
                if (!avengine.isRunning)
                {
                    if ([avengine startAndReturnError:&nserror])
                    {
                        //TODO: Restart sounds that are in a playing state? If so, be sure to advance sounds back to their play offset
                    }
                    else
                    {
                        FINJIN_DEBUG_LOG_ERROR("Failed to restarting AV audio engine session after interruption: %1%'", nserror.localizedDescription.UTF8String);
                    }
                }
            }
        }
        else
        {
            FINJIN_DEBUG_LOG_ERROR("Failed to set active AV audio engine session after interruption: %1%'", nserror.localizedDescription.UTF8String);
        }
    }
}

- (void)handleRouteChange:(NSNotification*)notification
{
    /*
    UInt8 reasonValue = [[notification.userInfo valueForKey:AVAudioSessionRouteChangeReasonKey] intValue];
    AVAudioSessionRouteDescription* previousRouteDescription = [notification.userInfo valueForKey:AVAudioSessionRouteChangePreviousRouteKey];

    switch (reasonValue)
    {
        case AVAudioSessionRouteChangeReasonNewDeviceAvailable: break;
        case AVAudioSessionRouteChangeReasonOldDeviceUnavailable: break;
        case AVAudioSessionRouteChangeReasonCategoryChange: break;
        case AVAudioSessionRouteChangeReasonOverride: break;
        case AVAudioSessionRouteChangeReasonWakeFromSleep: break;
        case AVAudioSessionRouteChangeReasonNoSuitableRouteForCategory: break;
        default: break;
    }
    */
}

- (void)handleMediaServicesReset:(NSNotification*)notification
{
    FINJIN_DECLARE_ERROR(error);

    //Nullify AVAudioEngine objects
    for (auto& soundSourceLookup : soundSourceLookups)
    {
        for (auto& sourceVoice : soundSourceLookup.sourceVoices.items)
        {
            sourceVoice.player = nullptr;
        }
    }
    avenv = nullptr;
    avengine = nullptr;

    //Re-initialize everything
    [self initialize:error];
    if (error)
    {
        FINJIN_DEBUG_LOG_ERROR("Error restarting AV audio engine after media services reset.");
    }
}
#endif

- (AVAudioFormat*)createOutputFormatForEnvironment
{
    auto desiredSpeakerSetup = settings.speakerSetup;
    if (desiredSpeakerSetup == SoundSpeakerSetup::DEFAULT)
        desiredSpeakerSetup = GetBestSpeakerSetup(avengine);

    AudioChannelLayoutTag layoutTag = 0;
    switch (desiredSpeakerSetup)
    {
        case SoundSpeakerSetup::MONO: //Fall through to stereo, don't use mono
        case SoundSpeakerSetup::STEREO: layoutTag = kAudioChannelLayoutTag_Stereo; break;
        case SoundSpeakerSetup::STEREO_PLUS: layoutTag = kAudioChannelLayoutTag_MPEG_3_0_A; break;
        case SoundSpeakerSetup::SURROUND_4: layoutTag = kAudioChannelLayoutTag_Quadraphonic; break;
        case SoundSpeakerSetup::SURROUND_4_1: layoutTag = kAudioChannelLayoutTag_Pentagonal; break;
        case SoundSpeakerSetup::SURROUND_5_1: layoutTag = kAudioChannelLayoutTag_Hexagonal; break;
        case SoundSpeakerSetup::SURROUND_7_1: layoutTag = kAudioChannelLayoutTag_Octagonal; break;
        default: layoutTag = kAudioChannelLayoutTag_Stereo; break;
    }

    auto layout = [[AVAudioChannelLayout alloc] initWithLayoutTag:layoutTag];
    auto sampleRate = [avengine.outputNode outputFormatForBus:0].sampleRate;
    return [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatFloat32 sampleRate:sampleRate interleaved:NO channelLayout:layout];
}

- (AVAudioEngineSoundSourceLookup*)getSoundSourceLookup:(const SoundFormat&)soundFormat
{
    for (auto& soundSourceLookup : soundSourceLookups)
    {
        if (soundSourceLookup.format == soundFormat)
            return &soundSourceLookup;
    }

    return nullptr;
}

- (AVAudioEngineSoundSourceLookup*)addSoundSourceLookup:(size_t&)count soundFormat:(const SoundFormat&)soundFormat maxVoiceCount:(size_t)maxVoiceCount error:(Error&)error
{
    FINJIN_ERROR_METHOD_START(error);

    //Exit if there aren't any more slots left
    if (count >= soundSourceLookups.size())
        return nullptr;

    //Add new lookup item
    auto newSoundSourceIndex = count++;
    auto soundSourceLookup = &soundSourceLookups[newSoundSourceIndex];

    //Initialize
    auto layout = [[AVAudioChannelLayout alloc] initWithLayoutTag:SoundFormatToLayoutTag(soundFormat)];

    soundSourceLookup->format = soundFormat;
    soundSourceLookup->avformat = [[AVAudioFormat alloc] initWithCommonFormat:AVAudioPCMFormatFloat32 sampleRate:soundFormat.samplesPerSecond interleaved:NO channelLayout:layout];
    if (!soundSourceLookup->sourceVoices.CreateEmpty(maxVoiceCount, allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create source voices lookup.");
        return nullptr;
    }
    if (!soundSourceLookup->sources.CreateEmpty(maxVoiceCount, allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create sources lookup.");
        return nullptr;
    }
    soundSourceLookup->playingList.CreateEmpty(maxVoiceCount, allocator);
    return soundSourceLookup;
}

@end

//AVAudioEngineSoundSourceLookup
AVAudioEngineSoundSourceLookup::AVAudioEngineSoundSourceLookup()
{
    this->avformat = nullptr;
}

bool AVAudioEngineSoundSourceLookup::Virtualize(AVAudioEngineSoundSource* source)
{
    //Remove voice from source
    auto sourceVoice = source->impl->DetachVoice();
    if (sourceVoice != nullptr)
    {
        [sourceVoice->player stop];

        //Remove from used list
        this->sourceVoices.Unuse(sourceVoice);

        return true;
    }

    return false;
}

bool AVAudioEngineSoundSourceLookup::Realize(AVAudioEngineSoundSource* source)
{
    auto sourceImpl = source->impl;
    auto sourceVoice = sourceImpl->sourceVoice;
    if (sourceVoice == nullptr)
    {
        //Find unused. This should not fail
        sourceVoice = this->sourceVoices.FindUnused();
        assert(sourceVoice != nullptr);

        //Set voice into source
        sourceImpl->SetVoice(sourceVoice);

        //Add to used list
        this->sourceVoices.Use(sourceVoice);

        return true;
    }

    return false;
}

//AVAudioEngineSoundContextImpl
AVAudioEngineSoundContextImpl::AVAudioEngineSoundContextImpl(Allocator* allocator, AVAudioEngineSoundSystem* soundSystem) : AllocatedClass(allocator)
{
    nsimpl = [[AVAudioEngineSoundContextImplNSObject alloc] init];

    nsimpl->allocator = allocator;

    nsimpl->soundSystem = soundSystem;

    nsimpl->soundSortingEnabled = true;
    nsimpl->metersPerUnit = 1;
    nsimpl->dopplerFactor = 0;
    nsimpl->createID = 1;
}

void AVAudioEngineSoundContextImpl::Initialize(const AVAudioEngineSoundContext::Settings& settings, Error& error)
{
    assert(settings.assetFileReader != nullptr);

    FINJIN_ERROR_METHOD_START(error);

    //Copy and validate settings-------------------------------
    nsimpl->settings = settings;

    if (!nsimpl->readBuffer.Create(nsimpl->settings.tempReadBufferSize, nsimpl->allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create temp read buffer");
        return;
    }

    nsimpl->settingsAssetFileReader.Create(*nsimpl->settings.assetFileReader, nsimpl->settings.initialAssetFileSelector, AssetClass::SETTINGS, nsimpl->allocator, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create settings asset file reader.");
        return;
    }
    for (auto& settingsFileName : nsimpl->settings.settingsFileNames)
    {
        nsimpl->settings.ReadSettings(nsimpl->settingsAssetFileReader, settingsFileName, nsimpl->readBuffer, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There was an error reading the sound context '%1%' settings file.", settingsFileName.ToUriString()));
            return;
        }
    }

    auto totalSoundSourceCount = nsimpl->settings.GetTotalSoundSourceCount();
    if (totalSoundSourceCount == 0)
    {
        FINJIN_SET_ERROR(error, "A zero sound source count was specified.");
        return;
    }

    if (nsimpl->settings.maxSoundBufferCount == 0)
    {
        FINJIN_SET_ERROR(error, "A zero sound buffer count was specified.");
        return;
    }

    //Listener
    nsimpl->listener.Create(this);

    //Master sound source group
    nsimpl->masterSoundGroup.Create(this);

    //Sound sources
    nsimpl->sources.Create(totalSoundSourceCount, nsimpl->allocator);
    for (auto& source : nsimpl->sources)
        source.Create(this);

    //Sound buffers
    if (!nsimpl->buffers.Create(nsimpl->settings.maxSoundBufferCount, nsimpl->allocator))
    {
        Destroy();

        FINJIN_SET_ERROR(error, "Failed to create buffers lookup.");
        return;
    }
    for (auto& buffer : nsimpl->buffers.items)
        buffer.Create(this);

    [nsimpl initialize:error];
    if (error)
    {
        Destroy();

        FINJIN_SET_ERROR_NO_MESSAGE(error);

        return;
    }

    //Sign up for notifications from the engine if there's a hardware config change
    nsimpl->configChangeNotificationID = [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioEngineConfigurationChangeNotification object:nullptr queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification *note)
    {
        nsimpl->_isConfigChangePending = true;

        if (!nsimpl->_isSessionInterrupted)
        {
            //Reinitialize all the connections and start the engine
            FINJIN_DECLARE_ERROR(error);
            [nsimpl initialize:error];
            if (error)
            {
                FINJIN_DEBUG_LOG_ERROR("Error restarting AV audio engine after configuration change.");
            }
        }
    }];
}

void AVAudioEngineSoundContextImpl::Destroy()
{
    auto notificationCenter = [NSNotificationCenter defaultCenter];

    if (nsimpl->configChangeNotificationID != nullptr)
    {
        [notificationCenter removeObserver:nsimpl->configChangeNotificationID];
        nsimpl->configChangeNotificationID = nullptr;
    }

#if FINJIN_TARGET_PLATFORM != FINJIN_TARGET_PLATFORM_MACOS
    if (nsimpl->_addedSessionObservers)
    {
        auto sessionInstance = [AVAudioSession sharedInstance];

        [notificationCenter removeObserver:nsimpl name:AVAudioSessionInterruptionNotification object:sessionInstance];
        [notificationCenter removeObserver:nsimpl name:AVAudioSessionRouteChangeNotification object:sessionInstance];
        [notificationCenter removeObserver:nsimpl name:AVAudioSessionMediaServicesWereResetNotification object:sessionInstance];

        nsimpl->_addedSessionObservers = false;
    }
#endif

    nsimpl->sources.clear();
    for (auto& soundSourceLookup : nsimpl->soundSourceLookups)
    {
        soundSourceLookup.avformat = nullptr;

        soundSourceLookup.sourceVoices.usedItems.clear();
        soundSourceLookup.sources.usedItems.clear();

        for (auto& sourceVoice : soundSourceLookup.sourceVoices.items)
        {
            [nsimpl->avengine disconnectNodeOutput:sourceVoice.player];
            sourceVoice.player = nullptr;
        }
    }
    nsimpl->soundSourceLookups.clear();

    for (auto buffer : nsimpl->buffers.usedItems)
    {
        buffer->impl->DetachBuffer();
    }
    nsimpl->buffers.clear();

    nsimpl->avenv = nullptr;
    if (nsimpl->avengine != nullptr)
    {
        [nsimpl->avengine stop];
        nsimpl->avengine = nullptr;
        nsimpl->avenvOutputFormat = nullptr;
    }
}

void AVAudioEngineSoundContextImpl::Update(SimpleTimeDelta elapsedTime)
{
    PriorityListenerDistanceSoundSorter<AVAudioEngineSoundSource> priorityListenerDistanceSoundSorter;
    auto soundSorter = (nsimpl->soundSorter != nullptr) ? nsimpl->soundSorter : &priorityListenerDistanceSoundSorter;
    {
        MathVector3 position;
        nsimpl->listener.GetPosition(position);
        soundSorter->SetListener(position);
    }

    //Update listener
    nsimpl->listener.Commit();

    //Process each lookup
    for (auto& soundSourceLookup : nsimpl->soundSourceLookups)
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
        if (nsimpl->soundSortingEnabled)
        {
            std::stable_sort(soundSourceLookup.playingList.begin(), soundSourceLookup.playingList.end(), SoundSorterProxy<AVAudioEngineSoundSource>(soundSorter));
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

void AVAudioEngineSoundContextImpl::CreateBuffer(AVAudioEngineSoundBuffer* item, const AssetReference& assetRef, ByteBufferReader& reader, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Try to read the bytes as a WAV file
    WAVReader wavReader;
    auto wavData = wavReader.Read(reader, item->impl->decompressedBuffer, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to decode sound buffer file '%1%'.", assetRef.ToUriString()));
        return;
    }

    auto frameCount = static_cast<AVAudioFrameCount>(wavData.GetSampleCount());
    if (frameCount == 0)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate sound buffer for file '%1%' since there are no samples.", assetRef.ToUriString()));
        return;
    }

    //Finish
    size_t totalDataLength = 0;
    auto avsoundFormat = GenericSoundFormatToAVAudioSoundFormat(wavData.soundFormat);
    auto soundSourceLookup = [nsimpl getSoundSourceLookup:avsoundFormat];
    if (soundSourceLookup != nullptr)
    {
        auto avbuffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:soundSourceLookup->avformat frameCapacity:frameCount];
        if (avbuffer == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate sound buffer for file '%1%'.", assetRef.ToUriString()));
            return;
        }
        avbuffer.frameLength = frameCount;

        auto bufferList = avbuffer.mutableAudioBufferList;
        auto minChannelCount = std::min(static_cast<size_t>(bufferList->mNumberBuffers), static_cast<size_t>(wavData.soundFormat.channelCount));
        for (size_t channelIndex = 0; channelIndex < minChannelCount; channelIndex++)
        {
            auto& destBuffer = bufferList->mBuffers[channelIndex];

            destBuffer.mNumberChannels = 1;
            destBuffer.mDataByteSize = frameCount * sizeof(float);

            totalDataLength += destBuffer.mDataByteSize;

            auto destBufferData = static_cast<float*>(destBuffer.mData);
            switch (wavData.soundFormat.bytesPerChannel)
            {
                case 1:
                {
                    auto* sourceData = static_cast<const uint8_t*>(wavData.data) + channelIndex;
                    for (size_t frameIndex = 0; frameIndex < frameCount; frameIndex++)
                    {
                        *destBufferData = RoundToFloat(*sourceData) / 128.0f - 1.0f;
                        Limit(*destBufferData, -1.0f, 1.0f);
                        destBufferData++;

                        sourceData += wavData.soundFormat.channelCount;
                    }
                    break;
                }
                case 2:
                {
                    auto* sourceData = static_cast<const int16_t*>(wavData.data) + channelIndex;
                    for (size_t frameIndex = 0; frameIndex < frameCount; frameIndex++)
                    {
                        *destBufferData = RoundToFloat(*sourceData) / 32767.0f;
                        Limit(*destBufferData, -1.0f, 1.0f);
                        destBufferData++;

                        sourceData += wavData.soundFormat.channelCount;
                    }
                    break;
                }
            }
        }

        item->impl->Set(avbuffer, totalDataLength, avsoundFormat);
        nsimpl->buffers.Use(item);
        return;
    }

    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to load sound buffer file '%1%'.", assetRef.ToUriString()));
}

AVAudioEngineSoundBuffer* AVAudioEngineSoundContextImpl::CreateBuffer(const AssetReference& assetRef, const ByteBuffer& bytes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Find unused item
    auto item = nsimpl->buffers.FindUnused();
    if (item == nullptr)
    {
        FINJIN_SET_ERROR(error, "The maximum number of sound buffers have already been created.");
        return nullptr;
    }

    ByteBufferReader reader(bytes);
    CreateBuffer(item, assetRef, reader, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return nullptr;
    }

    return item;
}

AVAudioEngineSoundSource* AVAudioEngineSoundContextImpl::CreateSource(const SoundFormat& soundFormat, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto avsoundFormat = GenericSoundFormatToAVAudioSoundFormat(soundFormat);
    auto soundSourceLookup = [nsimpl getSoundSourceLookup:avsoundFormat];
    if (soundSourceLookup == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("The specified sound format '%1%' doesn't have any precreated instances.", avsoundFormat.ToString()));
        return nullptr;
    }

    auto source = soundSourceLookup->sources.FindUnused();
    if (source == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("The maximum number of sound sources of the specified format '%1%' have already been created.", avsoundFormat.ToString()));
        return nullptr;
    }

    source->SetDefaults();

    source->SetGroup(&nsimpl->masterSoundGroup);

    soundSourceLookup->sources.Use(source);

    return source;
}
