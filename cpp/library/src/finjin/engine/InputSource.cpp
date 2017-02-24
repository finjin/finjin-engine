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
#include "InputSource.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Math.hpp"

#define DEFAULT_DEVICE_INDEX 0

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------

//InputSource
InputSource::InputSource()
{
    Reset();
}

void InputSource::Reset()
{
    this->deviceComponent = InputDeviceComponent::NONE;
    this->semantic = InputComponentSemantic::NONE;
    this->buttonIndex = (size_t)-1;
    this->buttonCode = NO_COMPONENT_CODE;
    this->direction = 0;
    this->directionFloat = 0;
    this->touchCount = 0;
    this->deviceIndex = 0;
}

InputSource InputSource::FromKeyboardKey(size_t keyIndex)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::KEYBOARD_KEY;
    source.keyIndex = keyIndex;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromMouseButton(size_t buttonIndex)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::MOUSE_BUTTON;
    source.buttonIndex = buttonIndex;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromMouseRelativeAxis(size_t axisIndex, int direction)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::MOUSE_RELATIVE_AXIS;
    source.axisIndex = axisIndex;
    source.direction = direction;
    source.directionFloat = (float)direction;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromMouseAbsoluteAxis(size_t axisIndex, int direction)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::MOUSE_ABSOLUTE_AXIS;
    source.axisIndex = axisIndex;
    source.direction = direction;
    source.directionFloat = (float)direction;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromGameControllerButton(size_t buttonIndex)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::GAME_CONTROLLER_BUTTON;
    source.buttonIndex = buttonIndex;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromGameControllerAxis(size_t axisIndex, int direction)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::GAME_CONTROLLER_AXIS;
    source.axisIndex = axisIndex;
    source.direction = direction;
    source.directionFloat = (float)direction;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromGameControllerPov(size_t povIndex, PovDirection direction)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::GAME_CONTROLLER_POV;
    source.povIndex = povIndex;
    source.direction = ToNumber(direction);
    source.directionFloat = (float)direction;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromGameControllerLocator(size_t locatorIndex)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::GAME_CONTROLLER_LOCATOR;
    source.locatorIndex = locatorIndex;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromTouchCount(size_t touchCount)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::TOUCH_COUNT;
    source.touchCount = touchCount;    
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromTouchRelativeAxis(size_t axisIndex, int direction)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::TOUCH_RELATIVE_AXIS;
    source.axisIndex = axisIndex;
    source.direction = direction;
    source.directionFloat = (float)direction;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromTouchAbsoluteAxis(size_t axisIndex, int direction)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::TOUCH_ABSOLUTE_AXIS;
    source.axisIndex = axisIndex;
    source.direction = direction;
    source.directionFloat = (float)direction;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromMultitouchRelativeRadius(size_t touchCount, int direction)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS;
    source.direction = direction;
    source.directionFloat = (float)direction;
    source.touchCount = touchCount;    
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromMultitouchRelativeAxis(size_t touchCount, size_t axisIndex, int direction)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS;
    source.axisIndex = axisIndex;
    source.direction = direction;
    source.directionFloat = (float)direction;
    source.touchCount = touchCount;    
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromAccelerometerRelativeAxis(size_t axisIndex, int direction)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::ACCELEROMETER_RELATIVE_AXIS;
    source.axisIndex = axisIndex;
    source.direction = direction;
    source.directionFloat = (float)direction;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromAccelerometerAbsoluteAxis(size_t axisIndex, int direction)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::ACCELEROMETER_ABSOLUTE_AXIS;
    source.axisIndex = axisIndex;
    source.direction = direction;
    source.directionFloat = (float)direction;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::FromHeadsetLocator(size_t locatorIndex)
{
    InputSource source;
    source.deviceComponent = InputDeviceComponent::HEADSET_LOCATOR;
    source.locatorIndex = locatorIndex;
    source.deviceIndex = DEFAULT_DEVICE_INDEX;
    return source;
}

InputSource InputSource::WithDeviceIndex(size_t index) const
{
    InputSource source = *this;
    source.deviceIndex = index;
    return source;
}

InputSource InputSource::WithCode(int code) const
{
    InputSource source = *this;
    source.keyCode = code;
    return source;
}

InputSource InputSource::WithSemantic(InputComponentSemantic semantic) const
{
    InputSource source = *this;
    source.semantic = semantic;
    return source;
}

Utf8String InputSource::GetDeviceComponentString() const
{
    return InputDeviceComponentUtilities::ToString(this->deviceComponent);
}

Utf8String InputSource::GetSemanticString() const
{
    return InputComponentSemanticUtilities::ToString(this->semantic);
}

bool InputSource::HasDeviceComponentCode() const
{
    return this->keyCode != NO_COMPONENT_CODE;
}

