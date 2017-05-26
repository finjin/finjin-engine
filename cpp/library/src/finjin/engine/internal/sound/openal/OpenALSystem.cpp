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
#include "OpenALSystem.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "OpenALIncludes.hpp"

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct OpenALSystem::Impl : public AllocatedClass
{
    Impl(Allocator* allocator) : AllocatedClass(allocator), settings(allocator)
    {
    }

    OpenALSystem::Settings settings;

    StaticVector<OpenALContext*, EngineConstants::MAX_WINDOWS> contexts;

    OpenALAdapterDescriptions adapterDescriptions;
    Utf8String defaultDeviceSpecifier;
};


//Implementation----------------------------------------------------------------
const Utf8String& OpenALSystem::GetSystemInternalName()
{
    static const Utf8String value("openal");
    return value;
}

OpenALSystem::OpenALSystem()
{
}

OpenALSystem::~OpenALSystem()
{
}

void OpenALSystem::Create(const Settings& settings, Error& error)
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

    if (alcIsExtensionPresent(0, "ALC_ENUMERATION_EXT"))
    {
        StaticVector<Utf8String, EngineConstants::MAX_SOUND_ADAPTERS> deviceSpecifiers;
        const ALCchar* deviceSpecifier = alcGetString(0, ALC_DEVICE_SPECIFIER);
        ParseNullTerminatedStrings(deviceSpecifier, deviceSpecifiers);
        if (deviceSpecifiers.empty())
        {
            FINJIN_SET_ERROR(error, "No sound adapters detected.");
            return;
        }

        impl->defaultDeviceSpecifier = alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER);

        OpenALAdapterDescription adapterDescription;
        impl->adapterDescriptions.clear();
        for (size_t i = 0; i < deviceSpecifiers.size() && !impl->adapterDescriptions.full(); i++)
        {
            adapterDescription.adapterID = deviceSpecifiers[i];

            auto aldevice = alcOpenDevice(deviceSpecifiers[i].c_str());

            alcGetIntegerv(aldevice, ALC_MAJOR_VERSION, 1, &adapterDescription.versionMajor);
            alcGetIntegerv(aldevice, ALC_MINOR_VERSION, 1, &adapterDescription.versionMinor);

            alcGetIntegerv(aldevice, ALC_EFX_MAJOR_VERSION, 1, &adapterDescription.efxVersionMajor);
            alcGetIntegerv(aldevice, ALC_EFX_MINOR_VERSION, 1, &adapterDescription.efxVersionMinor);

            //std::cout << "EFX " << adapterDescription.efxVersionMajor << "." << adapterDescription.efxVersionMinor << std::endl;

            if (adapterDescription.versionMajor > 1 || (adapterDescription.versionMajor == 1 && adapterDescription.versionMinor > 0))
            {
                auto alcontext = alcCreateContext(aldevice, nullptr);
                alcMakeContextCurrent(alcontext);

                adapterDescription.vendor = alGetString(AL_VENDOR);
                adapterDescription.renderer = alGetString(AL_RENDERER);
                adapterDescription.versionText = alGetString(AL_VERSION);
                adapterDescription.extensionsText = alGetString(AL_EXTENSIONS);

                alcMakeContextCurrent(nullptr);
                alcDestroyContext(alcontext);

                impl->adapterDescriptions.push_back(adapterDescription);
            }

            alcCloseDevice(aldevice);
        }

        if (impl->adapterDescriptions.empty())
        {
            FINJIN_SET_ERROR(error, "No compatible sound adapters detected.");
            return;
        }
    }
}

void OpenALSystem::Destroy()
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

const OpenALAdapterDescriptions& OpenALSystem::GetAdapterDescriptions() const
{
    return impl->adapterDescriptions;
}

OpenALContext* OpenALSystem::CreateContext(const OpenALContext::Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL_RETURN(impl, error, nullptr);

    if (impl->contexts.full())
    {
        FINJIN_SET_ERROR(error, "The maximum number of sound contexts have already been created.");
        return nullptr;
    }

    auto context = AllocatedClass::NewUnique<OpenALContext>(impl->settings.allocator, FINJIN_CALLER_ARGUMENTS, this);
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

void OpenALSystem::DestroyContext(OpenALContext* context)
{
    if (context != nullptr)
    {
        std::unique_ptr<OpenALContext> ptr(context);

        auto it = std::find_if(impl->contexts.begin(), impl->contexts.end(), [&context](OpenALContext* c) { return c == context; });
        if (it != impl->contexts.end())
            impl->contexts.erase(it);

        context->Destroy();
    }
}
