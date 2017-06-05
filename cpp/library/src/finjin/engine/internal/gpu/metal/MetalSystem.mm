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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_METAL

#include "MetalSystem.hpp"
#include "MetalSystemImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
const Utf8String& MetalSystem::GetSystemInternalName()
{
    static const Utf8String value("metal");
    return value;
}

MetalSystem::MetalSystem()
{
}

MetalSystem::~MetalSystem()
{
}

void MetalSystem::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(settings.allocator != nullptr);
    if (settings.allocator == nullptr)
    {
        FINJIN_SET_ERROR(error, "No allocator was specified.");
        return;
    }

    impl.reset(AllocatedClass::New<MetalSystemImpl>(settings.allocator, FINJIN_CALLER_ARGUMENTS));
    assert(impl != nullptr);
    if (impl == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed to allocate initialize internal state.");
        return;
    }

    impl->settings = settings;

    //Get devices-----------------------

    //Default goes at the front
    impl->devices.push_back(MTLCreateSystemDefaultDevice());

#if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
    //Get the other devices
    auto allDevices = MTLCopyAllDevices();
    for (NSUInteger deviceIndex = 0; deviceIndex < allDevices.count; deviceIndex++)
    {
        if (allDevices[deviceIndex] != impl->devices[0])
        {
            if (impl->devices.push_back(allDevices[deviceIndex]).HasErrorOrValue(false))
                break;
        }
    }
#endif

    //Create device descriptions-----------------------
    for (auto device : impl->devices)
    {
        if (!impl->deviceDescriptions.push_back())
            break;

        auto& deviceDescription = impl->deviceDescriptions.back();

        if (deviceDescription.gpuID.assign(device.name.UTF8String).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign GPU ID.");
            return;
        }

    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_TVOS
        if ([device supportsFeatureSet:MTLFeatureSet_tvOS_GPUFamily1_v2])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_tvOS_GPUFamily1_v2);
        else if ([device supportsFeatureSet:MTLFeatureSet_tvOS_GPUFamily1_v1])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_tvOS_GPUFamily1_v1);
    #elif FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS
        if ([device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v2])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_iOS_GPUFamily3_v2);
        else if ([device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v3])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_iOS_GPUFamily2_v3);
        else if ([device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v3])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_iOS_GPUFamily1_v3);

        else if ([device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily3_v1])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_iOS_GPUFamily3_v1);
        else if ([device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v2])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_iOS_GPUFamily2_v2);
        else if ([device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v2])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_iOS_GPUFamily1_v2);

        else if ([device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v1])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_iOS_GPUFamily2_v1);
        else if ([device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily1_v1])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_iOS_GPUFamily1_v1);
    #elif FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
        if ([device supportsFeatureSet:MTLFeatureSet_OSX_ReadWriteTextureTier2])
        {
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_OSX_GPUFamily1_v2);
            deviceDescription.supportsReadWriteTexturesTier2 = true;
        }
        else if ([device supportsFeatureSet:MTLFeatureSet_OSX_GPUFamily1_v2])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_OSX_GPUFamily1_v2);
        else if ([device supportsFeatureSet:MTLFeatureSet_OSX_GPUFamily1_v1])
            deviceDescription.SetStandardFeatureSet(MTLFeatureSet_OSX_GPUFamily1_v1);

        deviceDescription.isLowPower = device.lowPower ? true : false;
        deviceDescription.isHeadless = device.headless ? true : false;
    #endif

        auto maxThreadsPerThreadgroup = device.maxThreadsPerThreadgroup;
        deviceDescription.maxThreadsPerThreadgroup[0] = maxThreadsPerThreadgroup.width;
        deviceDescription.maxThreadsPerThreadgroup[1] = maxThreadsPerThreadgroup.height;
        deviceDescription.maxThreadsPerThreadgroup[2] = maxThreadsPerThreadgroup.depth;

    #if FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS
        deviceDescription.recommendedMaxWorkingSetSize = device.recommendedMaxWorkingSetSize;
    #endif

        for (size_t sampleCount = 1; sampleCount <= deviceDescription.supportedTextureSampleCounts.max_size(); sampleCount++)
        {
            if ([device supportsTextureSampleCount:static_cast<NSUInteger>(sampleCount)])
                deviceDescription.supportedTextureSampleCounts.push_back(sampleCount);
        }
    }
    impl->deviceDescriptions.SortBestToWorst();
}

void MetalSystem::Destroy()
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

    impl->devices.clear();

    impl.reset();
}

const MetalHardwareGpuDescriptions& MetalSystem::GetHardwareGpuDescriptions() const
{
    return impl->deviceDescriptions;
}

const MetalSoftwareGpuDescriptions& MetalSystem::GetSoftwareGpuDescriptions() const
{
    return impl->softwareDeviceDescriptions;
}

MetalGpuContext* MetalSystem::CreateContext(const MetalGpuContext::Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL_RETURN(impl, error, nullptr)

    if (impl->contexts.full())
    {
        FINJIN_SET_ERROR(error, "The maximum number of GPU device contexts have already been created.");
        return nullptr;
    }

    auto context = AllocatedClass::NewUnique<MetalGpuContext>(impl->settings.allocator, FINJIN_CALLER_ARGUMENTS, this);

    context->Create(settings, error);
    if (error)
    {
        DestroyContext(context.release());

        FINJIN_SET_ERROR(error, "Failed to create GPU device context.");
        return nullptr;
    }

    impl->contexts.push_back(context.get());

    return context.release();
}

void MetalSystem::DestroyContext(MetalGpuContext* context)
{
    if (context != nullptr)
    {
        std::unique_ptr<MetalGpuContext> ptr(context);

        auto it = impl->contexts.find(context);
        if (it != impl->contexts.end())
            impl->contexts.erase(it);

        context->Destroy();
    }
}

MetalSystemImpl* MetalSystem::GetImpl()
{
    return impl.get();
}

#endif
