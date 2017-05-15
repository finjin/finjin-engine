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
#include "InputComponents.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/StaticUnorderedMap.hpp"
#include "InputSource.hpp"

using namespace Finjin::Engine;


//Local variables---------------------------------------------------------------
static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(InputDeviceClass, InputDeviceClassUtilities::COUNT) inputDeviceClassLowerLookup
    (
    "none", InputDeviceClass::NONE,
    "keyboard", InputDeviceClass::KEYBOARD,
    "mouse", InputDeviceClass::MOUSE,
    "game-controller", InputDeviceClass::GAME_CONTROLLER,
    "touch-screen", InputDeviceClass::TOUCH_SCREEN,
    "accelerometer", InputDeviceClass::ACCELEROMETER,
    "headset", InputDeviceClass::HEADSET,
    "all", InputDeviceClass::ALL
    );

static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(InputDeviceComponent, InputDeviceComponent::COUNT) inputDeviceComponentLookup
    (
    "keyboard-key", InputDeviceComponent::KEYBOARD_KEY,

    "mouse-button", InputDeviceComponent::MOUSE_BUTTON,
    "mouse-relative-axis", InputDeviceComponent::MOUSE_RELATIVE_AXIS,
    "mouse-absolute-axis", InputDeviceComponent::MOUSE_ABSOLUTE_AXIS,

    "game-controller-button", InputDeviceComponent::GAME_CONTROLLER_BUTTON,
    "game-controller-axis", InputDeviceComponent::GAME_CONTROLLER_AXIS,
    "game-controller-pov", InputDeviceComponent::GAME_CONTROLLER_POV,
    "game-controller-locator", InputDeviceComponent::GAME_CONTROLLER_LOCATOR,

    "touch-count", InputDeviceComponent::TOUCH_COUNT,
    "touch-relative-axis", InputDeviceComponent::TOUCH_RELATIVE_AXIS,
    "touch-absolute-axis", InputDeviceComponent::TOUCH_ABSOLUTE_AXIS,

    "multitouch-relative-radius", InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS,
    "multitouch-relative-axis", InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS,

    "accelerometer-relative-axis", InputDeviceComponent::ACCELEROMETER_RELATIVE_AXIS,
    "accelerometer-absolute-axis", InputDeviceComponent::ACCELEROMETER_ABSOLUTE_AXIS,

    "headset-locator", InputDeviceComponent::HEADSET_LOCATOR
    );

static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(InputDeviceSemantic, InputDeviceSemanticUtilities::COUNT) inputDeviceSemanticLookup
    (
    "none", InputDeviceSemantic::NONE,

    "left-hand", InputDeviceSemantic::LEFT_HAND,
    "right-hand", InputDeviceSemantic::RIGHT_HAND
    );

static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(InputComponentSemantic, InputComponentSemanticUtilities::COUNT) inputComponentSemanticLookup
    (
    "none", InputComponentSemantic::NONE,

    "accept", InputComponentSemantic::ACCEPT,

    "cancel", InputComponentSemantic::CANCEL,

    "default", InputComponentSemantic::DEFAULT,
    "handbrake", InputComponentSemantic::HANDBRAKE,
    "reload", InputComponentSemantic::RELOAD,

    "change", InputComponentSemantic::CHANGE,
    "change-view", InputComponentSemantic::CHANGE_VIEW,
    "change-weapon", InputComponentSemantic::CHANGE_WEAPON,

    "settings", InputComponentSemantic::SETTINGS,

    "system-settings", InputComponentSemantic::SYSTEM_SETTINGS,

    "system-home", InputComponentSemantic::SYSTEM_HOME,

    "shift-left", InputComponentSemantic::SHIFT_LEFT,
    "grenade-left", InputComponentSemantic::GRENADE_LEFT,

    "shift-right", InputComponentSemantic::SHIFT_RIGHT,
    "grenade-right", InputComponentSemantic::GRENADE_RIGHT,

    "brake", InputComponentSemantic::BRAKE,
    "aim", InputComponentSemantic::AIM,

    "gas", InputComponentSemantic::GAS,
    "fire", InputComponentSemantic::FIRE,

    "toggle-up", InputComponentSemantic::TOGGLE_UP,
    "toggle-down", InputComponentSemantic::TOGGLE_DOWN,
    "toggle-left", InputComponentSemantic::TOGGLE_LEFT,
    "toggle-right", InputComponentSemantic::TOGGLE_RIGHT,
    "toggle-up-down", InputComponentSemantic::TOGGLE_UP_DOWN,
    "toggle-left-right", InputComponentSemantic::TOGGLE_LEFT_RIGHT,
    "toggle-all", InputComponentSemantic::TOGGLE_ALL,

    "steer-up", InputComponentSemantic::STEER_UP,
    "move-up", InputComponentSemantic::MOVE_UP,
    "steer-down", InputComponentSemantic::STEER_DOWN,
    "move-down", InputComponentSemantic::MOVE_DOWN,
    "steer-up-down", InputComponentSemantic::STEER_UP_DOWN,
    "move-up-down", InputComponentSemantic::MOVE_UP_DOWN,
    "steer-left", InputComponentSemantic::STEER_LEFT,
    "move-left", InputComponentSemantic::MOVE_LEFT,
    "steer-right", InputComponentSemantic::STEER_RIGHT,
    "move-right", InputComponentSemantic::MOVE_RIGHT,
    "steer-left-right", InputComponentSemantic::STEER_LEFT_RIGHT,
    "move-left-right", InputComponentSemantic::MOVE_LEFT_RIGHT,
    "steer-all", InputComponentSemantic::STEER_ALL,
    "move-all", InputComponentSemantic::MOVE_ALL,

    "steer-toggle", InputComponentSemantic::STEER_TOGGLE,
    "move-toggle", InputComponentSemantic::MOVE_TOGGLE,

    "look-up", InputComponentSemantic::LOOK_UP,
    "look-down", InputComponentSemantic::LOOK_DOWN,
    "look-up-down", InputComponentSemantic::LOOK_UP_DOWN,
    "look-left", InputComponentSemantic::LOOK_LEFT,
    "look-right", InputComponentSemantic::LOOK_RIGHT,
    "look-left-right", InputComponentSemantic::LOOK_LEFT_RIGHT,
    "look-all", InputComponentSemantic::LOOK_ALL,

    "look-toggle", InputComponentSemantic::LOOK_TOGGLE,

    "touch-pad-button", InputComponentSemantic::TOUCH_PAD_BUTTON,

    "brake-click", InputComponentSemantic::BRAKE_CLICK,
    "aim-click", InputComponentSemantic::AIM_CLICK,

    "gas-click", InputComponentSemantic::GAS_CLICK,
    "fire-click", InputComponentSemantic::FIRE_CLICK,

    "locator", InputComponentSemantic::LOCATOR
    );

