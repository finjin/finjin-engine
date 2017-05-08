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
#include "AndroidInputSystem.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/NvAndroidNativeAppGlue.h"
#include "finjin/common/StaticVector.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct AndroidInputSystem::Impl : public AllocatedClass
{
    Impl(Allocator* allocator) : AllocatedClass(allocator)
    {
    }

    AndroidInputSystem::Settings settings;

    StaticVector<AndroidInputContext*, EngineConstants::MAX_WINDOWS> contexts; //The actual objects
};


//Implementation----------------------------------------------------------------
const Utf8String& AndroidInputSystem::GetSystemInternalName()
{
    static const Utf8String value("androidinput");
    return value;
}

AndroidInputSystem::AndroidInputSystem()
{
}

AndroidInputSystem::~AndroidInputSystem()
{
}

void AndroidInputSystem::Create(const Settings& settings, Error& error)
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

void AndroidInputSystem::Destroy()
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

AndroidInputContext* AndroidInputSystem::CreateContext(const AndroidInputContext::Settings& _settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL_RETURN(impl, error, nullptr);

    if (impl->contexts.full())
    {
        FINJIN_SET_ERROR(error, "The maximum number of input contexts have already been created.");
        return nullptr;
    }

    auto context = AllocatedClass::NewUnique<AndroidInputContext>(impl->settings.allocator, FINJIN_CALLER_ARGUMENTS, this);
    FINJIN_ENGINE_CHECK_CREATED_CONTEXT_NOT_NULL_RETURN(context, error, nullptr);

    AndroidInputContext::Settings settings = _settings;
    settings.useSystemBackButton = impl->settings.useSystemBackButton;
    settings.useAccelerometer = impl->settings.useAccelerometer;
    settings.accelerometerAlooperID = impl->settings.accelerometerAlooperID;
    context->Create(settings, error);
    if (error)
    {
        DestroyContext(context.release());

        FINJIN_SET_ERROR(error, "Failed to create input context.");
        return nullptr;
    }

    impl->contexts.push_back(context.get());

    return context.release();
}

void AndroidInputSystem::DestroyContext(AndroidInputContext* context)
{
    if (context != nullptr)
    {
        std::unique_ptr<AndroidInputContext> ptr(context);

        auto it = std::find_if(impl->contexts.begin(), impl->contexts.end(), [&context](AndroidInputContext* c) { return c == context; });
        if (it != impl->contexts.end())
            impl->contexts.erase(it);

        context->Destroy();
    }
}

int32_t AndroidInputSystem::HandleApplicationInputEvent(void* androidInputEvent)
{
    if (!impl->contexts.empty())
        return impl->contexts[0]->HandleApplicationInputEvent(androidInputEvent);
    else
        return 0;
}
