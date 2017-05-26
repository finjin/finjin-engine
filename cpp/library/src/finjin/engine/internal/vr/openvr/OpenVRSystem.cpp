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


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"

#if FINJIN_TARGET_VR_SYSTEM == FINJIN_TARGET_VR_SYSTEM_OPENVR

#include "OpenVRSystem.hpp"
#include "OpenVRSystemSettings.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct OpenVRSystem::Impl
{
public:
    Impl()
    {
    }

public:
    OpenVRSystemSettings settings;

    StaticVector<OpenVRContext*, 1> contexts;
};


//Implementation----------------------------------------------------------------
const Utf8String& OpenVRSystem::GetSystemInternalName()
{
    static const Utf8String value("openvr");
    return value;
}

OpenVRSystem::OpenVRSystem() : impl(new Impl)
{
}

OpenVRSystem::~OpenVRSystem()
{
}

void OpenVRSystem::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    impl->settings = settings;
}

void OpenVRSystem::Destroy()
{
    if (impl == nullptr)
        return;

    assert(impl->contexts.empty()); //There shouldn't be any contexts at this point
    for (auto context : impl->contexts)
    {
        context->Destroy();
        delete context;
    }
    impl->contexts.clear();

    impl.reset();
}

OpenVRContext* OpenVRSystem::CreateContext(const OpenVRContext::Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl->contexts.full())
    {
        FINJIN_SET_ERROR(error, "The maximum number of VR device contexts have already been created.");
        return nullptr;
    }

    auto context = AllocatedClass::NewUnique<OpenVRContext>(impl->settings.allocator, FINJIN_CALLER_ARGUMENTS, this);
    FINJIN_ENGINE_CHECK_CREATED_CONTEXT_NOT_NULL_RETURN(context, error, nullptr);

    context->Create(settings, error);
    if (error)
    {
        DestroyContext(context.release());

        FINJIN_SET_ERROR(error, "Failed to create VR device context.");
        return nullptr;
    }

    impl->contexts.push_back(context.get());

    return context.release();
}

void OpenVRSystem::DestroyContext(OpenVRContext* context)
{
    if (context != nullptr)
    {
        std::unique_ptr<OpenVRContext> ptr(context);

        auto it = std::find_if(impl->contexts.begin(), impl->contexts.end(), [&context](OpenVRContext* c) { return c == context; });
        if (it != impl->contexts.end())
            impl->contexts.erase(it);

        context->Destroy();
    }
}

#endif