static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(PovDirection, PovDirectionUtilities::COUNT) povDirectionLookup
    (
    "centered", PovDirection::CENTERED,
    "up", PovDirection::UP,
    "up-right", PovDirection::UP_RIGHT,
    "right", PovDirection::RIGHT,
    "down-right", PovDirection::DOWN_RIGHT,
    "down", PovDirection::DOWN,
    "down-left", PovDirection::DOWN_LEFT,
    "left", PovDirection::LEFT,
    "up-left", PovDirection::UP_LEFT
    );


//Local functions---------------------------------------------------------------
inline bool IsAxisDead(float v, float deadZone)
{
    return std::abs(v) <= deadZone;
}

static void GetMultitouchInfo(InputTouchScreen::Pointer** pointers, size_t pointerCount, InputAxis* centerAxes, InputAxis& pinchRadiusAxis, bool isFirstUpdate)
{
    float x = 0;
    float y = 0;
    float pinchRadius = 0;

    //Add up the centers
    for (size_t i = 0; i < pointerCount; i++)
    {
        x += pointers[i]->GetX().GetAbsoluteValue();
        y += pointers[i]->GetY().GetAbsoluteValue();
    }

    if (pointerCount > 1)
    {
        //Divide center by count, getting average center
        auto floatCount = RoundToFloat(pointerCount);

        x /= floatCount;
        y /= floatCount;

        //Get average radius
        float relativeX;
        float relativeY;
        for (size_t i = 0; i < pointerCount; i++)
        {
            relativeX = pointers[i]->GetX().GetAbsoluteValue() - x;
            relativeY = pointers[i]->GetY().GetAbsoluteValue() - y;
            pinchRadius += sqrtf(relativeX * relativeX + relativeY * relativeY);
        }
        pinchRadius /= floatCount;
    }

    centerAxes[0].Update(x, isFirstUpdate);
    centerAxes[1].Update(y, isFirstUpdate);
    pinchRadiusAxis.Update(pinchRadius, isFirstUpdate);
}


//Implementation----------------------------------------------------------------

//InputDeviceComponentUtilities
bool InputDeviceComponentUtilities::IsKeyboard(InputDeviceComponent type)
{
    return type == InputDeviceComponent::KEYBOARD_KEY;
}

bool InputDeviceComponentUtilities::IsMouse(InputDeviceComponent type)
{
    return
        type == InputDeviceComponent::MOUSE_BUTTON ||
        type == InputDeviceComponent::MOUSE_RELATIVE_AXIS ||
        type == InputDeviceComponent::MOUSE_ABSOLUTE_AXIS
        ;
}

bool InputDeviceComponentUtilities::IsGameController(InputDeviceComponent type)
{
    return
        type == InputDeviceComponent::GAME_CONTROLLER_BUTTON ||
        type == InputDeviceComponent::GAME_CONTROLLER_AXIS ||
        type == InputDeviceComponent::GAME_CONTROLLER_POV ||
        type == InputDeviceComponent::GAME_CONTROLLER_LOCATOR
        ;
}

bool InputDeviceComponentUtilities::IsTouchScreen(InputDeviceComponent type)
{
    return
        type == InputDeviceComponent::TOUCH_COUNT ||
        type == InputDeviceComponent::TOUCH_RELATIVE_AXIS ||
        type == InputDeviceComponent::TOUCH_ABSOLUTE_AXIS ||
        type == InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS ||
        type == InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS
        ;
}

bool InputDeviceComponentUtilities::IsAccelerometer(InputDeviceComponent type)
{
    return
        type == InputDeviceComponent::ACCELEROMETER_RELATIVE_AXIS ||
        type == InputDeviceComponent::ACCELEROMETER_ABSOLUTE_AXIS
        ;
}

bool InputDeviceComponentUtilities::IsHeadset(InputDeviceComponent type)
{
    return
        type == InputDeviceComponent::HEADSET_LOCATOR
        ;
}

//InputDeviceClassUtilities
size_t InputDeviceClassUtilities::ToIndex(InputDeviceClass deviceClass)
{
    switch (deviceClass)
    {
        case InputDeviceClass::NONE: return 0;
        case InputDeviceClass::KEYBOARD: return 1;
        case InputDeviceClass::MOUSE: return 2;
        case InputDeviceClass::GAME_CONTROLLER: return 3;
        case InputDeviceClass::TOUCH_SCREEN: return 4;
        case InputDeviceClass::ACCELEROMETER: return 5;
        case InputDeviceClass::HEADSET: return 6;
        default: return 0;
    }
}

const char* InputDeviceClassUtilities::ToString(InputDeviceClass deviceClass)
{
    for (auto& item : inputDeviceClassLowerLookup)
    {
        if (item.second == deviceClass)
            return item.first;
    }

    return "<unknown>";
}

//InputDeviceComponentUtilities
const char* InputDeviceComponentUtilities::ToString(InputDeviceComponent deviceComponent)
{
    for (auto& item : inputDeviceComponentLookup)
    {
        if (item.second == deviceComponent)
            return item.first;
    }

    return "<unknown>";
}

InputDeviceComponent InputDeviceComponentUtilities::Parse(const Utf8String& deviceComponent)
{
    return inputDeviceComponentLookup.GetOrDefault(deviceComponent, InputDeviceComponent::NONE);
}