Utf8String InputSource::ToString() const
{
    switch (this->deviceComponent)
    {
        case InputDeviceComponent::KEYBOARD_KEY: return Utf8StringFormatter::Format("Device: Keyboard %1%, Key code: %2%, Key index: %3%", this->deviceIndex, this->keyCode, this->keyIndex);
        
        case InputDeviceComponent::MOUSE_BUTTON: return Utf8StringFormatter::Format("Device: Mouse %1%, Button code: %2%, Button index: %3%", this->deviceIndex, this->buttonCode, this->buttonIndex);
        case InputDeviceComponent::MOUSE_RELATIVE_AXIS: return Utf8StringFormatter::Format("Device: Mouse (relative) %1%, Axis code: %2%, Axis index: %3%, Direction: %4%", this->deviceIndex, this->axisCode, this->axisIndex, this->direction);
        case InputDeviceComponent::MOUSE_ABSOLUTE_AXIS: return Utf8StringFormatter::Format("Device: Mouse (absolute) %1%, Axis code: %2%, Axis index: %3%, Direction: %4%", this->deviceIndex, this->axisCode, this->axisIndex, this->direction);
        
        case InputDeviceComponent::GAME_CONTROLLER_BUTTON: return Utf8StringFormatter::Format("Device: Game controller %1%, Button code: %2%, Button index: %3%", this->deviceIndex, this->buttonCode, this->buttonIndex);
        case InputDeviceComponent::GAME_CONTROLLER_AXIS: return Utf8StringFormatter::Format("Device: Game controller %1%, Axis code: %2%, Axis index: %3%, Direction: %4%", this->deviceIndex, this->axisCode, this->axisIndex, this->direction);
        case InputDeviceComponent::GAME_CONTROLLER_POV: return Utf8StringFormatter::Format("Device: Game controller %1%, POV code: %2%, POV index: %3%, Direction: %4%", this->deviceIndex, this->povCode, this->povIndex, this->direction);
        case InputDeviceComponent::GAME_CONTROLLER_LOCATOR: return Utf8StringFormatter::Format("Device: Game controller %1%: Locator", this->deviceIndex);
        
        case InputDeviceComponent::TOUCH_COUNT: return Utf8StringFormatter::Format("Device: Touch count. Count: %1%", this->touchCount);
        case InputDeviceComponent::TOUCH_RELATIVE_AXIS: return Utf8StringFormatter::Format("Device: Touch (relative), Axis index: %1%, Direction: %2%", this->axisIndex, this->direction);
        case InputDeviceComponent::TOUCH_ABSOLUTE_AXIS: return Utf8StringFormatter::Format("Device: Touch (absolute), Axis index: %1%, Direction: %2%", this->axisIndex, this->direction);
        
        case InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS: return Utf8StringFormatter::Format("Device: Radius (relative), Count: %1%, Direction: %2%", this->touchCount, this->direction);
        case InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS: return Utf8StringFormatter::Format("Device: Multitouch (relative), Axis index: %1%, Direction: %2%", this->axisIndex, this->direction);
        
        case InputDeviceComponent::ACCELEROMETER_RELATIVE_AXIS: return Utf8StringFormatter::Format("Device: Accelerometer (relative), Axis index: %1%, Direction: %2%", this->axisIndex, this->direction);
        case InputDeviceComponent::ACCELEROMETER_ABSOLUTE_AXIS: return Utf8StringFormatter::Format("Device: Accelerometer (absolute), Axis index: %1%, Direction: %2%", this->axisIndex, this->direction);
        
        case InputDeviceComponent::HEADSET_LOCATOR: return Utf8StringFormatter::Format("Device: Headset %1%: Locator", this->deviceIndex);
        
        default: return "Device: None"; break;
    }
}

//ChangedInputSource
ChangedInputSource::ChangedInputSource()
{
    this->inputSource.deviceIndex = (size_t)-1;
}

ChangedInputSource::ChangedInputSource(InputSource inputSource, Utf8String name)
{
    this->inputSource = inputSource;
    this->name = name;
    this->value = 0.0f;
}

ChangedInputSource::ChangedInputSource(InputSource inputSource, Utf8String name, bool value)
{
    this->inputSource = inputSource;
    this->name = name;
    this->value = value ? 1.0f : 0.0f;
}

ChangedInputSource::ChangedInputSource(InputSource inputSource, Utf8String name, float value)
{
    this->inputSource = inputSource;
    this->name = name;
    this->value = value;
}

ChangedInputSource::ChangedInputSource(InputSource inputSource, Utf8String name, PovDirection value)
{
    this->inputSource = inputSource;
    this->name = name;
    this->value = RoundToFloat(ToNumber(value));
}

ChangedInputSource::operator bool() const
{
    return this->inputSource.deviceIndex != (size_t)-1;
}

Utf8String ChangedInputSource::ToString() const
{
    Utf8String s;

    if (*this)
    {
        s = "'";
        s += this->name;
        s += "' = ";
        s += Convert::ToString(this->value);
        s += ": ";
        s += this->inputSource.ToString();
    }

    return s;
}
