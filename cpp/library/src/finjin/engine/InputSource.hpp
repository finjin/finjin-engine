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
#include "finjin/common/Math.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/InputComponents.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    /**
     * A single component of an input device, such as a keyboard key or joystick button.
     * You should not attempt to construct an instance of this class manually. 
     * Instead, always use the "From" functions
     */
    class InputSource
    {
    public:
        enum { NO_COMPONENT_CODE = std::numeric_limits<int>::lowest() };
        
        InputSource();

        void Reset();

        static InputSource FromKeyboardKey(size_t keyIndex);
        
        static InputSource FromMouseButton(size_t buttonIndex);
        static InputSource FromMouseRelativeAxis(size_t axisIndex, int direction);
        static InputSource FromMouseAbsoluteAxis(size_t axisIndex, int direction);
        
        static InputSource FromGameControllerButton(size_t buttonIndex);
        static InputSource FromGameControllerAxis(size_t axisIndex, int direction);
        static InputSource FromGameControllerPov(size_t povIndex, PovDirection direction);
        static InputSource FromGameControllerLocator(size_t locatorIndex = 0);
        
        static InputSource FromTouchCount(size_t touchCount);
        static InputSource FromTouchRelativeAxis(size_t axisIndex, int direction);
        static InputSource FromTouchAbsoluteAxis(size_t axisIndex, int direction);
        
        static InputSource FromMultitouchRelativeRadius(size_t touchCount, int direction);
        static InputSource FromMultitouchRelativeAxis(size_t touchCount, size_t axis, int direction);
        
        static InputSource FromAccelerometerRelativeAxis(size_t axisIndex, int direction);
        static InputSource FromAccelerometerAbsoluteAxis(size_t axisIndex, int direction);
        
        static InputSource FromHeadsetLocator(size_t locatorIndex = 0);
        
        InputSource WithDeviceIndex(size_t index) const;
        InputSource WithCode(int code) const;
        InputSource WithSemantic(InputComponentSemantic semantic) const;

        Utf8String GetDeviceComponentString() const;
        Utf8String GetSemanticString() const;

        bool HasDeviceComponentCode() const;

        /** Gets a string containing the input source information. Useful for testing */
        Utf8String ToString() const;

    public:
        /** Device component(s) */
        InputDeviceComponent deviceComponent;

        InputComponentSemantic semantic;

        /* The device component */
        union
        {
            int code;

            /** Keyboard key */
            int keyCode;

            /** Mouse or joystick button */
            int buttonCode;

            /** Mouse or joystick axis */
            int axisCode;

            /** Game controller POV */
            int povCode;

            /** Locator locator */
            int locatorCode;
        };
        union
        {
            size_t index;

            /** Keyboard key */
            size_t keyIndex;

            /** Mouse or joystick button */
            size_t buttonIndex;

            /** Mouse or joystick axis */
            size_t axisIndex;

            /** Game controller POV index */
            size_t povIndex;

            /** Game controller or headset locator index */
            size_t locatorIndex;
        };
        
        /** Mouse axis direction, game controller axis direction, or game controller pov direction */
        union
        {
            int direction;
            PovDirection povDirection;
        };
        float directionFloat;

        /** The number of touches. It's not within the union because multitouch axis mapping requires the axis member. */
        size_t touchCount;

        /** Index of the device. */
        size_t deviceIndex;
    };

    struct ChangedInputSourceFilter
    {
        ChangedInputSourceFilter()
        {
            SetDefaultDeadZones();
            this->deviceClasses = InputDeviceClass::ALL;
            this->componentClasses = InputDeviceComponentClass::ALL;
            this->unifyRelativeAxisDirections = true;
        }
        
        void SetDefaultDeadZones()
        {
            this->mouseMoveAxisDeadZone = 10;
            this->gameControllerAxisDeadZone = .4f;
            this->touchScreenAxisDeadZone = 10;
            this->touchScreenMultitouchRadiusDeadZone = 3;
            this->accelerometerDeadZone = 1.25f;
        }
        
        void SetMinimalDeadZones()
        {
            this->mouseMoveAxisDeadZone = 1;
            this->gameControllerAxisDeadZone = .2f;
            this->touchScreenAxisDeadZone = 3;
            this->touchScreenMultitouchRadiusDeadZone = 1;
            this->accelerometerDeadZone = .5f;
        }

        template <typename T>
        float UnifyDirection(float value, T direction) const
        {
            if (this->unifyRelativeAxisDirections)
                value *= RoundToFloat(direction);
            return value;
        }

        float mouseMoveAxisDeadZone;
        float gameControllerAxisDeadZone;
        float touchScreenAxisDeadZone;
        float touchScreenMultitouchRadiusDeadZone;
        float accelerometerDeadZone;
        InputDeviceClass deviceClasses;
        InputDeviceComponentClass componentClasses;
        bool unifyRelativeAxisDirections;
    };

    struct ChangedInputSource
    {
        ChangedInputSource();
        ChangedInputSource(InputSource inputSource, Utf8String name);
        ChangedInputSource(InputSource inputSource, Utf8String name, bool value);
        ChangedInputSource(InputSource inputSource, Utf8String name, float value);        
        ChangedInputSource(InputSource inputSource, Utf8String name, PovDirection value);

        operator bool() const;

        Utf8String ToString() const;

        InputSource inputSource;
        Utf8String name;

        float value;
    };

    template <size_t maxCount>
    struct ChangedInputSources : public StaticVector<ChangedInputSource, maxCount>
    {
        using Super = StaticVector<ChangedInputSource, maxCount>;

        template <typename Device>
        void CheckGameController(Device* gameController, size_t deviceIndex, const ChangedInputSourceFilter& filter)
        {
            //Buttons
            if (AnySet(filter.componentClasses & InputDeviceComponentClass::BUTTON))
            {
                auto buttonCount = gameController->GetButtonCount();
                for (size_t buttonIndex = 0; buttonIndex < buttonCount; buttonIndex++)
                {
                    auto component = gameController->GetButton(buttonIndex);
                    if (component->Changed() && component->IsFirstDown())
                        Super::push_back(ChangedInputSource(InputSource::FromGameControllerButton(buttonIndex).WithDeviceIndex(deviceIndex).WithCode(component->GetCode()).WithSemantic(component->GetSemantic()), component->GetDisplayName(), component->GetValue()));
                }
            }

            //Axes
            if (AnySet(filter.componentClasses & InputDeviceComponentClass::AXIS))
            {
                auto axisCount = gameController->GetAxisCount();
                for (size_t axisIndex = 0; axisIndex < axisCount; axisIndex++)
                {
                    auto component = gameController->GetAxis(axisIndex);
                    if (component->Changed() && component->IsFirstDown(filter.gameControllerAxisDeadZone))
                    {
                        auto value = filter.UnifyDirection(component->GetValue(), component->GetDirection());
                        Super::push_back(ChangedInputSource(InputSource::FromGameControllerAxis(axisIndex, value < 0 ? -1 : 1).WithDeviceIndex(deviceIndex).WithCode(component->GetCode()).WithSemantic(component->GetSemantic()), component->GetDisplayName(), value));
                    }
                }
            }

            //POV
            if (AnySet(filter.componentClasses & InputDeviceComponentClass::POV))
            {
                auto povCount = gameController->GetPovCount();
                for (size_t povIndex = 0; povIndex < povCount; povIndex++)
                {
                    auto component = gameController->GetPov(povIndex);
                    if (component->Changed() && component->IsFirstDown(true))
                        Super::push_back(ChangedInputSource(InputSource::FromGameControllerPov(povIndex, component->GetValue()).WithDeviceIndex(deviceIndex).WithCode(component->GetCode()).WithSemantic(component->GetSemantic()), component->GetDisplayName(), component->GetValue()));
                }
            }
        }

        template <typename Device>
        void CheckMouse(Device* mouse, size_t deviceIndex, const ChangedInputSourceFilter& filter)
        {
            //Buttons
            if (AnySet(filter.componentClasses & InputDeviceComponentClass::BUTTON))
            {
                auto buttonCount = mouse->GetButtonCount();
                for (size_t buttonIndex = 0; buttonIndex < buttonCount; buttonIndex++)
                {
                    auto component = mouse->GetButton(buttonIndex);
                    if (component->Changed() && component->IsFirstDown())
                        Super::push_back(ChangedInputSource(InputSource::FromMouseButton(buttonIndex).WithDeviceIndex(deviceIndex).WithCode(component->GetCode()).WithSemantic(component->GetSemantic()), component->GetDisplayName(), component->GetValue()));
                }
            }

            //Axes
            if (AnySet(filter.componentClasses & InputDeviceComponentClass::AXIS))
            {
                auto axisCount = mouse->GetAxisCount();
                for (size_t axisIndex = 0; axisIndex < axisCount; axisIndex++)
                {
                    auto component = mouse->GetAxis(axisIndex);
                    if (component->Changed() && component->IsFirstDown(filter.mouseMoveAxisDeadZone))
                    {
                        auto value = filter.UnifyDirection(component->GetRelativeValue(), component->GetDirection());
                        Super::push_back(ChangedInputSource(InputSource::FromMouseRelativeAxis(axisIndex, value < 0 ? -1 : 1).WithDeviceIndex(deviceIndex).WithCode(component->GetCode()).WithSemantic(component->GetSemantic()), component->GetDisplayName(), value));
                    }
                }
            }
        }

        template <typename Device>
        void CheckKeyboard(Device* keyboard, size_t deviceIndex, const ChangedInputSourceFilter& filter)
        {
            //Keys
            if (AnySet(filter.componentClasses & InputDeviceComponentClass::BUTTON))
            {
                auto keyCount = keyboard->GetButtonCount();
                for (size_t keyIndex = 0; keyIndex < keyCount; keyIndex++)
                {
                    auto component = keyboard->GetButton(keyIndex);
                    if (component->Changed() && component->IsFirstDown())
                        Super::push_back(ChangedInputSource(InputSource::FromKeyboardKey(keyIndex).WithDeviceIndex(deviceIndex).WithCode(component->GetCode()).WithSemantic(component->GetSemantic()), component->GetDisplayName(), component->GetValue()));
                }
            }
        }

        template <typename Device>
        void CheckTouchScreen(Device* touchScreen, size_t deviceIndex, const ChangedInputSourceFilter& filter)
        {
            //Touch count
            if (AnySet(filter.componentClasses & InputDeviceComponentClass::COUNT))
            {
                if (touchScreen->TouchCountChanged() && touchScreen->GetTouchCount() > 0)
                    Super::push_back(ChangedInputSource(InputSource::FromTouchCount(touchScreen->GetTouchCount()).WithDeviceIndex(deviceIndex), touchScreen->GetDisplayName(), RoundToFloat(touchScreen->GetTouchCount())));
            }

            //Axis
            if (AnySet(filter.componentClasses & InputDeviceComponentClass::AXIS))
            {
                auto pointerCount = touchScreen->GetPointerCount();
                for (size_t pointerIndex = 0; pointerIndex < pointerCount; pointerIndex++)
                {
                    auto pointer = touchScreen->GetPointer(pointerIndex);
                    auto axisCount = pointer->GetAxisCount();
                    for (size_t axisIndex = 0; axisIndex < axisCount; axisIndex++)
                    {
                        auto component = pointer->GetAxis(axisIndex);
                        if (component->Changed() && std::abs(component->GetRelativeValue()) > filter.touchScreenAxisDeadZone)
                        {
                            //Note that the touch screen device index is used in WithDeviceIndex(), not the pointer index
                            auto value = filter.UnifyDirection(component->GetRelativeValue(), component->GetDirection());
                            Super::push_back(ChangedInputSource(InputSource::FromTouchRelativeAxis(axisIndex, value < 0 ? -1 : 1).WithDeviceIndex(deviceIndex), touchScreen->GetDisplayName(), value));
                        }
                    }
                }
            }

            //Radius
            if (AnySet(filter.componentClasses & InputDeviceComponentClass::RADIUS))
            {
                if (touchScreen->GetMultitouchRadius().Changed() && std::abs(touchScreen->GetMultitouchRadius().GetRelativeValue()) > filter.touchScreenMultitouchRadiusDeadZone)
                    Super::push_back(ChangedInputSource(InputSource::FromMultitouchRelativeRadius(touchScreen->GetTouchCount(), touchScreen->GetMultitouchRadius().GetRelativeValue() < 0 ? -1 : 1).WithDeviceIndex(deviceIndex), touchScreen->GetDisplayName(), touchScreen->GetMultitouchRadius().GetAbsoluteValue()));
            }
        }

        template <typename Accelerometer>
        void CheckAccelerometer(Accelerometer* accelerometer, size_t deviceIndex, const ChangedInputSourceFilter& filter)
        {
            //Axes
            if (AnySet(filter.componentClasses & InputDeviceComponentClass::AXIS))
            {
                auto axisCount = accelerometer->GetAxisCount();
                for (size_t axisIndex = 0; axisIndex < axisCount; axisIndex++)
                {
                    auto component = accelerometer->GetAxis(axisIndex);
                    if (component->Changed() && component->IsFirstDown(filter.accelerometerDeadZone))
                    {
                        auto value = filter.UnifyDirection(component->GetRelativeValue(), component->GetDirection());
                        Super::push_back(ChangedInputSource(InputSource::FromAccelerometerAbsoluteAxis(axisIndex, value < 0 ? -1 : 1).WithDeviceIndex(deviceIndex), accelerometer->GetDisplayName(), value));
                    }
                }
            }            
        }
    };
    
} }
