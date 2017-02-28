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


#pragma once


//Includes---------------------------------------------------------------------
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Distance.hpp"
#include "finjin/engine/AssetClassFileReader.hpp"
#include "finjin/engine/ContextEventInfo.hpp"
#include "finjin/engine/PerFrameObjectAllocator.hpp"
#include "finjin/engine/SoundSourceCommon.hpp"
#include "finjin/engine/ResourcePoolDescription.hpp"
#include <boost/thread/null_mutex.hpp>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    class OSWindow;

    using namespace Finjin::Common;

    struct SoundEvent
    {
        enum class Type
        {
            NOTIFY,

            CREATED_STATIC_BUFFER,
            
            CREATED_SOURCE
        };
        
        SoundEvent()
        {
            this->type = Type::NOTIFY;
            this->count = 0;
        }
        
        Type type;
        
        union
        {
            size_t count;
            void* soundBuffer;
            void* soundSource;
        };

        ContextEventInfo eventInfo;
    };
    
    class SoundEvents : public AllocatedVector<SoundEvent>
    {
    public:
        using Super = AllocatedVector<SoundEvent>;

        using iterator = Super::iterator;
        using const_iterator = Super::const_iterator;

        const_iterator begin() const { return Super::begin(); }
        iterator begin() { return Super::begin(); }

        const_iterator end() const { return Super::end(); }
        iterator end() { return Super::end(); }
    };

    struct SoundCommand
    {
        enum class Type
        {
            NOTIFY,
            
            CREATE_STATIC_BUFFER,
            DESTROY_STATIC_BUFFER,
            
            CREATE_SOURCE,
            DESTROY_SOURCE,
            
            PLAY_SOURCE,
            PAUSE_SOURCE,
            STOP_SOURCE
        };

        SoundCommand()
        {
            this->type = Type::NOTIFY;
            this->next = nullptr;

            this->count = 0;
        }

        Type type;
        SoundCommand* next;

        ContextEventInfo eventInfo;

        union
        {
            size_t count;
            void* staticSound; //FinjinSound
            void* soundBuffer; //SoundBuffer
            void* soundSource; //SoundSource
        };
    };

    class SoundCommands : public PerFrameObjectAllocator<SoundCommand, boost::null_mutex>
    {
    public:
        using Super = PerFrameObjectAllocator<SoundCommand, boost::null_mutex>;
        
        using iterator = Super::iterator;
        using const_iterator = Super::const_iterator;

        const_iterator begin() const { return this->commands.begin(); }
        iterator begin() { return this->commands.begin(); }

        const_iterator end() const { return this->commands.end(); }
        iterator end() { return this->commands.end(); }

        bool CreateStaticBuffer(void* staticSound, const ContextEventInfo& eventInfo = ContextEventInfo::Empty())
        {
            auto command = NewObject<SoundCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->type = SoundCommand::Type::CREATE_STATIC_BUFFER;
            command->eventInfo = eventInfo;
            command->staticSound = staticSound;

            this->commands.push_back(command);
            
            return true;
        }

        bool CreateSource(void* soundBuffer, const ContextEventInfo& eventInfo = ContextEventInfo::Empty())
        {
            auto command = NewObject<SoundCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->type = SoundCommand::Type::CREATE_SOURCE;
            command->eventInfo = eventInfo;
            command->soundBuffer = soundBuffer;

            this->commands.push_back(command);
            
            return true;
        }

        bool Play(void* soundSource, const ContextEventInfo& eventInfo = ContextEventInfo::Empty())
        {
            auto command = NewObject<SoundCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->type = SoundCommand::Type::PLAY_SOURCE;
            command->eventInfo = eventInfo;
            command->soundSource = soundSource;

            this->commands.push_back(command);
            
            return true;
        }

        bool Stop(void* soundSource, const ContextEventInfo& eventInfo = ContextEventInfo::Empty())
        {
            auto command = NewObject<SoundCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->type = SoundCommand::Type::STOP_SOURCE;
            command->eventInfo = eventInfo;
            command->soundSource = soundSource;

            this->commands.push_back(command);
            
            return true;
        }

        bool Pause(void* soundSource, const ContextEventInfo& eventInfo = ContextEventInfo::Empty())
        {
            auto command = NewObject<SoundCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->type = SoundCommand::Type::PAUSE_SOURCE;
            command->eventInfo = eventInfo;
            command->soundSource = soundSource;

            this->commands.push_back(command);
            
            return true;
        }
    };

    enum class SoundDistanceModel
    {
        INVERSE_DISTANCE_CLAMPED,
        LINEAR_DISTANCE_CLAMPED,
        EXPONENT_DISTANCE_CLAMPED
    };
    
    enum class SoundSpeakerSetup
    {
        DEFAULT,
        MONO,
        STEREO,
        STEREO_PLUS,
        SURROUND_4,
        SURROUND_4_1,
        SURROUND_5_1,
        SURROUND_7_1
    };
    
    inline int GetSoundSpeakerSetupChannelCount(SoundSpeakerSetup s)
    {
        switch (s)
        {
            case SoundSpeakerSetup::MONO: return 1;
            case SoundSpeakerSetup::STEREO: return 2;
            case SoundSpeakerSetup::STEREO_PLUS: return 3;
            case SoundSpeakerSetup::SURROUND_4: return 4;
            case SoundSpeakerSetup::SURROUND_4_1: return 5;
            case SoundSpeakerSetup::SURROUND_5_1: return 6;
            case SoundSpeakerSetup::SURROUND_7_1: return 8;
            default: return 0;
        }
    }
    
    struct SoundContextCommonSettings
    {    
        SoundContextCommonSettings(Allocator* allocator);

        void SetDefaultSoundSourcePools();

        size_t GetTotalSoundSourceCount() const;
        size_t GetTotalSoundSourceCount(const SoundFormat& soundFormat) const;

        void ReadSettings(AssetClassFileReader& settingsAssetReader, const AssetReference& settingsFileAssetRef, ByteBuffer& readBuffer, Error& error);
        virtual void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

        void HandleTopLevelSetting(const Utf8StringView& key, const Utf8StringView& value, Error& error);

        void* applicationHandle;
        OSWindow* osWindow;
        AssetFileReader* assetFileReader;
        AssetPathSelector initialAssetFileSelector;
        AssetReferences<EngineConstants::MAX_CONTEXT_SETTINGS_FILES> settingsFileNames; //Usually "sound-context.cfg"
        uint64_t processorID;

        size_t tempReadBufferSize;

        size_t maxCommandsPerUpdate; //Maximum number of high level commands

        Utf8String deviceID;
        
        int playbackSampleRate;
        SoundSpeakerSetup speakerSetup;
        Sound3DQuality sound3DQuality;
        Distance<float> speedOfSound;

        StaticVector<SoundFormat, 16> soundFormats;
        StaticVector<ResourcePoolDescription, 16> soundSourcePools;
                
        size_t maxSoundBufferCount;
    };

} }
