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
#include "finjin/common/Error.hpp"
#include "finjin/common/Chrono.hpp"
#include "DInputDeviceComponents.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class Win32InputContext;
    class DInputDeviceConfiguration;

    class DInputMouse
    {    
    public:
        DInputMouse();
        ~DInputMouse();

        void Create(Win32InputContext* context, const DInputDeviceConfiguration& config, Error& error);
        void Destroy();

        void Update(SimpleTimeDelta elapsedTime, bool isFirstUpdate);

        void ClearChanged();

        bool IsConnected() const;

        const Utf8String& GetDisplayName() const;
        void SetDisplayName(const Utf8String& value);

        InputDeviceSemantic GetSemantic() const;
        void SetSemantic(InputDeviceSemantic value);

        const Utf8String& GetProductDescriptor() const;
        const Utf8String& GetInstanceDescriptor() const;

        size_t GetAxisCount() const;
        DInputAxis* GetAxis(size_t axisIndex);

        size_t GetButtonCount() const;
        DInputButton* GetButton(size_t buttonIndex);

        size_t GetPovCount() const;
        DInputPov* GetPov(size_t povIndex);

        size_t GetLocatorCount() const;
        InputLocator* GetLocator(size_t index);

        struct Impl;
        Impl* GetImpl();

    public:
        std::unique_ptr<Impl> impl;        
    };

} }
