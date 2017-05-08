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


//Includes----------------------------------------------------------------------
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/AssetFileReader.hpp"
#include "finjin/engine/ContextEventInfo.hpp"
#include "finjin/engine/InputComponents.hpp"
#include "finjin/engine/PerFrameObjectAllocator.hpp"
#include <boost/thread/null_mutex.hpp>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class OSWindow;

    using namespace Finjin::Common;

    struct InputEvent
    {
        enum class Type
        {
            NOTIFY,

            ADDED_HAPTIC_FEEDBACK,
            STOPPED_HAPTIC_FEEDBACK
        };

        InputEvent()
        {
            this->type = Type::NOTIFY;
            this->count = 0;
        }

        Type type;

        union
        {
            size_t count;
        };

        ContextEventInfo eventInfo;
    };

    class InputEvents : public DynamicVector<InputEvent>
    {
    public:
        using Super = DynamicVector<InputEvent>;

        using iterator = Super::iterator;
        using const_iterator = Super::const_iterator;

        const_iterator begin() const { return Super::begin(); }
        iterator begin() { return Super::begin(); }

        const_iterator end() const { return Super::end(); }
        iterator end() { return Super::end(); }
    };

    struct InputCommand
    {
        enum class Type
        {
            NOTIFY,

            ADD_HAPTIC_FEEDBACK,
            STOP_HAPTIC_FEEDBACK
        };

        InputCommand()
        {
            this->type = Type::NOTIFY;
            this->next = nullptr;
        }

        Type type;
        InputCommand* next;

        ContextEventInfo eventInfo;

        InputDeviceClass deviceClass;
        size_t deviceIndex;
        HapticFeedbackSettings force;
    };

    class InputCommands : public PerFrameObjectAllocator<InputCommand, boost::null_mutex>
    {
    public:
        using Super = PerFrameObjectAllocator<InputCommand, boost::null_mutex>;

        using iterator = Super::iterator;
        using const_iterator = Super::const_iterator;

        const_iterator begin() const { return this->commands.begin(); }
        iterator begin() { return this->commands.begin(); }

        const_iterator end() const { return this->commands.end(); }
        iterator end() { return this->commands.end(); }

        bool AddHapticFeedback(InputDeviceClass deviceClass, size_t deviceIndex, const HapticFeedbackSettings& force, const ContextEventInfo& eventInfo = ContextEventInfo::Empty())
        {
            auto command = NewObject<InputCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->type = InputCommand::Type::ADD_HAPTIC_FEEDBACK;
            command->eventInfo = eventInfo;
            command->deviceClass = deviceClass;
            command->deviceIndex = deviceIndex;
            command->force = force;

            this->commands.push_back(command);

            return true;
        }

        bool StopHapticFeedback(InputDeviceClass deviceClass, size_t deviceIndex, const ContextEventInfo& eventInfo = ContextEventInfo::Empty())
        {
            auto command = NewObject<InputCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->type = InputCommand::Type::STOP_HAPTIC_FEEDBACK;
            command->eventInfo = eventInfo;
            command->deviceClass = deviceClass;
            command->deviceIndex = deviceIndex;

            this->commands.push_back(command);

            return true;
        }
    };

    enum class InputDevicePollFlag
    {
        NONE = 0,
        CHECK_FOR_NEW_DEVICES = 1 << 0
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(InputDevicePollFlag)

    struct InputContextCommonSettings
    {
        InputContextCommonSettings(Allocator* allocator);

        void HandleTopLevelSetting(const Utf8StringView& key, const Utf8StringView& value, Error& error);

        void* applicationHandle;
        OSWindow* osWindow;
        AssetFileReader* assetFileReader;
        AssetPathSelector initialAssetFileSelector;
        AssetReferences<EngineConstants::MAX_CONTEXT_SETTINGS_FILES> settingsFileNames; //Usually "sound-context.cfg"

        size_t maxCommandsPerUpdate;
    };

} }
