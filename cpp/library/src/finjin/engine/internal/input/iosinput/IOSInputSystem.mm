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
#include "IOSInputSystem.hpp"
#include "finjin/common/StaticVector.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct IOSInputSystem::Impl : public AllocatedClass
{
    Impl(Allocator* allocator) : AllocatedClass(allocator)
    {
    }

    IOSInputSystem::Settings settings;

    StaticVector<IOSInputContext*, EngineConstants::MAX_WINDOWS> contexts; //The actual objects
};


//Implementation----------------------------------------------------------------
const Utf8String& IOSInputSystem::GetSystemInternalName()
{
    static const Utf8String value("iosinput");
    return value;
}

IOSInputSystem::IOSInputSystem()
{
}

IOSInputSystem::~IOSInputSystem()
{
}

void IOSInputSystem::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(settings.allocator != nullptr);
    if (settings.allocator == nullptr)
    {
        FINJIN_SET_ERROR(error, "No allocator was specified.");
        return;
    }

    impl.reset(AllocatedClass::New<Impl>(settings.allocator, FINJIN_CALLER_ARGUMENTS));
    assert(impl != nullptr);
    if (impl == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to allocate initialize internal state.");
        return;
    }

    impl->settings = settings;
}

void IOSInputSystem::Destroy()
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

IOSInputContext* IOSInputSystem::CreateContext(const IOSInputContext::Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl == nullptr)
    {
        FINJIN_SET_ERROR(error, "IOSInputSystem is not in an initialized state.");
        return nullptr;
    }

    if (impl->contexts.full())
    {
        FINJIN_SET_ERROR(error, "The maximum number of input contexts have already been created.");
        return nullptr;
    }

    auto context = AllocatedClass::NewUnique<IOSInputContext>(impl->settings.allocator, FINJIN_CALLER_ARGUMENTS, this);

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

void IOSInputSystem::DestroyContext(IOSInputContext* context)
{
    if (context != nullptr)
    {
        std::unique_ptr<IOSInputContext> ptr(context);

        auto it = impl->contexts.find(context);
        if (it != impl->contexts.end())
            impl->contexts.erase(it);

        context->Destroy();
    }
}
