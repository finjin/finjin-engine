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
#include "finjin/common/Error.hpp"
#include "D3D12GpuContext.hpp"
#include "D3D12GpuDescription.hpp"
#include "D3D12SystemSettings.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12SystemImpl;

    class D3D12System
    {
    public:
        D3D12System();
        ~D3D12System();

        using Settings = D3D12SystemSettings;

        void Create(const Settings& settings, Error& error);
        void Destroy();

        const D3D12HardwareGpuDescriptions& GetHardwareGpuDescriptions() const;
        const D3D12SoftwareGpuDescriptions& GetSoftwareGpuDescriptions() const;

        D3D12GpuContext* CreateContext(const D3D12GpuContext::Settings& settings, Error& error);
        void DestroyContext(D3D12GpuContext* context);

        D3D12SystemImpl* GetImpl();

        static const Utf8String& GetSystemInternalName();

    private:
        std::unique_ptr<D3D12SystemImpl> impl;
    };

} }