InputDeviceComponent InputDeviceComponentUtilities::Parse(const Utf8StringView& deviceComponent)
{
    return inputDeviceComponentLookup.GetOrDefault(deviceComponent, InputDeviceComponent::NONE);
}

InputDeviceClass InputDeviceComponentUtilities::GetDeviceClass(InputDeviceComponent deviceComponent)
{
    switch (deviceComponent)
    {
        case InputDeviceComponent::KEYBOARD_KEY:
        {
            return InputDeviceClass::KEYBOARD;
        }

        case InputDeviceComponent::MOUSE_BUTTON:
        case InputDeviceComponent::MOUSE_RELATIVE_AXIS:
        case InputDeviceComponent::MOUSE_ABSOLUTE_AXIS:
        {
            return InputDeviceClass::MOUSE;
        }

        case InputDeviceComponent::GAME_CONTROLLER_BUTTON:
        case InputDeviceComponent::GAME_CONTROLLER_AXIS:
        case InputDeviceComponent::GAME_CONTROLLER_POV:
        case InputDeviceComponent::GAME_CONTROLLER_LOCATOR:
        {
            return InputDeviceClass::GAME_CONTROLLER;
        }

        case InputDeviceComponent::TOUCH_COUNT:
        case InputDeviceComponent::TOUCH_RELATIVE_AXIS:
        case InputDeviceComponent::TOUCH_ABSOLUTE_AXIS:
        case InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS:
        case InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS:
        {
            return InputDeviceClass::TOUCH_SCREEN;
        }

        case InputDeviceComponent::ACCELEROMETER_RELATIVE_AXIS:
        case InputDeviceComponent::ACCELEROMETER_ABSOLUTE_AXIS:
        {
            return InputDeviceClass::ACCELEROMETER;
        }

        case InputDeviceComponent::HEADSET_LOCATOR:
        {
            return InputDeviceClass::HEADSET;
        }

        default: return InputDeviceClass::NONE;
    }
}

InputDeviceComponentClass InputDeviceComponentUtilities::GetDeviceComponentClass(InputDeviceComponent deviceComponent)
{
    switch (deviceComponent)
    {
        case InputDeviceComponent::KEYBOARD_KEY:
        case InputDeviceComponent::MOUSE_BUTTON:
        case InputDeviceComponent::GAME_CONTROLLER_BUTTON:
        {
            return InputDeviceComponentClass::BUTTON;
        }

        case InputDeviceComponent::MOUSE_RELATIVE_AXIS:
        case InputDeviceComponent::MOUSE_ABSOLUTE_AXIS:
        case InputDeviceComponent::GAME_CONTROLLER_AXIS:
        case InputDeviceComponent::TOUCH_RELATIVE_AXIS:
        case InputDeviceComponent::TOUCH_ABSOLUTE_AXIS:
        case InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS:
        {
            return InputDeviceComponentClass::AXIS;
        }

        case InputDeviceComponent::GAME_CONTROLLER_POV:
        {
            return InputDeviceComponentClass::POV;
        }

        case InputDeviceComponent::TOUCH_COUNT:
        {
            return InputDeviceComponentClass::COUNT;
        }

        case InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS:
        {
            return InputDeviceComponentClass::RADIUS;
        }

        case InputDeviceComponent::ACCELEROMETER_RELATIVE_AXIS:
        case InputDeviceComponent::ACCELEROMETER_ABSOLUTE_AXIS:
        {
            return InputDeviceComponentClass::AXIS;
        }

        case InputDeviceComponent::GAME_CONTROLLER_LOCATOR:
        case InputDeviceComponent::HEADSET_LOCATOR:
        {
            return InputDeviceComponentClass::LOCATOR;
        }

        default: return InputDeviceComponentClass::NONE;
    }
}

//InputDeviceSemanticUtilities
const char* InputDeviceSemanticUtilities::ToString(InputDeviceSemantic semantic)
{
    for (auto& item : inputDeviceSemanticLookup)
    {
        if (item.second == semantic)
            return item.first;
    }

    return "<unknown>";
}

InputDeviceSemantic InputDeviceSemanticUtilities::Parse(const Utf8String& semantic)
{
    return inputDeviceSemanticLookup.GetOrDefault(semantic, InputDeviceSemantic::NONE);
}

InputDeviceSemantic InputDeviceSemanticUtilities::Parse(const Utf8StringView& semantic)
{
    return inputDeviceSemanticLookup.GetOrDefault(semantic, InputDeviceSemantic::NONE);
}

//InputComponentSemanticUtilities
const char* InputComponentSemanticUtilities::ToString(InputComponentSemantic semantic)
{
    for (auto& item : inputComponentSemanticLookup)
    {
        if (item.second == semantic)
            return item.first;
    }

    return "<unknown>";
}

InputComponentSemantic InputComponentSemanticUtilities::Parse(const Utf8String& semantic)
{
    return inputComponentSemanticLookup.GetOrDefault(semantic, InputComponentSemantic::NONE);
}

InputComponentSemantic InputComponentSemanticUtilities::Parse(const Utf8StringView& semantic)
{
    return inputComponentSemanticLookup.GetOrDefault(semantic, InputComponentSemantic::NONE);
}

int InputComponentSemanticUtilities::GetMoveLookToggleDirection(InputComponentSemantic semantic)
{
    switch (semantic)
    {
        case InputComponentSemantic::MOVE_UP:
        case InputComponentSemantic::MOVE_RIGHT:
        case InputComponentSemantic::LOOK_UP:
        case InputComponentSemantic::LOOK_RIGHT:
        case InputComponentSemantic::TOGGLE_UP:
        case InputComponentSemantic::TOGGLE_RIGHT: return 1;

        case InputComponentSemantic::MOVE_DOWN:
        case InputComponentSemantic::MOVE_LEFT:
        case InputComponentSemantic::LOOK_DOWN:
        case InputComponentSemantic::LOOK_LEFT:
        case InputComponentSemantic::TOGGLE_DOWN:
        case InputComponentSemantic::TOGGLE_LEFT: return -1;

        default: return 0;
    }
}

