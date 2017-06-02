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
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    #include "finjin/engine/InputComponents.hpp"
    #include "finjin/engine/VRSystem.hpp"
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;
    
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    class VRGameControllerForInputSystem : public InputGenericGameController
    {
    public:
        VRGameControllerForInputSystem() { this->vrContext = nullptr; this->vrDeviceIndex = (size_t)-1; }
        VRGameControllerForInputSystem(VRContext* vrContext, size_t index) { this->vrContext = vrContext; this->vrDeviceIndex = index; }

        const Utf8String& GetSystemInternalName() const override { return this->vrContext->GetDeviceSystemInternalName(this->vrDeviceIndex); }

        const Utf8String& GetProductDescriptor() const override { return this->vrContext->GetDeviceProductDescriptor(this->vrDeviceIndex); }
        const Utf8String& GetInstanceDescriptor() const override { return this->vrContext->GetDeviceInstanceDescriptor(this->vrDeviceIndex); }

        InputDeviceSemantic GetSemantic() const override { return this->vrContext->GetDeviceSemantic(this->vrDeviceIndex); }
        void SetSemantic(InputDeviceSemantic value) override { this->vrContext->SetDeviceSemantic(this->vrDeviceIndex, value); }

        const Utf8String& GetDisplayName() const override { return this->vrContext->GetDeviceDisplayName(this->vrDeviceIndex); }
        void SetDisplayName(const Utf8String& value) override { this->vrContext->SetDeviceDisplayName(this->vrDeviceIndex, value); }

        bool IsConnected() const override { return this->vrContext->IsDeviceConnected(this->vrDeviceIndex); }
        bool ConnectionChanged() const override { return this->vrContext->DeviceConnectionChanged(this->vrDeviceIndex); }
        bool IsNewConnection() const override { return this->vrContext->IsNewDeviceConnection(this->vrDeviceIndex); }

        size_t GetAxisCount() const override { return this->vrContext->GetDeviceAxisCount(this->vrDeviceIndex); }
        InputAxis* GetAxis(size_t index) override { return this->vrContext->GetDeviceAxis(this->vrDeviceIndex, index); }

        size_t GetPovCount() const override { return 0; }
        InputPov* GetPov(size_t index) override { return nullptr; }

        size_t GetButtonCount() const override { return this->vrContext->GetDeviceButtonCount(this->vrDeviceIndex); }
        InputButton* GetButton(size_t index) override { return this->vrContext->GetDeviceButton(this->vrDeviceIndex, index); }

        size_t GetLocatorCount() const override { return this->vrContext->GetLocatorCount(this->vrDeviceIndex); }
        InputLocator* GetLocator(size_t index) override { return this->vrContext->GetLocator(this->vrDeviceIndex, index); }

        void AddHapticFeedback(const HapticFeedbackSettings* forces, size_t count) override { this->vrContext->AddHapticFeedback(this->vrDeviceIndex, forces, count); }
        void StopHapticFeedback() override { this->vrContext->StopHapticFeedback(this->vrDeviceIndex); }

        bool operator == (const VRGameControllerForInputSystem& other) const { return this->vrDeviceIndex == other.vrDeviceIndex; }

    public:
        VRContext* vrContext;
        size_t vrDeviceIndex; //The index of the device in vrContext
    };

    class VRHeadsetForInputSystem : public InputGenericHeadset
    {
    public:
        VRHeadsetForInputSystem() { this->vrContext = nullptr; this->vrDeviceIndex = (size_t)-1; }
        VRHeadsetForInputSystem(VRContext* vrContext, size_t index) { this->vrContext = vrContext; this->vrDeviceIndex = index; }

        const Utf8String& GetSystemInternalName() const override { return this->vrContext->GetDeviceSystemInternalName(this->vrDeviceIndex); }

        const Utf8String& GetProductDescriptor() const override { return this->vrContext->GetDeviceProductDescriptor(this->vrDeviceIndex); }
        const Utf8String& GetInstanceDescriptor() const override { return this->vrContext->GetDeviceInstanceDescriptor(this->vrDeviceIndex); }

        InputDeviceSemantic GetSemantic() const override { return this->vrContext->GetDeviceSemantic(this->vrDeviceIndex); }
        void SetSemantic(InputDeviceSemantic value) override { this->vrContext->SetDeviceSemantic(this->vrDeviceIndex, value); }

        const Utf8String& GetDisplayName() const override { return this->vrContext->GetDeviceDisplayName(this->vrDeviceIndex); }
        void SetDisplayName(const Utf8String& value) override { this->vrContext->SetDeviceDisplayName(this->vrDeviceIndex, value); }

        bool IsConnected() const override { return this->vrContext->IsDeviceConnected(this->vrDeviceIndex); }
        bool ConnectionChanged() const override { return this->vrContext->DeviceConnectionChanged(this->vrDeviceIndex); }
        bool IsNewConnection() const override { return this->vrContext->IsNewDeviceConnection(this->vrDeviceIndex); }

        size_t GetLocatorCount() const override { return this->vrContext->GetLocatorCount(this->vrDeviceIndex); }
        InputLocator* GetLocator(size_t index) override { return this->vrContext->GetLocator(this->vrDeviceIndex, index); }

        bool operator == (const VRHeadsetForInputSystem& other) const { return this->vrDeviceIndex == other.vrDeviceIndex; }

    public:
        VRContext* vrContext;
        size_t vrDeviceIndex; //The index of the device in vrContext
    };
#endif

} }
