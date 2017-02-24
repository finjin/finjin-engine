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


//Includes---------------------------------------------------------------
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "OpenVRContextSettings.hpp"
#include "OpenVRDevice.hpp"


//Classes----------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    using namespace Finjin::Common;
    
    class OpenVRSystem;
                
    class OpenVRContext : public AllocatedClass
    {
    public:
        OpenVRContext(Allocator* allocator, OpenVRSystem* vrSystem);
        ~OpenVRContext();

        using Settings = OpenVRContextSettings;
                
        void Create(const Settings& settings, Error& error);
        void Destroy();

        const OperationStatus& GetInitializationStatus() const;
        
        void GetSelectorComponents(AssetPathSelector& result);

        const Settings& GetSettings() const;

        void UpdateInputDevices(SimpleTimeDelta elapsedTime);
        void UpdateCompositor();

        void Execute(VREvents& events, VRCommands& commands, Error& error);

        size_t GetDeviceCount() const;
        size_t GetDeviceCount(InputDeviceClass deviceClass) const;

        InputDeviceClass GetDeviceClass(size_t index) const;

        const Utf8String& GetDeviceSystemInternalName(size_t index) const;

        const Utf8String& GetDeviceProductDescriptor(size_t index) const;
        const Utf8String& GetDeviceInstanceDescriptor(size_t index) const;

        InputDeviceSemantic GetDeviceSemantic(size_t index) const;
        void SetDeviceSemantic(size_t index, InputDeviceSemantic value);

        const Utf8String& GetDeviceDisplayName(size_t index) const;
        void SetDeviceDisplayName(size_t index, const Utf8String& value);

        bool IsDeviceConnected(size_t index) const;
        bool DeviceConnectionChanged(size_t index) const;
        bool IsNewDeviceConnection(size_t index) const;

        size_t GetDeviceAxisCount(size_t index) const;
        InputAxis* GetDeviceAxis(size_t index, size_t axisIndex);

        size_t GetDeviceButtonCount(size_t index) const;
        InputButton* GetDeviceButton(size_t index, size_t buttonIndex);

        size_t GetLocatorCount(size_t index) const;
        InputLocator* GetLocator(size_t index, size_t locatorIndex);

        void AddHapticFeedback(size_t index, const HapticFeedbackSettings* forces, size_t count);
        void StopHapticFeedback(size_t index);
        
    private:
        struct Impl;
        std::unique_ptr<Impl> impl;
    };
    
} }
