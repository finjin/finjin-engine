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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/engine/InputContextCommonSettings.hpp"
#include "finjin/engine/InputSource.hpp"
#include "../xinput/XInputGameController.hpp"
#include "WindowsUWPKeyboard.hpp"
#include "WindowsUWPMouse.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class WindowsUWPInputSystem;

    using namespace Finjin::Common;

    class WindowsUWPInputContext : public AllocatedClass
    {
    public:
        WindowsUWPInputContext(Allocator* allocator, WindowsUWPInputSystem* inputSystem);
        ~WindowsUWPInputContext();

        struct Settings : InputContextCommonSettings
        {
            Settings(Allocator* initialAllocator) : InputContextCommonSettings(initialAllocator)
            {
            }
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

        void AddHapticFeedback(InputDeviceClass deviceClass, size_t index, const HapticFeedbackSettings* forces, size_t count);
        void StopHapticFeedback(InputDeviceClass deviceClass, size_t index);

        size_t GetGameControllerCount() const;
        bool IsGameControllerConnected(size_t index) const;
        const Utf8String& GetGameControllerProductDescriptor(size_t index) const;
        const Utf8String& GetGameControllerInstanceDescriptor(size_t index) const;
        InputDeviceSemantic GetGameControllerSemantic(size_t index) const;

        size_t GetXInputGameControllerCount() const;
        XInputGameController* GetXInputGameController(size_t index);

        void AddExternalGameController(InputGenericGameController* gameController, bool configure, Error& error);
        void RemoveExternalGameController(InputGenericGameController* gameController);
        size_t GetExternalGameControllerCount() const;
        InputGenericGameController* GetExternalGameController(size_t index);

        size_t GetMouseCount() const;
        WindowsUWPMouse* GetMouse(size_t index);

        size_t GetKeyboardCount() const;
        WindowsUWPKeyboard* GetKeyboard(size_t index);

        size_t GetTouchScreenCount() const;
        InputTouchScreen* GetTouchScreen(size_t index);

        void AddExternalHeadset(InputGenericHeadset* headset, bool configure, Error& error);
        void RemoveExternalHeadset(InputGenericHeadset* headset);
        size_t GetExternalHeadsetCount() const;
        InputGenericHeadset* GetExternalHeadset(size_t index);

        FileOperationResult ReadInputBinding(InputDeviceClass deviceClass, size_t deviceIndex, const AssetReference& configAssetRef, ByteBuffer& fileBuffer);

        template <typename InputBindings>
        void TranslateInputBindingsSemantics(InputBindings& inputBindings, InputDeviceClass deviceClass, size_t deviceIndex)
        {
            switch (deviceClass)
            {
                case InputDeviceClass::KEYBOARD:
                {
                    inputBindings.TranslateKeyboardBindings(deviceIndex, GetKeyboard(deviceIndex));
                    break;
                }
                case InputDeviceClass::MOUSE:
                {
                    inputBindings.TranslateMouseBindings(deviceIndex, GetMouse(deviceIndex));
                    break;
                }
                case InputDeviceClass::GAME_CONTROLLER:
                {
                    if (deviceIndex < GetXInputGameControllerCount())
                        inputBindings.TranslateGameControllerBindings(deviceIndex, GetXInputGameController(deviceIndex));
                    else
                        inputBindings.TranslateGameControllerBindings(deviceIndex, GetExternalGameController(deviceIndex - GetXInputGameControllerCount()));
                    break;
                }
                case InputDeviceClass::TOUCH_SCREEN:
                {
                    if (deviceIndex < GetTouchScreenCount())
                        inputBindings.TranslateTouchScreenBindings(deviceIndex, GetTouchScreen(deviceIndex));
                    break;
                }
                case InputDeviceClass::HEADSET:
                {
                    if (deviceIndex < GetExternalHeadsetCount())
                        inputBindings.TranslateHeadsetBindings(deviceIndex, GetExternalHeadset(deviceIndex));
                    break;
                }
            }
        }

        template <typename Actions, typename InputBindings>
        void GetActions(Actions& actions, InputBindings& inputBindings, SimpleTimeDelta elapsedTimeSeconds)
        {
            auto mappingCount = inputBindings.GetBindingCount();
            for (size_t index = 0; index < mappingCount; index++)
            {
                auto& mapping = inputBindings.GetBinding(index);

                auto mappingAllowsSingleTouchWithMultiTouch = AnySet(mapping.triggerCriteria.flags & InputTriggerFlag::TOUCH_ALLOWED_WITH_MULTITOUCH);

                switch (mapping.inputSource.deviceComponent)
                {
                    case InputDeviceComponent::KEYBOARD_KEY:
                    {
                        if (mapping.inputSource.deviceIndex < GetKeyboardCount())
                            inputBindings.ProcessKeyboardKeyBinding(mapping, *GetKeyboard(mapping.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }

                    case InputDeviceComponent::MOUSE_BUTTON:
                    {
                        if (mapping.inputSource.deviceIndex < GetMouseCount())
                            inputBindings.ProcessMouseButtonBinding(mapping, *GetMouse(mapping.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::MOUSE_RELATIVE_AXIS:
                    {
                        if (mapping.inputSource.deviceIndex < GetMouseCount())
                            inputBindings.ProcessMouseRelativeAxisBinding(mapping, *GetMouse(mapping.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::MOUSE_ABSOLUTE_AXIS:
                    {
                        if (mapping.inputSource.deviceIndex < GetMouseCount())
                            inputBindings.ProcessMouseAbsoluteAxisBinding(mapping, *GetMouse(mapping.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }

                    case InputDeviceComponent::GAME_CONTROLLER_BUTTON:
                    {
                        if (mapping.inputSource.deviceIndex < GetXInputGameControllerCount())
                            inputBindings.ProcessGameControllerButtonBinding(mapping, *GetXInputGameController(mapping.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        else if (mapping.inputSource.deviceIndex < GetXInputGameControllerCount() - GetExternalGameControllerCount())
                            inputBindings.ProcessGameControllerButtonBinding(mapping, *GetExternalGameController(mapping.inputSource.deviceIndex - GetXInputGameControllerCount()), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::GAME_CONTROLLER_AXIS:
                    {
                        if (mapping.inputSource.deviceIndex < GetXInputGameControllerCount())
                            inputBindings.ProcessGameControllerAxisBinding(mapping, *GetXInputGameController(mapping.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        else if (mapping.inputSource.deviceIndex < GetXInputGameControllerCount() - GetExternalGameControllerCount())
                            inputBindings.ProcessGameControllerAxisBinding(mapping, *GetExternalGameController(mapping.inputSource.deviceIndex - GetXInputGameControllerCount()), actions, elapsedTimeSeconds);
                        break;
                    }

                    case InputDeviceComponent::TOUCH_COUNT:
                    {
                        if (mapping.inputSource.deviceIndex < GetTouchScreenCount())
                            inputBindings.ProcessTouchCountBinding(mapping, *GetTouchScreen(mapping.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::TOUCH_RELATIVE_AXIS:
                    {
                        if (mapping.inputSource.deviceIndex < GetTouchScreenCount())
                        {
                            auto touchScreen = GetTouchScreen(mapping.inputSource.deviceIndex);
                            auto pointerCount = touchScreen->GetPointerCount();
                            if (pointerCount > 0 && (mappingAllowsSingleTouchWithMultiTouch || pointerCount < 2))
                                inputBindings.ProcessTouchRelativeAxisBinding(mapping, *touchScreen->GetPointer(0), actions, elapsedTimeSeconds);
                        }
                        break;
                    }
                    case InputDeviceComponent::TOUCH_ABSOLUTE_AXIS:
                    {
                        if (mapping.inputSource.deviceIndex < GetTouchScreenCount())
                        {
                            auto touchScreen = GetTouchScreen(mapping.inputSource.deviceIndex);
                            auto pointerCount = touchScreen->GetPointerCount();
                            if (pointerCount > 0 && (mappingAllowsSingleTouchWithMultiTouch || pointerCount < 2))
                                inputBindings.ProcessTouchAbsoluteAxisBinding(mapping, *touchScreen->GetPointer(0), actions, elapsedTimeSeconds);
                        }
                        break;
                    }

                    case InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS:
                    {
                        if (mapping.inputSource.deviceIndex < GetTouchScreenCount() && GetTouchScreen(mapping.inputSource.deviceIndex)->GetPointerCount() > 1)
                            inputBindings.ProcessMultitouchRelativeRadiusBinding(mapping, *GetTouchScreen(mapping.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                    case InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS:
                    {
                        if (mapping.inputSource.deviceIndex < GetTouchScreenCount() && GetTouchScreen(mapping.inputSource.deviceIndex)->GetPointerCount() > 1)
                            inputBindings.ProcessMultitouchRelativeAxisBinding(mapping, *GetTouchScreen(mapping.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }

                    case InputDeviceComponent::LOCATOR:
                    {
                        if (mapping.inputSource.deviceIndex < GetExternalHeadsetCount())
                            inputBindings.ProcessHeadsetLocatorBinding(mapping, *GetExternalHeadset(mapping.inputSource.deviceIndex), actions, elapsedTimeSeconds);
                        break;
                    }
                }
            }
        }

        template <size_t maxCount>
        void GetChangedInputSources(ChangedInputSources<maxCount>& changed, const ChangedInputSourceFilter& filter)
        {
            if (AnySet(filter.deviceClasses & InputDeviceClass::GAME_CONTROLLER))
            {
                auto gameControllerCount = GetXInputGameControllerCount();
                for (size_t gameControllerIndex = 0; gameControllerIndex < gameControllerCount && !changed.full(); gameControllerIndex++)
                    changed.CheckGameController(GetXInputGameController(gameControllerIndex), gameControllerIndex, filter);

                gameControllerCount = GetExternalGameControllerCount();
                for (size_t gameControllerIndex = 0; gameControllerIndex < gameControllerCount && !changed.full(); gameControllerIndex++)
                    changed.CheckGameController(GetExternalGameController(gameControllerIndex), gameControllerIndex, filter);
            }

            if (AnySet(filter.deviceClasses & InputDeviceClass::KEYBOARD))
            {
                auto keyboardCount = GetKeyboardCount();
                for (size_t keyboardIndex = 0; keyboardIndex < keyboardCount && !changed.full(); keyboardIndex++)
                    changed.CheckKeyboard(GetKeyboard(keyboardIndex), keyboardIndex, filter);
            }

            if (AnySet(filter.deviceClasses & InputDeviceClass::MOUSE))
            {
                auto mouseCount = GetMouseCount();
                for (size_t mouseIndex = 0; mouseIndex < mouseCount && !changed.full(); mouseIndex++)
                    changed.CheckMouse(GetMouse(mouseIndex), mouseIndex, filter);
            }

            if (AnySet(filter.deviceClasses & InputDeviceClass::TOUCH_SCREEN))
            {
                auto touchScreenCount = GetTouchScreenCount();
                for (size_t touchScreenIndex = 0; touchScreenIndex < touchScreenCount && !changed.full(); touchScreenIndex++)
                    changed.CheckTouchScreen(GetTouchScreen(touchScreenIndex), touchScreenIndex, filter);
            }
        }

        struct Impl;
        Impl* GetImpl();

    private:
        std::unique_ptr<Impl> impl;
    };

} }