//PovDirectionUtilities
const char* PovDirectionUtilities::ToString(PovDirection povDirection)
{
    for (auto& item : povDirectionLookup)
    {
        if (item.second == povDirection)
            return item.first;
    }

    return "<unknown>";
}

PovDirection PovDirectionUtilities::Parse(const Utf8String& povDirection)
{
    return povDirectionLookup.GetOrDefault(povDirection, PovDirection::CENTERED);
}

PovDirection PovDirectionUtilities::Parse(const Utf8StringView& povDirection)
{
    return povDirectionLookup.GetOrDefault(povDirection, PovDirection::CENTERED);
}

//InputAxis
InputAxis::InputAxis()
{
    Reset(true);
}

void InputAxis::Reset(bool isConstructing)
{
    this->index = 0;
    this->code = InputSource::NO_COMPONENT_CODE;
    this->restValue = 0;
    this->minValue = 0;
    this->maxValue = 0;
    this->deadZone = 0;
    this->direction = 1;
    this->displayName.clear();
    this->semantic = InputComponentSemantic::NONE;
    this->processing = InputAxisProcessing::NONE;

    this->isEnabled = true;
    this->previousValue.Reset(0.0f);
    this->value.Reset(0.0f);
    this->changed = false;
}

void InputAxis::Update(float value, bool isFirstUpdate)
{
    /*auto showInfo = false;
    if (this->semantic == InputComponentSemantic::MOVE_LEFT_RIGHT)
    {
        FINJIN_DEBUG_LOG_INFO("Entering InputAxis::Update(%1%)", value);
        showInfo = true;

        FINJIN_DEBUG_LOG_INFO("Axis value: %1%, rest: %2%", value, this->restValue);
    }*/

    if (this->isEnabled)
    {
        if (AnySet(this->processing & InputAxisProcessing::ROUND_UPDATE_TO_ONE))
        {
            if (value < 0)
                value = -1;
            else if (value > 0)
                value = 1;
        }

        if (isFirstUpdate)
        {
            if (NoneSet(this->processing & InputAxisProcessing::IS_ABSOLUTE))
            {
                //Axis is relative. Assume the axis is at rest for the first update
                value = 0;
            }
        }
        else
        {
            if (AnySet(this->processing & InputAxisProcessing::IS_ABSOLUTE))
            {
                //Axis is absolute
                if (this->value.IsSet() && std::abs(value - this->value.value) <= this->deadZone)
                {
                    //The value change falls beneath the dead zone. Set it to the current value
                    value = this->value.value;
                }
            }
            else
            {
                //Axis is relative
                if (std::abs(value - this->restValue) <= this->deadZone)
                {
                    //Axis value falls beneath the dead zone. Set it back to rest value
                    value = this->restValue;

                    //if (showInfo)
                        //std::cout << "Fell back to rest: " << value << std::endl;
                }
            }
        }

        if (this->minValue != this->maxValue)
        {
            if (AnySet(this->processing & InputAxisProcessing::LIMIT_MIN_MAX))
                Limit(value, this->minValue, this->maxValue);

            if (AnySet(this->processing & InputAxisProcessing::NORMALIZE))
            {
                value = (value - this->restValue) / (this->maxValue - this->restValue);
                //if (showInfo)
                    //std::cout << "Normalized to: " << value << std::endl;
            }
        }

        if (isFirstUpdate)
        {
            //if (showInfo)
                //std::cout << "First update" << std::endl;
            //First update
            this->changed = false;
            this->previousValue.Reset(0.0f);
            this->value = value;
        }
        else if (!this->changed)
        {
            //if (showInfo)
                //std::cout << "First change" << std::endl;
            //First change
            this->previousValue = this->value;
            this->changed |= value != this->value.value;
            this->value = value;
        }
        else
        {
            //if (showInfo)
                //std::cout << "Multiple changes" << std::endl;

            //Multiple changes in a single frame's update
            if (AnySet(this->processing & InputAxisProcessing::ACCUMULATE_MULTIPLE_CHANGES))
            {
                //Accumulate
                this->value.value += value;
            }
            else
            {
                //if (showInfo)
                    //std::cout << "Keeping most sensible value" << std::endl;

                //Keep the most sensible value
                if (value < 0 && this->value < 0)
                    this->value.value = std::min(value, this->value.value);
                else if (value >= 0 && this->value >= 0)
                    this->value.value = std::max(value, this->value.value);
                else
                    this->value.value = value;
            }
        }
    }

    //if (showInfo)
        //FINJIN_DEBUG_LOG_INFO("Leaving InputAxis::Update(%1%)", value);
}

bool InputAxis::Changed() const
{
    return this->changed;
}

void InputAxis::ClearChanged()
{
    this->changed = false;

    if (AnySet(this->processing & InputAxisProcessing::REST_ON_CLEAR_CHANGES))
    {
        this->previousValue.Reset(0.0f);
        this->value = 0.0f;
    }
}

float InputAxis::GetValue() const
{
    return this->value;
}

float InputAxis::GetRelativeValue() const
{
    if (!this->previousValue.IsSet())
        return 0;

    if (AnySet(this->processing & InputAxisProcessing::IS_ABSOLUTE))
        return this->value - this->previousValue;
    else
        return this->value;
}

float InputAxis::GetAbsoluteValue() const
{
    if (AnySet(this->processing & InputAxisProcessing::IS_ABSOLUTE))
        return this->value;
    else
        return this->value; //Assume caller knows what they're doing.
}

bool InputAxis::IsFirstDown(float deadZone) const
{
    if (AnySet(this->processing & InputAxisProcessing::IS_ABSOLUTE))
    {
        //No rest value. Use previous value as basis

        if (!this->value.IsSet() || !this->previousValue.IsSet())
            return false;

        return !IsAxisDead(this->value - this->previousValue, deadZone);
    }
    else
    {
        //Use rest value as basis

        if (!this->previousValue.IsSet())
            return !IsAxisDead(this->value, deadZone);

        return IsAxisDead(this->previousValue, deadZone) && !IsAxisDead(this->value, deadZone);
    }
}

