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
#include "OpenSLESSystem.hpp"
#include "finjin/common/StaticVector.hpp"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct OpenSLESSystem::Impl : public AllocatedClass
{
    Impl(Allocator* allocator) : AllocatedClass(allocator), settings(allocator)
    {
    }

    OpenSLESSystem::Settings settings;

    StaticVector<OpenSLESContext*, EngineConstants::MAX_APPLICATION_VIEWPORTS> contexts;
};


//Implementation----------------------------------------------------------------
const Utf8String& OpenSLESSystem::GetSystemInternalName()
{
    static const Utf8String value("opensles");
    return value;
}

OpenSLESSystem::OpenSLESSystem()
{
}

OpenSLESSystem::~OpenSLESSystem()
{
}

void OpenSLESSystem::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(settings.allocator != nullptr);
    if (settings.allocator == nullptr)
    {
        FINJIN_SET_ERROR(error, "No allocator was specified.");
        return;
    }

    impl.reset(AllocatedClass::New<Impl>(settings.allocator, FINJIN_CALLER_ARGUMENTS));
    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->settings = settings;
}

void OpenSLESSystem::Destroy()
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

OpenSLESContext* OpenSLESSystem::CreateContext(const OpenSLESContext::Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL_RETURN(impl, error, nullptr);

    if (impl->contexts.full())
    {
        FINJIN_SET_ERROR(error, "The maximum number of sound contexts have already been created.");
        return nullptr;
    }

    auto context = AllocatedClass::NewUnique<OpenSLESContext>(impl->settings.allocator, FINJIN_CALLER_ARGUMENTS, this);
    FINJIN_ENGINE_CHECK_CREATED_CONTEXT_NOT_NULL_RETURN(context, error, nullptr);

    context->Create(settings, error);
    if (error)
    {
        DestroyContext(context.release());

        FINJIN_SET_ERROR(error, "Failed to create sound context.");
        return nullptr;
    }

    impl->contexts.push_back(context.get());

    return context.release();
}

void OpenSLESSystem::DestroyContext(OpenSLESContext* context)
{
    if (context != nullptr)
    {
        std::unique_ptr<OpenSLESContext> ptr(context);

        auto it = std::find_if(impl->contexts.begin(), impl->contexts.end(), [&context](OpenSLESContext* c) { return c == context; });
        if (it != impl->contexts.end())
            impl->contexts.erase(it);

        context->Destroy();
    }
}
