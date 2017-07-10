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
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct InputDeviceConstants
    {
        enum { MAX_DEVICES = 32 }; //Maximum of GameControllerConstants::MAX_GAME_CONTROLLERS, KeyboardConstants::MAX_KEYBOARDS, MouseConstants::MAX_MICE, HeadsetConstants::MAX_HEADSETS, InputScreenConstants::MAX_INPUT_SCREENS
    };

    struct GameControllerConstants
    {
        enum { MAX_GAME_CONTROLLERS = 32 }; //Large to accomodate multiple API's being used at once (for example: XInput + DirectInput + OpenVR on Win32)
        enum { MAX_BUTTON_COUNT = 300 }; //Large for Android
        enum { MAX_AXIS_COUNT = 32 };
        enum { MAX_POV_COUNT = 4 };
        enum { MAX_LOCATOR_COUNT = 1 }; //Maximum number of locators for a game controller
    };

    struct KeyboardConstants
    {
        enum { MAX_KEYBOARDS = 16 };
        enum { MAX_BUTTON_COUNT = 300 }; //Large for Android
    };

    struct MouseConstants
    {
        enum { MAX_MICE = 16 };
        enum { MAX_BUTTON_COUNT = 300 }; //Large for Android
        enum { MAX_AXIS_COUNT = 50 }; //Large for Android

        enum { WHEEL_STEP_SIZE = 10};
    };

    struct HeadsetConstants
    {
        enum { MAX_HEADSETS = 1 };
        enum { MAX_LOCATOR_COUNT = 1 }; //Maximum number of locators for a headset
    };

    struct InputScreenConstants
    {
        enum { MAX_INPUT_SCREENS = 2 }; //2 screens for input (like on PS Vita, for example)
    };

    struct InputPointerConstants
    {
        enum { MAX_INPUT_POINTERS = 10 + MouseConstants::MAX_MICE }; //10 fingers + whatever mice, if we decide to treat the mice as pointers
    };

    enum class InputTriggerFlag : uint32_t
    {
        NONE = 0,

        /** The event should only be triggered when the input is pressed  */
        PRESSED = 1 << 0,

        /** The event can be triggered continuously.*/
        HOLDING = 1 << 1,

        /** The event should only be triggered when the input is released */
        RELEASED = 1 << 2,

        /**
        * Diagonal POV directions are considered to be part horizontal, part vertical
        * If this is specified, do not specify InputTriggerFlag::POV_STRONG
        */
        POV_WEAK = 1 << 3,

        /**
        * Each POV direction is considered to be distinct
        * If this is specified, do not specify InputTriggerFlag::POV_WEAK
        */
        POV_STRONG = 1 << 4,

        /**
        * Indicates single touch actions are allowed to be added even if there is a multitouch input event in effect.
        * If this is NOT specified, single touch actions will not be added if there is a multitouch input event.
        */
        TOUCH_ALLOWED_WITH_MULTITOUCH = 1 << 5
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(InputTriggerFlag)

    struct InputTriggerCriteria
    {
        InputTriggerCriteria() : flags(InputTriggerFlag::PRESSED | InputTriggerFlag::HOLDING | InputTriggerFlag::RELEASED), count(1) {}
        InputTriggerCriteria(InputTriggerFlag f) : flags(f), count(1) {}
        InputTriggerCriteria(InputTriggerFlag f, size_t c) : flags(f), count(c) {}

        InputTriggerFlag flags;

        size_t count; //Not currently used for anything
    };

    class InputDeviceUtilities
    {
    public:
        static Utf8String MakeInputDeviceIdentifier(const Utf8StringView& identifierString);
        static Utf8String MakeInputDeviceIdentifier(const Utf8String& identifierString);
    };

} }