bool InputAxis::IsHoldingDown(float deadZone) const
{
    if (!this->previousValue.IsSet())
        return false;

    return !IsAxisDead(this->previousValue, deadZone) && !IsAxisDead(this->value, deadZone);
}

bool InputAxis::IsFirstUp(float deadZone) const
{
    if (!this->previousValue.IsSet())
        return false;

    return !IsAxisDead(this->previousValue, deadZone) && IsAxisDead(this->value, deadZone);
}

float InputAxis::GetRestValue() const
{
    return this->restValue;
}

InputAxis& InputAxis::SetRestValue(float value)
{
    this->restValue = value;
    return *this;
}

float InputAxis::GetMinValue() const
{
    return this->minValue;
}

InputAxis& InputAxis::SetMinValue(float value)
{
    this->minValue = value;
    return *this;
}

float InputAxis::GetMaxValue() const
{
    return this->maxValue;
}

InputAxis& InputAxis::SetMaxValue(float value)
{
    this->maxValue = value;
    return *this;
}

InputAxis& InputAxis::SetMinMax(float minValue, float maxValue)
{
    this->minValue = minValue;
    this->maxValue = maxValue;
    return *this;
}

float InputAxis::GetDeadZone() const
{
    return this->deadZone;
}

InputAxis& InputAxis::SetDeadZone(float value)
{
    this->deadZone = value;
    return *this;
}

size_t InputAxis::GetIndex() const
{
    return this->index;
}

InputAxis& InputAxis::SetIndex(size_t index)
{
    this->index = index;
    return *this;
}

bool InputAxis::HasCode() const
{
    return this->code != InputSource::NO_COMPONENT_CODE;
}

int InputAxis::GetCode() const
{
    return this->code;
}

InputAxis& InputAxis::SetCode(int code)
{
    this->code = code;
    return *this;
}

const Utf8String& InputAxis::GetDisplayName() const
{
    return this->displayName;
}

InputComponentSemantic InputAxis::GetSemantic() const
{
    return this->semantic;
}

InputAxis& InputAxis::SetSemantic(InputComponentSemantic value)
{
    this->semantic = value;
    return *this;
}

InputAxisProcessing InputAxis::GetProcessing() const
{
    return this->processing;
}

InputAxis& InputAxis::SetProcessing(InputAxisProcessing value)
{
    this->processing = value;
    return *this;
}

bool InputAxis::IsAbsolute() const
{
    return AnySet(this->processing & InputAxisProcessing::IS_ABSOLUTE);
}

bool InputAxis::IsRelative() const
{
    return NoneSet(this->processing & InputAxisProcessing::IS_ABSOLUTE);
}

float InputAxis::GetDirection() const
{
    return this->direction;
}

InputAxis& InputAxis::SetDirection(float value)
{
    this->direction = value;
    return *this;
}

bool InputAxis::IsEnabled() const
{
    return this->isEnabled;
}

InputAxis& InputAxis::Enable(bool value)
{
    this->isEnabled = value;
    return *this;
}

//InputButton
InputButton::InputButton()
{
    Reset(true);
}

void InputButton::Reset(bool isConstructing)
{
    this->index = 0;
    this->code = InputSource::NO_COMPONENT_CODE;
    this->displayName.clear();
    this->semantic = InputComponentSemantic::NONE;
    this->processing = InputButtonProcessing::NONE;

    this->isEnabled = true;
    this->previousValue.Reset(false);
    this->value = false;
    this->changed = false;
}

void InputButton::Update(bool pressedDown, bool isFirstUpdate)
{
    if (this->isEnabled)
    {
        if (isFirstUpdate)
        {
            this->previousValue.Reset(false);
            this->changed = false;
        }
        else
        {
            this->previousValue = this->value;
            this->changed |= this->value != pressedDown;
        }
        this->value = pressedDown;
    }
}

bool InputButton::Changed() const
{
    return this->changed;
}

void InputButton::ClearChanged()
{
    if (AnySet(this->processing & InputButtonProcessing::EVENT_DRIVEN))
    {
        this->previousValue = value;
        this->changed = false;
    }
    else if (AnySet(this->processing & InputButtonProcessing::REST_ON_CLEAR_CHANGES))
    {
        if (this->value)
        {
            this->value = false;
            this->changed = true;
        }
        else
            this->changed = false;
    }
    else
        this->changed = false;
}

bool InputButton::GetValue() const
{
    return this->value;
}

bool InputButton::IsFirstDown() const
{
    if (!this->previousValue.IsSet())
        return this->value;

    return !this->previousValue.value && this->value;
}

bool InputButton::IsHoldingDown() const
{
    if (!this->previousValue.IsSet())
        return false;

    return this->previousValue.value && this->value;
}

bool InputButton::IsFirstUp() const
{
    if (!this->previousValue.IsSet())
        return false;

    return this->previousValue.value && !this->value;
}

size_t InputButton::GetIndex() const
{
    return this->index;
}

bool InputButton::HasCode() const
{
    return this->code != InputSource::NO_COMPONENT_CODE;
}

int InputButton::GetCode() const
{
    return this->code;
}

InputButton& InputButton::SetCode(int code)
{
    this->code = code;
    return *this;
}

InputButton& InputButton::SetIndex(size_t index)
{
    this->index = index;
    return *this;
}

const Utf8String& InputButton::GetDisplayName() const
{
    return this->displayName;
}

InputComponentSemantic InputButton::GetSemantic() const
{
    return this->semantic;
}

InputButton& InputButton::SetSemantic(InputComponentSemantic value)
{
    this->semantic = value;
    return *this;
}

InputButtonProcessing InputButton::GetProcessing() const
{
    return this->processing;
}

InputButton& InputButton::SetProcessing(InputButtonProcessing value)
{
    this->processing = value;
    return *this;
}

bool InputButton::IsEnabled() const
{
    return this->isEnabled;
}

InputButton& InputButton::Enable(bool value)
{
    this->isEnabled = value;
    return *this;
}

