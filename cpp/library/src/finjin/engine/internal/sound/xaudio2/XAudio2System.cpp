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
#include "XAudio2System.hpp"
#include "finjin/common/StaticVector.hpp"
#include "XAudio2Includes.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct XAudio2System::Impl : public AllocatedClass
{
    Impl(Allocator* allocator) : AllocatedClass(allocator)
    {
    }

    XAudio2System::Settings settings;

    StaticVector<XAudio2Context*, EngineConstants::MAX_WINDOWS> contexts; //The actual objects
};


//Implementation----------------------------------------------------------------
const Utf8String& XAudio2System::GetSystemInternalName()
{
    static const Utf8String value("xaudio2");
    return value;
}

XAudio2System::XAudio2System()
{
}

XAudio2System::~XAudio2System()
{
}

void XAudio2System::Create(const Settings& settings, Error& error)
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

    CoInitializeEx(NULL, COINIT_MULTITHREADED);
}

void XAudio2System::Destroy()
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

    CoUninitialize();
}

XAudio2Context* XAudio2System::CreateContext(const XAudio2Context::Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL_RETURN(impl, error, nullptr);

    if (impl->contexts.full())
    {
        FINJIN_SET_ERROR(error, "The maximum number of sound contexts have already been created.");
        return nullptr;
    }

    auto context = AllocatedClass::NewUnique<XAudio2Context>(impl->settings.allocator, FINJIN_CALLER_ARGUMENTS, this);
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

void XAudio2System::DestroyContext(XAudio2Context* context)
{
    if (context != nullptr)
    {
        std::unique_ptr<XAudio2Context> ptr(context);

        auto it = std::find_if(impl->contexts.begin(), impl->contexts.end(), [&context](XAudio2Context* c) { return c == context; });
        if (it != impl->contexts.end())
            impl->contexts.erase(it);

        context->Destroy();
    }
}
