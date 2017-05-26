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
#include "finjin/engine/InputContextCommonSettings.hpp"
#include "../gcinput/GCInputGameController.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/OperationStatus.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class IOSInputSystem;

    using namespace Finjin::Common;

    class IOSInputContext : public AllocatedClass
    {
    public:
        IOSInputContext(Allocator* allocator, IOSInputSystem* inputSystem);
        ~IOSInputContext();

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

        size_t GetGameControllerCount() const;
        GCInputGameController* GetGameController(size_t index);

        size_t GetTouchScreenCount() const;
        InputTouchScreen* GetTouchScreen(size_t index);

        FileOperationResult ReadInputBinding(InputDeviceClass deviceClass, size_t deviceIndex, const AssetReference& configAssetRef, ByteBuffer& fileBuffer);

        template <typename InputBindings>
        void TranslateInputBindingsSemantics(InputBindings& inputBindings, InputDeviceClass deviceClass, size_t deviceIndex)
        {
            switch (deviceClass)
            {
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
                for (size_t gameControllerIndex = 0; gameControllerIndex < gameControllerCount; gameControllerIndex++)
                    changed.CheckGameController(GetGameController(gameControllerIndex), gameControllerIndex, filter);
            }

            if (AnySet(filter.deviceClasses & InputDeviceClass::TOUCH_SCREEN))
            {
                auto touchScreenCount = GetTouchScreenCount();
                for (size_t touchScreenIndex = 0; touchScreenIndex < touchScreenCount; touchScreenIndex++)
                    changed.CheckTouchScreen(GetTouchScreen(touchScreenIndex), touchScreenIndex, filter);
            }
        }

        struct Impl;
        Impl* GetImpl();

    private:
        std::unique_ptr<Impl> impl;
    };

} }