//InputPov
InputPov::InputPov()
{
    Reset(true);
}

void InputPov::Reset(bool isConstructing)
{
    this->index = 0;
    this->code = InputSource::NO_COMPONENT_CODE;
    this->displayName.clear();
    this->semantic = InputComponentSemantic::NONE;

    this->isEnabled = true;
    this->previousValue.Reset(PovDirection::CENTERED);
    this->value = PovDirection::CENTERED;
    this->changed = false;
}

void InputPov::Update(long povValue, bool isFirstUpdate)
{
    if (this->isEnabled)
    {
        PovDirection pov;
        switch (povValue)
        {
            case 0: pov = PovDirection::UP; break;
            case 4500: pov = PovDirection::UP_RIGHT; break;
            case 9000: pov = PovDirection::RIGHT; break;
            case 13500: pov = PovDirection::DOWN_RIGHT; break;
            case 18000: pov = PovDirection::DOWN; break;
            case 22500: pov = PovDirection::DOWN_LEFT; break;
            case 27000: pov = PovDirection::LEFT; break;
            case 31500: pov = PovDirection::UP_LEFT; break;
            default: pov = PovDirection::CENTERED; break;
        }

        if (isFirstUpdate)
        {
            this->previousValue.Reset(PovDirection::CENTERED);
            this->changed = false;
        }
        else
        {
            this->previousValue = this->value;
            this->changed |= this->value != pov;
        }
        this->value = pov;
    }
}

bool InputPov::Changed() const
{
    return this->changed;
}

void InputPov::ClearChanged()
{
    this->changed = false;
}

PovDirection InputPov::GetValue() const
{
    return this->value;
}

bool InputPov::IsFirstDown(bool strict) const
{
    if (!this->previousValue.IsSet() && this->value != PovDirection::CENTERED)
        return true;

    if (strict)
        return this->previousValue.value == PovDirection::CENTERED && this->value != PovDirection::CENTERED;
    else
        return this->previousValue.value != this->value && this->value != PovDirection::CENTERED && AnySet(this->previousValue.value & this->value);
}

bool InputPov::IsFirstDown(PovDirection povDirection, bool strict) const
{
    if (!this->previousValue.IsSet())
    {
        if (strict)
            return this->value == povDirection;
        else
            return AnySet(this->value & povDirection);
    }

    if (strict)
        return (this->previousValue.value != povDirection) && (this->value == povDirection);
    else
        return NoneSet(this->previousValue.value & povDirection) && AnySet(this->value & povDirection);
}

bool InputPov::IsHoldingDown(PovDirection povDirection, bool strict) const
{
    if (!this->previousValue.IsSet())
        return false;

    if (strict)
        return (this->previousValue.value == povDirection) && (this->value == povDirection);
    else
        return AnySet(this->previousValue.value & povDirection) && AnySet(this->value & povDirection);
}

bool InputPov::IsFirstUp(PovDirection povDirection, bool strict) const
{
    if (!this->previousValue.IsSet())
        return false;

    if (strict)
        return (this->previousValue.value == povDirection) && (this->value != povDirection);
    else
        return AnySet(this->previousValue.value & povDirection) && NoneSet(this->value & povDirection);
}

size_t InputPov::GetIndex() const
{
    return this->index;
}

InputPov& InputPov::SetIndex(size_t index)
{
    this->index = index;
    return *this;
}

bool InputPov::HasCode() const
{
    return this->code != InputSource::NO_COMPONENT_CODE;
}

int InputPov::GetCode() const
{
    return this->code;
}

InputPov& InputPov::SetCode(int code)
{
    this->code = code;
    return *this;
}

const Utf8String& InputPov::GetDisplayName() const
{
    return this->displayName;
}

InputComponentSemantic InputPov::GetSemantic() const
{
    return this->semantic;
}

InputPov& InputPov::SetSemantic(InputComponentSemantic value)
{
    this->semantic = value;
    return *this;
}

bool InputPov::IsEnabled() const
{
    return this->isEnabled;
}

InputPov& InputPov::Enable(bool value)
{
    this->isEnabled = value;
    return *this;
}

//InputLocator
InputLocator::InputLocator()
{
    Reset(true);
}

void InputLocator::Reset(bool isConstructing)
{
    this->index = 0;
    this->code = InputSource::NO_COMPONENT_CODE;
    this->displayName.clear();
    this->semantic = InputComponentSemantic::NONE;

    this->isEnabled = true;

    this->orientationMatrix.setIdentity();
    this->position.SetMeters(MathVector3::Zero());
    this->velocity.SetMeters(MathVector3::Zero());
}

void InputLocator::GetOrientationMatrix33(float* result)
{
    GetMatrixAsArray(result, this->orientationMatrix);
}

void InputLocator::GetPositionVector3(float* result, DistanceUnitType distanceUnitType)
{
    auto positionMeters = this->position.ToUnit(distanceUnitType);
    GetVectorAsArray(result, positionMeters);
}

void InputLocator::GetVelocityVector3(float* result, DistanceUnitType distanceUnitType)
{
    auto velocityMeters = this->velocity.ToUnit(distanceUnitType);
    GetVectorAsArray(result, velocityMeters);
}

size_t InputLocator::GetIndex() const
{
    return this->index;
}

InputLocator& InputLocator::SetIndex(size_t index)
{
    this->index = index;
    return *this;
}

bool InputLocator::HasCode() const
{
    return this->code != InputSource::NO_COMPONENT_CODE;
}

int InputLocator::GetCode() const
{
    return this->code;
}

InputLocator& InputLocator::SetCode(int code)
{
    this->code = code;
    return *this;
}

const Utf8String& InputLocator::GetDisplayName() const
{
    return this->displayName;
}

InputComponentSemantic InputLocator::GetSemantic() const
{
    return this->semantic;
}

InputLocator& InputLocator::SetSemantic(InputComponentSemantic value)
{
    this->semantic = value;
    return *this;
}

bool InputLocator::IsEnabled() const
{
    return this->isEnabled;
}

