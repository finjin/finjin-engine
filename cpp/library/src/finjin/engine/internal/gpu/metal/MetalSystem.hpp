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
#include "MetalGpuContext.hpp"
#include "MetalGpuDescription.hpp"
#include "MetalSystemSettings.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class MetalSystemImpl;

    class MetalSystem
    {
    public:
        MetalSystem();
        ~MetalSystem();

        using Settings = MetalSystemSettings;

        void Create(const Settings& settings, Error& error);
        void Destroy();

        const MetalHardwareGpuDescriptions& GetHardwareGpuDescriptions() const;
        const MetalSoftwareGpuDescriptions& GetSoftwareGpuDescriptions() const;

        MetalGpuContext* CreateContext(const MetalGpuContext::Settings& settings, Error& error);
        void DestroyContext(MetalGpuContext* context);

        static const Utf8String& GetSystemInternalName();

        MetalSystemImpl* GetImpl();

    private:
        std::unique_ptr<MetalSystemImpl> impl;
    };

} }
