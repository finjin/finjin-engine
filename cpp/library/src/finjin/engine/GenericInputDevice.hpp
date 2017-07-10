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
#include "finjin/engine/InputComponents.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    /**
     * Basic values for an input device. Its use is not required, depending on the needs of the input system/device.
     */
    struct GenericInputDeviceImpl
    {
        GenericInputDeviceImpl(Allocator* allocator = nullptr) :
            semantic(InputDeviceSemantic::NONE),
            displayName(allocator),
            instanceName(allocator),
            productName(allocator),
            instanceDescriptor(allocator),
            productDescriptor(allocator)
        {
        }

        ~GenericInputDeviceImpl() {} //Intentionally not virtual

        void SetAllocator(Allocator* allocator)
        {
            this->displayName.SetAllocator(allocator);

            this->instanceName.SetAllocator(allocator);
            this->productName.SetAllocator(allocator);

            this->instanceDescriptor.SetAllocator(allocator);
            this->productDescriptor.SetAllocator(allocator);
        }

        void Reset()
        {
            this->semantic = InputDeviceSemantic::NONE;

            this->displayName.clear();

            this->instanceName.clear();
            this->productName.clear();

            this->instanceDescriptor.clear();
            this->productDescriptor.clear();
        }

        InputDeviceSemantic GetSemantic() const { return this->semantic; }
        void SetSemantic(InputDeviceSemantic value) { this->semantic = value; }

        const Utf8String& GetDisplayName() const { return this->displayName; }
        void SetDisplayName(const Utf8String& value) { this->displayName = value; }

        const Utf8String& GetInstanceName() const { return this->instanceName; }
        const Utf8String& GetProductName() const { return this->productName; }

        const Utf8String& GetInstanceDescriptor() const { return this->instanceDescriptor; }
        const Utf8String& GetProductDescriptor() const { return this->productDescriptor; }

        InputDeviceSemantic semantic;

        Utf8String displayName;

        Utf8String instanceName;
        Utf8String productName;

        Utf8String instanceDescriptor;
        Utf8String productDescriptor;
    };

    class ExternalGameController
    {
    public:
        virtual ~ExternalGameController() {}

        virtual const Utf8String& GetSystemInternalName() const = 0;

        virtual const Utf8String& GetProductDescriptor() const = 0;
        virtual const Utf8String& GetInstanceDescriptor() const = 0;

        virtual InputDeviceSemantic GetSemantic() const = 0;
        virtual void SetSemantic(InputDeviceSemantic value) = 0;

        virtual const Utf8String& GetDisplayName() const = 0;
        virtual void SetDisplayName(const Utf8String& value) = 0;

        virtual bool IsConnected() const = 0;
        virtual bool ConnectionChanged() const = 0;
        virtual bool IsNewConnection() const = 0;

        virtual size_t GetAxisCount() const = 0;
        virtual InputAxis* GetAxis(size_t index) = 0;

        virtual size_t GetPovCount() const = 0;
        virtual InputPov* GetPov(size_t index) = 0;

        virtual size_t GetButtonCount() const = 0;
        virtual InputButton* GetButton(size_t index) = 0;

        virtual size_t GetLocatorCount() const = 0;
        virtual InputLocator* GetLocator(size_t index) = 0;

        virtual void AddHapticFeedback(const HapticFeedback* forces, size_t count) = 0;
        virtual void StopHapticFeedback() = 0;
    };

    class ExternalHeadset
    {
    public:
        virtual ~ExternalHeadset() {}

        virtual const Utf8String& GetSystemInternalName() const = 0;

        virtual const Utf8String& GetProductDescriptor() const = 0;
        virtual const Utf8String& GetInstanceDescriptor() const = 0;

        virtual InputDeviceSemantic GetSemantic() const = 0;
        virtual void SetSemantic(InputDeviceSemantic value) = 0;

        virtual const Utf8String& GetDisplayName() const = 0;
        virtual void SetDisplayName(const Utf8String& value) = 0;

        virtual bool IsConnected() const = 0;
        virtual bool ConnectionChanged() const = 0;
        virtual bool IsNewConnection() const = 0;

        virtual size_t GetLocatorCount() const = 0;
        virtual InputLocator* GetLocator(size_t index) = 0;
    };

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    template <typename VRContext>
    class VRGameControllerForInputContext : public ExternalGameController
    {
    public:
        VRGameControllerForInputContext() { this->vrContext = nullptr; this->vrDeviceIndex = (size_t)-1; }
        VRGameControllerForInputContext(VRContext* vrContext, size_t index) { this->vrContext = vrContext; this->vrDeviceIndex = index; }

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

        void AddHapticFeedback(const HapticFeedback* forces, size_t count) override { this->vrContext->AddHapticFeedback(this->vrDeviceIndex, forces, count); }
        void StopHapticFeedback() override { this->vrContext->StopHapticFeedback(this->vrDeviceIndex); }

        bool operator == (const VRGameControllerForInputContext& other) const { return this->vrDeviceIndex == other.vrDeviceIndex; }

    public:
        VRContext* vrContext;
        size_t vrDeviceIndex; //The index of the device in vrContext
    };

    template <typename VRContext>
    class VRHeadsetForInputContext : public ExternalHeadset
    {
    public:
        VRHeadsetForInputContext() { this->vrContext = nullptr; this->vrDeviceIndex = (size_t)-1; }
        VRHeadsetForInputContext(VRContext* vrContext, size_t index) { this->vrContext = vrContext; this->vrDeviceIndex = index; }

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

        bool operator == (const VRHeadsetForInputContext& other) const { return this->vrDeviceIndex == other.vrDeviceIndex; }

    public:
        VRContext* vrContext;
        size_t vrDeviceIndex; //The index of the device in vrContext
    };
#endif

} }