InputLocator& InputLocator::Enable(bool value)
{
    this->isEnabled = value;
    return *this;
}


//InputTouchScreen::Pointer
InputTouchScreen::Pointer::Pointer()
{
    Reset(true);
}

void InputTouchScreen::Pointer::Reset(bool isConstructing)
{
    this->screen = nullptr;

    this->id = 0;
    this->pointerType = PointerType::NONE;

    this->isEnabled = false;

    this->contact.Reset();
    for (auto& axis : this->axes)
        axis.Reset();

    this->contact.SetDisplayName("Contact");
    this->axes[0].SetIndex(0).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDisplayName("X");
    this->axes[1].SetIndex(1).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDisplayName("Y");
}

void InputTouchScreen::Pointer::Update(bool down, float x, float y)
{
    ////FINJIN_DEBUG_LOG_INFO("Entering InputTouchScreen::Pointer::Update(%1%, %2%, %3%)", (int)down, x, y);

    this->contact.Update(down);
    this->axes[0].Update(x, this->contact.IsFirstDown());
    this->axes[1].Update(y, this->contact.IsFirstDown());

    if (this->contact.Changed() || this->axes[0].Changed() || this->axes[1].Changed())
        this->screen->HandleChangedPointer(this);
}

const InputButton& InputTouchScreen::Pointer::GetContact() const
{
    return this->contact;
}

const InputAxis& InputTouchScreen::Pointer::GetX() const
{
    return this->axes[0];
}

const InputAxis& InputTouchScreen::Pointer::GetY() const
{
    return this->axes[1];
}

size_t InputTouchScreen::Pointer::GetButtonCount() const
{
    return 1;
}

InputButton* InputTouchScreen::Pointer::GetButton(size_t index)
{
    return &this->contact;
}

size_t InputTouchScreen::Pointer::GetAxisCount() const
{
    return this->axes.size();
}

InputAxis* InputTouchScreen::Pointer::GetAxis(size_t index)
{
    return &this->axes[index];
}

bool InputTouchScreen::Pointer::Changed() const
{
    return this->contact.Changed() || this->axes[0].Changed() || this->axes[1].Changed();
}

void InputTouchScreen::Pointer::ClearChanged()
{
    this->contact.ClearChanged();

    for (auto& axis : this->axes)
        axis.ClearChanged();
}

int InputTouchScreen::Pointer::GetID() const
{
    return this->id;
}

PointerType InputTouchScreen::Pointer::GetPointerType() const
{
    return this->pointerType;
}

//InputTouchScreen::Multitouch
InputTouchScreen::Multitouch::Multitouch()
{
    Reset(true);
}

void InputTouchScreen::Multitouch::Reset(bool isConstructing)
{
    this->contact.Reset();
    this->contact.SetDisplayName("Contact");

    this->centerAxes[0].Reset();
    this->centerAxes[0].SetIndex(0).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDisplayName("Center X");

    this->centerAxes[1].Reset();
    this->centerAxes[1].SetIndex(1).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDisplayName("Center Y");

    this->radius.Reset();
    this->radius.SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDisplayName("Radius");

    this->previousDraggingPointerCount = 0;
    this->draggingPointerCount = 0;
}

bool InputTouchScreen::Multitouch::Changed() const
{
    return this->contact.Changed() || this->centerAxes[0].Changed() || this->centerAxes[1].Changed() || this->radius.Changed();
}

void InputTouchScreen::Multitouch::ClearChanged()
{
    this->contact.ClearChanged();

    this->centerAxes[0].ClearChanged();
    this->centerAxes[1].ClearChanged();

    this->radius.ClearChanged();
}

//InputTouchScreen
InputTouchScreen::InputTouchScreen()
{
    Reset(true);
}

void InputTouchScreen::Reset(bool isConstructing)
{
    this->index = 0;
    this->displayName.clear();

    this->isConnected = true;

    this->previousTouchCount.Reset(0);
    this->touchCount = 0;

    for (size_t i = 0; i < this->allPointers.size(); i++)
        this->allPointers[i].Reset();
    this->pointers.clear();

    this->multitouch.Reset();
}

InputTouchScreen::Pointer* InputTouchScreen::ConnectPointer(PointerType pointerType, int pointerID)
{
    for (size_t i = 0; i < this->allPointers.size(); i++)
    {
        auto pointer = &this->allPointers[i];
        if (!pointer->isEnabled)
        {
            pointer->screen = this;

            pointer->id = pointerID;
            pointer->pointerType = pointerType;

            pointer->isEnabled = true;

            this->pointers.push_back(pointer);

            return pointer;
        }
    }

    return nullptr;
}

InputTouchScreen::Pointer* InputTouchScreen::GetPointer(PointerType pointerType, int pointerID)
{
    for (auto pointer : this->pointers)
    {
        if (pointer->pointerType == pointerType && pointer->id == pointerID)
            return pointer;
    }

    return nullptr;
}

size_t InputTouchScreen::GetPointerCount() const
{
    return this->pointers.size();
}

InputTouchScreen::Pointer* InputTouchScreen::GetPointer(size_t index)
{
    return this->pointers[index];
}

size_t InputTouchScreen::GetPreviousTouchCount() const
{
    return this->previousTouchCount;
}

size_t InputTouchScreen::GetTouchCount() const
{
    return this->touchCount;
}

const InputAxis& InputTouchScreen::GetMultitouchRadius() const
{
    return this->multitouch.radius;
}

size_t InputTouchScreen::GetMultitouchAxisCount() const
{
    return 2;
}

InputAxis* InputTouchScreen::GetMultitouchAxis(size_t index)
{
    return &this->multitouch.centerAxes[index];
}

bool InputTouchScreen::TouchCountChanged() const
{
    if (!this->previousTouchCount.IsSet())
        return false;

    return this->touchCount != this->previousTouchCount.value;
}

bool InputTouchScreen::PointersChanged() const
{
    for (auto pointer : this->pointers)
    {
        if (pointer->Changed())
            return true;
    }

    return false;
}

bool InputTouchScreen::Changed() const
{
    return TouchCountChanged() || PointersChanged();
}

