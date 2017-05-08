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
#include "finjin/engine/InputContextCommonSettings.hpp"
#include "finjin/engine/internal/input/linuxinput/LinuxGameController.hpp"
#include "finjin/engine/internal/input/linuxinput/LinuxKeyboard.hpp"
#include "finjin/engine/internal/input/linuxinput/LinuxMouse.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class LinuxInputSystem;

    using namespace Finjin::Common;

    class LinuxInputContext : public AllocatedClass
    {
    public:
        LinuxInputContext(Allocator* allocator, LinuxInputSystem* inputSystem);
        ~LinuxInputContext();

        struct Settings : InputContextCommonSettings
        {
            Settings(Allocator* allocator) : InputContextCommonSettings(allocator)
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

        size_t GetGameControllerCount() const;
        LinuxGameController* GetGameController(size_t index);

        size_t GetMouseCount() const;
        LinuxMouse* GetMouse(size_t index);

        size_t GetKeyboardCount() const;
        LinuxKeyboard* GetKeyboard(size_t index);

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
                    if (deviceIndex < GetGameControllerCount())
                        inputBindings.TranslateGameControllerBindings(deviceIndex, GetGameController(deviceIndex));
                    break;
                }
                default: break;
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
                    default: break;
                }
            }
        }

        template <size_t maxCount>
        void GetChangedInputSources(ChangedInputSources<maxCount>& changed, const ChangedInputSourceFilter& filter)
        {
            if (AnySet(filter.deviceClasses & InputDeviceClass::GAME_CONTROLLER))
            {
                auto gameControllerCount = GetGameControllerCount();
                for (size_t i = 0; i < gameControllerCount; i++)
                    changed.CheckGameController(GetGameController(i), i, filter);
            }

            if (AnySet(filter.deviceClasses & InputDeviceClass::KEYBOARD))
            {
                auto keyboardCount = GetKeyboardCount();
                for (size_t i = 0; i < keyboardCount; i++)
                    changed.CheckKeyboard(GetKeyboard(i), i, filter);
            }

            if (AnySet(filter.deviceClasses & InputDeviceClass::MOUSE))
            {
                auto mouseCount = GetMouseCount();
                for (size_t i = 0; i < mouseCount; i++)
                    changed.CheckMouse(GetMouse(i), i, filter);
            }
        }

        struct Impl;
        Impl* GetImpl();

    private:
        std::unique_ptr<Impl> impl;
    };

} }
