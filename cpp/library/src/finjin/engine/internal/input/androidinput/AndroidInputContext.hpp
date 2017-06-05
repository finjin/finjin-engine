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
#include "AndroidGameController.hpp"
#include "AndroidKeyboard.hpp"
#include "AndroidMouse.hpp"
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/engine/GenericInputDevice.hpp"
#include "finjin/engine/InputContextCommonSettings.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class AndroidInputSystem;
    class AndroidGameController;
    class AndroidKeyboard;
    class AndroidMouse;

    using namespace Finjin::Common;

    class AndroidInputContext : public AllocatedClass
    {
    public:
        AndroidInputContext(Allocator* allocator, AndroidInputSystem* inputSystem);
        ~AndroidInputContext();

        struct Settings : InputContextCommonSettings
        {
            Settings(Allocator* initialAllocator) : InputContextCommonSettings(initialAllocator)
            {
                this->useSystemBackButton = false;

                this->useAccelerometer = false;
                this->accelerometerAlooperID = 0;
            }

            bool useSystemBackButton;

            bool useAccelerometer;
            int accelerometerAlooperID;
        };

        void Create(const Settings& settings, Error& error);
        void Destroy();

        void GetSelectorComponents(AssetPathSelector& result);

        const Settings& GetSettings() const;

        void Update(SimpleTimeDelta elapsedTime, InputDevicePollFlag flags = InputDevicePollFlag::NONE);

        void Execute(InputEvents& events, InputCommands& commands, Error& error);

        void HandleDeviceChanges();

        void HandleApplicationViewportLostFocus();
        void HandleApplicationViewportGainedFocus();

        size_t GetDeviceCount(InputDeviceClass deviceClass) const;
        const Utf8String& GetDeviceProductDescriptor(InputDeviceClass deviceClass, size_t index) const;
        const Utf8String& GetDeviceInstanceDescriptor(InputDeviceClass deviceClass, size_t index) const;
        InputDeviceSemantic GetDeviceSemantic(InputDeviceClass deviceClass, size_t index) const;
        bool IsDeviceConnected(InputDeviceClass deviceClass, size_t index) const;

        void AddHapticFeedback(InputDeviceClass deviceClass, size_t index, const HapticFeedback* forces, size_t count);
        void StopHapticFeedback(InputDeviceClass deviceClass, size_t index);

        size_t GetGameControllerCount() const;
        AndroidGameController* GetGameController(size_t index);

        void AddExternalGameController(ExternalGameController* gameController, bool configure, Error& error);
        void RemoveExternalGameController(ExternalGameController* gameController);
        size_t GetExternalGameControllerCount() const;
        ExternalGameController* GetExternalGameController(size_t index);

        size_t GetMouseCount() const;
        AndroidMouse* GetMouse(size_t index);

        size_t GetKeyboardCount() const;
        AndroidKeyboard* GetKeyboard(size_t index);

        size_t GetTouchScreenCount() const;
        InputTouchScreen* GetTouchScreen(size_t index);

        size_t GetAccelerometerCount() const;
        InputAccelerometer* GetAccelerometer(size_t index);

        FileOperationResult ReadInputBinding(InputDeviceClass deviceClass, size_t deviceIndex, const AssetReference& configAssetRef, ByteBuffer& fileBuffer);

        template <typename InputBindings>
        void TranslateInputBindingsSemantics(InputBindings& inputBindings, InputDeviceClass deviceClass, size_t deviceIndex)
        {
            switch (deviceClass)
            {
                case InputDeviceClass::KEYBOARD:
                {
                    if (deviceIndex < GetKeyboardCount())
                        inputBindings.TranslateKeyboardBindings(deviceIndex, GetKeyboard(deviceIndex));
                    break;
                }
                case InputDeviceClass::MOUSE:
                {
                    if (deviceIndex < GetMouseCount())
                        inputBindings.TranslateMouseBindings(deviceIndex, GetMouse(deviceIndex));
                    break;
                }
                case InputDeviceClass::GAME_CONTROLLER:
                {
                    if (deviceIndex < GetGameControllerCount())
                        inputBindings.TranslateGameControllerBindings(deviceIndex, GetGameController(deviceIndex));
                    break;
                }
                case InputDeviceClass::TOUCH_SCREEN:
                {
                    if (deviceIndex < GetTouchScreenCount())
                        inputBindings.TranslateTouchScreenBindings(deviceIndex, GetTouchScreen(deviceIndex));
                    break;
                }
            }
        }

        template <typename Actions, typename InputBindings>
        void GetActions(Actions& actions, InputBindings& inputBindings, SimpleTimeDelta elapsedTimeSeconds)
        {
            auto bindingCount = inputBindings.GetBindingCount();
            for (size_t index = 0; index < bindingCount; index++)
            {
                auto& binding = inputBindings.GetBinding(index);

                auto bindingAllowsSingleTouchWithMultiTouch = AnySet(binding.triggerCriteria.flags & InputTriggerFlag::TOUCH_ALLOWED_WITH_MULTITOUCH);

                switch (binding.inputSource.deviceComponent)
                {
                    case InputDeviceComponent::KEYBOARD_KEY:
                    {
                        if (binding.inputSource.deviceIndex < GetKeyboardCount())
                            inputBindings.ProcessKeyboardKeyBinding(binding, *GetKeyboard(binding.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::MOUSE_BUTTON:
                    {
                        if (binding.inputSource.deviceIndex < GetMouseCount())
                            inputBindings.ProcessMouseButtonBinding(binding, *GetMouse(binding.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::MOUSE_RELATIVE_AXIS:
                    {
                        if (binding.inputSource.deviceIndex < GetMouseCount())
                            inputBindings.ProcessMouseRelativeAxisBinding(binding, *GetMouse(binding.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::MOUSE_ABSOLUTE_AXIS:
                    {
                        if (binding.inputSource.deviceIndex < GetMouseCount())
                            inputBindings.ProcessMouseAbsoluteAxisBinding(binding, *GetMouse(binding.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::GAME_CONTROLLER_BUTTON:
                    {
                        if (binding.inputSource.deviceIndex < GetGameControllerCount())
                            inputBindings.ProcessGameControllerButtonBinding(binding, *GetGameController(binding.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::GAME_CONTROLLER_AXIS:
                    {
                        if (binding.inputSource.deviceIndex < GetGameControllerCount())
                            inputBindings.ProcessGameControllerAxisBinding(binding, *GetGameController(binding.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::TOUCH_COUNT:
                    {
                        if (binding.inputSource.deviceIndex < GetTouchScreenCount())
                            inputBindings.ProcessTouchCountBinding(binding, *GetTouchScreen(binding.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::TOUCH_RELATIVE_AXIS:
                    {
                        if (binding.inputSource.deviceIndex < GetTouchScreenCount())
                        {
                            auto touchScreen = GetTouchScreen(binding.inputSource.deviceIndex);
                            auto pointerCount = touchScreen->GetPointerCount();
                            if (pointerCount > 0 && (bindingAllowsSingleTouchWithMultiTouch || pointerCount < 2))
                                inputBindings.ProcessTouchRelativeAxisBinding(binding, *touchScreen->GetPointer(0), actions, elapsedTimeSeconds);
                        }
                        break;
                    }
                    case InputDeviceComponent::TOUCH_ABSOLUTE_AXIS:
                    {
                        if (binding.inputSource.deviceIndex < GetTouchScreenCount())
                        {
                            auto touchScreen = GetTouchScreen(binding.inputSource.deviceIndex);
                            auto pointerCount = touchScreen->GetPointerCount();
                            if (pointerCount > 0 && (bindingAllowsSingleTouchWithMultiTouch || pointerCount < 2))
                                inputBindings.ProcessTouchAbsoluteAxisBinding(binding, *touchScreen->GetPointer(0), actions, elapsedTimeSeconds);
                        }
                        break;
                    }
                    case InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS:
                    {
                        if (binding.inputSource.deviceIndex < GetTouchScreenCount() && GetTouchScreen(binding.inputSource.deviceIndex)->GetPointerCount() > 1)
                            inputBindings.ProcessMultitouchRelativeRadiusBinding(binding, *GetTouchScreen(binding.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS:
                    {
                        if (binding.inputSource.deviceIndex < GetTouchScreenCount() && GetTouchScreen(binding.inputSource.deviceIndex)->GetPointerCount() > 1)
                            inputBindings.ProcessMultitouchRelativeAxisBinding(binding, *GetTouchScreen(binding.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                }
            }
        }

        template <size_t maxCount>
        void GetChangedInputSources(ChangedInputSources<maxCount>& changed, ChangedInputSourceFilter filter)
        {
            if (AnySet(filter.deviceClasses & InputDeviceClass::GAME_CONTROLLER))
            {
                auto gameControllerCount = GetGameControllerCount();
                for (size_t gameControllerIndex = 0; gameControllerIndex < gameControllerCount; gameControllerIndex++)
                    changed.CheckGameController(GetGameController(gameControllerIndex), gameControllerIndex, filter);
            }

            if (AnySet(filter.deviceClasses & InputDeviceClass::KEYBOARD))
            {
                auto keyboardCount = GetKeyboardCount();
                for (size_t keyboardIndex = 0; keyboardIndex < keyboardCount; keyboardIndex++)
                    changed.CheckKeyboard(GetKeyboard(keyboardIndex), keyboardIndex, filter);
            }

            if (AnySet(filter.deviceClasses & InputDeviceClass::MOUSE))
            {
                auto mouseCount = GetMouseCount();
                for (size_t mouseIndex = 0; mouseIndex < mouseCount; mouseIndex++)
                    changed.CheckMouse(GetMouse(mouseIndex), mouseIndex, filter);
            }

            if (AnySet(filter.deviceClasses & InputDeviceClass::TOUCH_SCREEN))
            {
                auto touchScreenCount = GetTouchScreenCount();
                for (size_t touchScreenIndex = 0; touchScreenIndex < touchScreenCount; touchScreenIndex++)
                    changed.CheckTouchScreen(GetTouchScreen(touchScreenIndex), touchScreenIndex, filter);
            }

            if (AnySet(filter.deviceClasses & InputDeviceClass::ACCELEROMETER))
            {
                auto accelerometerCount = GetAccelerometerCount();
                for (size_t accelerometerIndex = 0; accelerometerIndex < accelerometerCount; accelerometerIndex++)
                    changed.CheckAccelerometer(GetAccelerometer(accelerometerIndex), accelerometerIndex, filter);
            }
        }

        int32_t HandleApplicationInputEvent(void* androidInputEvent);
        bool ProcessQueue(int ident);

        struct Impl;
        Impl* GetImpl();

    private:
        std::unique_ptr<Impl> impl;
    };

} }