void InputTouchScreen::ClearChanged()
{
    this->previousTouchCount = this->touchCount;

    for (size_t i = 0; i < this->pointers.size();)
    {
        if (!this->pointers[i]->GetContact().GetValue())
        {
            //Pointer is no longer down, so reset/remove it
            this->pointers[i]->Reset();
            this->pointers.erase(&this->pointers[i]);
        }
        else
        {
            this->pointers[i]->ClearChanged();
            i++;
        }
    }

    this->multitouch.ClearChanged();
}

size_t InputTouchScreen::GetIndex() const
{
    return this->index;
}

InputTouchScreen& InputTouchScreen::SetIndex(size_t index)
{
    this->index = index;
    return *this;
}

const Utf8String& InputTouchScreen::GetDisplayName() const
{
    return this->displayName;
}

bool InputTouchScreen::IsConnected() const
{
    return this->isConnected;
}

InputTouchScreen& InputTouchScreen::Connect(bool value)
{
    this->isConnected = value;
    return *this;
}

void InputTouchScreen::HandleChangedPointer(Pointer* pointer)
{
    ////FINJIN_DEBUG_LOG_INFO("Entering InputTouchScreen::HandleChangedPointer(id=%1%, changedContact=%2%)", pointer->id, (int)pointer->GetContact().Changed());

    if (pointer->GetContact().Changed())
    {
        //A screen contact was changed

        this->previousTouchCount = this->touchCount;

        if (pointer->GetContact().GetValue())
        {
            //New touch
            this->touchCount++;

            if (this->touchCount == 2)
            {
                //Second touch (first multitouch)
                this->multitouch.contact.Update(true, true);
                GetMultitouchInfo(&this->pointers[0], this->pointers.size(), this->multitouch.centerAxes, this->multitouch.radius, true);
            }
            else if (touchCount > 2)
            {
                //Third, fourth... touch
                GetMultitouchInfo(&this->pointers[0], this->pointers.size(), this->multitouch.centerAxes, this->multitouch.radius, false);
            }
        }
        else
        {
            //Touch release
            this->touchCount--;

            if (this->touchCount == 1)
            {
                //Multitouch ended
                this->multitouch.contact.Update(false);

                this->multitouch.centerAxes[0].Update(pointer->GetX().GetAbsoluteValue(), true);
                this->multitouch.centerAxes[1].Update(pointer->GetY().GetAbsoluteValue(), true);
                this->multitouch.radius.Update(0, true);
            }
            else if (this->touchCount > 1)
            {
                //Continuing multitouch
                GetMultitouchInfo(&this->pointers[0], this->pointers.size(), this->multitouch.centerAxes, this->multitouch.radius, false);
            }
        }

        //if (this->touchCount >= 2)
          //  //FINJIN_DEBUG_LOG_INFO("Multitouch count %1%: %2% x %3%, radius: %4%", this->touchCount, this->multitouch.centerAxes[0].GetValue(), this->multitouch.centerAxes[1].GetValue(), this->multitouch.radius.GetAbsoluteValue());
    }
    else
    {
        //Pointer position was changed
        if (this->touchCount >= 2)
            GetMultitouchInfo(&this->pointers[0], this->pointers.size(), this->multitouch.centerAxes, this->multitouch.radius, false);
    }
}

//InputAccelerometer
const float InputAccelerometer::ON_TABLE_DEAD_ZONE = 0.085f;
const float InputAccelerometer::IN_HAND_DEAD_ZONE = 1.5f;

InputAccelerometer::InputAccelerometer()
{
    Reset(true);
}

void InputAccelerometer::Reset(bool isConstructing)
{
    this->index = 0;
    this->displayName.clear();

    this->isEnabled = true;

    for (auto& axis : this->axes)
        axis.Reset();

    this->axes[0].SetIndex(0).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDeadZone(IN_HAND_DEAD_ZONE).SetDisplayName("X");
    this->axes[1].SetIndex(1).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDeadZone(IN_HAND_DEAD_ZONE).SetDisplayName("Y");
    this->axes[2].SetIndex(2).SetProcessing(InputAxisProcessing::IS_ABSOLUTE).SetDeadZone(IN_HAND_DEAD_ZONE).SetDisplayName("Z");
}

void InputAccelerometer::Update(float x, float y, float z, bool isFirstUpdate)
{
    this->axes[0].Update(x, isFirstUpdate);
    this->axes[1].Update(y, isFirstUpdate);
    this->axes[2].Update(z, isFirstUpdate);
}

const InputAxis& InputAccelerometer::GetX() const
{
    return this->axes[0];
}

const InputAxis& InputAccelerometer::GetY() const
{
    return this->axes[1];
}

const InputAxis& InputAccelerometer::GetZ() const
{
    return this->axes[2];
}

size_t InputAccelerometer::GetAxisCount() const
{
    return this->axes.size();
}

InputAxis* InputAccelerometer::GetAxis(size_t index)
{
    return &this->axes[index];
}

bool InputAccelerometer::Changed() const
{
    return this->axes[0].Changed() || this->axes[1].Changed() || this->axes[2].Changed();
}

void InputAccelerometer::ClearChanged()
{
    for (auto& axis : this->axes)
        axis.ClearChanged();
}

size_t InputAccelerometer::GetIndex() const
{
    return this->index;
}

InputAccelerometer& InputAccelerometer::SetIndex(size_t index)
{
    this->index = index;
    return *this;
}

const Utf8String& InputAccelerometer::GetDisplayName() const
{
    return this->displayName;
}

bool InputAccelerometer::IsEnabled() const
{
    return this->isEnabled;
}

InputAccelerometer& InputAccelerometer::Enable(bool value)
{
    this->isEnabled = value;
    return *this;
}

float InputAccelerometer::GetDeadZone() const
{
    //All axes have same dead zone
    return this->axes[0].GetDeadZone();
}

InputAccelerometer& InputAccelerometer::SetDeadZone(float deadZone)
{
    for (auto& axis : this->axes)
        axis.SetDeadZone(deadZone);

    return *this;
}
