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
    Impl(Allocator* allocator) : AllocatedClass(allocator), settings(allocator), defaultDeviceSpecifier(allocator)
    {
        this->adapterDescriptions.maximize();
        for (auto& adapterDescription : this->adapterDescriptions)
        {
            adapterDescription.vendor.SetAllocator(allocator);
            adapterDescription.renderer.SetAllocator(allocator);
            adapterDescription.versionText.SetAllocator(allocator);
            adapterDescription.extensionsText.SetAllocator(allocator);
        }
        this->adapterDescriptions.clear();
    }

    OpenALSystem::Settings settings;

    StaticVector<OpenALContext*, EngineConstants::MAX_APPLICATION_VIEWPORTS> contexts;

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
        auto parseResult = ParseNullTerminatedStrings(deviceSpecifier, [&deviceSpecifiers](const char* value)
        {
            if (!deviceSpecifiers.push_back())
                return ValueOrError<bool>::CreateError();

            if (deviceSpecifiers.back().assign(value).HasError())
                return ValueOrError<bool>::CreateError();

            return ValueOrError<bool>(true);
        });
        if (parseResult.HasError())
        {
            FINJIN_SET_ERROR(error, "There was an error parsing sound adapter device specifiers.");
            return;
        }
        if (deviceSpecifiers.empty())
        {
            FINJIN_SET_ERROR(error, "No sound adapters detected.");
            return;
        }

        if (impl->defaultDeviceSpecifier.assign(alcGetString(0, ALC_DEFAULT_DEVICE_SPECIFIER)).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign default device specifier.");
            return;
        }

        for (size_t i = 0; i < deviceSpecifiers.size() && !impl->adapterDescriptions.full(); i++)
        {
            impl->adapterDescriptions.push_back();
            auto& adapterDescription = impl->adapterDescriptions.back();

            if (adapterDescription.adapterID.assign(deviceSpecifiers[i]).HasError())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign device specifier '%1%'.", deviceSpecifiers[i]));
                return;
            }

            auto aldevice = alcOpenDevice(deviceSpecifiers[i].c_str());
            if (aldevice == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to open device '%1%'.", deviceSpecifiers[i]));
                return;
            }

            alcGetIntegerv(aldevice, ALC_MAJOR_VERSION, 1, &adapterDescription.versionMajor);
            alcGetIntegerv(aldevice, ALC_MINOR_VERSION, 1, &adapterDescription.versionMinor);

            alcGetIntegerv(aldevice, ALC_EFX_MAJOR_VERSION, 1, &adapterDescription.efxVersionMajor);
            alcGetIntegerv(aldevice, ALC_EFX_MINOR_VERSION, 1, &adapterDescription.efxVersionMinor);

            //std::cout << "EFX " << adapterDescription.efxVersionMajor << "." << adapterDescription.efxVersionMinor << std::endl;

            if (adapterDescription.versionMajor > 1 || (adapterDescription.versionMajor == 1 && adapterDescription.versionMinor > 0))
            {
                auto alcontext = alcCreateContext(aldevice, nullptr);
                alcMakeContextCurrent(alcontext);

                if (adapterDescription.vendor.assign(alGetString(AL_VENDOR)).HasError())
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign vendor name for device '%1%'.", deviceSpecifiers[i]));
                    return;
                }

                if (adapterDescription.renderer.assign(alGetString(AL_RENDERER)).HasError())
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign renderer name for device '%1%'.", deviceSpecifiers[i]));
                    return;
                }

                if (adapterDescription.versionText.assign(alGetString(AL_VERSION)).HasError())
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign version for device '%1%'.", deviceSpecifiers[i]));
                    return;
                }

                if (adapterDescription.extensionsText.assign(alGetString(AL_EXTENSIONS)).HasError())
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign extensions name for device '%1%'.", deviceSpecifiers[i]));
                    return;
                }

                alcMakeContextCurrent(nullptr);
                alcDestroyContext(alcontext);
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
