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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/InputComponents.hpp"
#include "finjin/engine/VRContextCommonSettings.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct OpenVRDeviceImpl;

    class OpenVRDevice
    {
    public:
        OpenVRDevice();
        ~OpenVRDevice();

        const Utf8String& GetSystemInternalName() const;

        const Utf8String& GetProductDescriptor() const;
        const Utf8String& GetInstanceDescriptor() const;

        InputDeviceSemantic GetSemantic() const;
        void SetSemantic(InputDeviceSemantic value);

        const Utf8String& GetDisplayName() const;
        void SetDisplayName(const Utf8String& value);

        bool IsConnected() const;
        bool ConnectionChanged() const;
        bool IsNewConnection() const;

        size_t GetAxisCount() const;
        InputAxis* GetAxis(size_t index);

        size_t GetButtonCount() const;
        InputButton* GetButton(size_t index);

        size_t GetLocatorCount() const;
        InputLocator* GetLocator(size_t index);

        void AddHapticFeedback(const HapticFeedback* forces, size_t count);
        void StopHapticFeedback();

        InputDeviceClass GetDeviceClass() const;

        VRDeviceRenderModelPointer GetRenderModel();

        OpenVRDeviceImpl* GetImpl();

    private:
        std::unique_ptr<OpenVRDeviceImpl> impl;
    };

} }
