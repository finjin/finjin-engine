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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN

#include "VulkanSystem.hpp"
#include "VulkanSystemImpl.hpp"
#include "VulkanGpuContext.hpp"
#include "VulkanUtilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
const Utf8String& VulkanSystem::GetSystemInternalName()
{
    static const Utf8String value("vulkan");
    return value;
}

VulkanSystem::VulkanSystem()
{
}

VulkanSystem::~VulkanSystem()
{
}

void VulkanSystem::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(settings.allocator != nullptr);
    if (settings.allocator == nullptr)
    {
        FINJIN_SET_ERROR(error, "No allocator was specified.");
        return;
    }

    impl.reset(AllocatedClass::New<VulkanSystemImpl>(settings.allocator, FINJIN_CALLER_ARGUMENTS));
    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->settings = settings;

    //Load Vulkan--------------------------------------------------------------
    auto getInstanceProc = impl->LoadVulkan(error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }
    impl->vk.InitializeGlobal(getInstanceProc, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to load Vulkan library global functions.");
        return;
    }

    //Set up debug state-----------------------------------------
    if (impl->settings.enableDebug)
    {
        impl->layerProperties.Create(impl->vk, impl->GetAllocator(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to enumerate system Vulkan layer properties.");
            return;
        }
    }

    //Create Vulkan instance---------------------------------------------------
    VulkanNamePointers extensions, layers;
    impl->GetInstanceExtensionNames(extensions);
    impl->GetInstanceLayerNames(layers, extensions);

    VulkanApplicationInfo appInfo;
    appInfo.pApplicationName = impl->settings.applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(impl->settings.applicationVersion.GetMajor(), impl->settings.applicationVersion.GetMinor(), impl->settings.applicationVersion.GetPatch());

    VulkanInstanceCreateInfo instanceInfo;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
    instanceInfo.ppEnabledLayerNames = layers.data();
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    VkInstance vulkanInstance;
    auto result = impl->vk.CreateInstance(&instanceInfo, impl->systemAllocationCallbacks, &vulkanInstance);
    if (FINJIN_CHECK_VKRESULT_FAILED(result))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create Vulkan instance. Vulkan result: %1%", VulkanResult::ToString(result)));
        return;
    }

    impl->vk.InitializeInstance(vulkanInstance, impl->vk, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to load Vulkan library instance functions.");
        return;
    }

    //Enumerate physical devices----------------------------------------------------------
    impl->EnumeratePhysicalDevices(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to enumerate physical devices.");
        return;
    }
    if (impl->softwareGpuDescriptions.empty() && impl->hardwareGpuDescriptions.empty())
    {
        FINJIN_SET_ERROR(error, "No physical devices were found.");
        return;
    }
}

void VulkanSystem::Destroy()
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

    impl->vk.ShutdownInstance(impl->systemAllocationCallbacks);
    impl->vk.ShutdownGlobal();

    impl->UnloadVulkan();

    impl.reset();
}

const VulkanHardwareGpuDescriptions& VulkanSystem::GetHardwareGpuDescriptions() const
{
    return impl->hardwareGpuDescriptions;
}

const VulkanSoftwareGpuDescriptions& VulkanSystem::GetSoftwareGpuDescriptions() const
{
    return impl->softwareGpuDescriptions;
}

VulkanGpuContext* VulkanSystem::CreateContext(const VulkanGpuContext::Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (impl->contexts.full())
    {
        FINJIN_SET_ERROR(error, "The maximum number of GPU device contexts have already been created.");
        return nullptr;
    }

    auto context = AllocatedClass::NewUnique<VulkanGpuContext>(impl->settings.allocator, FINJIN_CALLER_ARGUMENTS, this);
    FINJIN_ENGINE_CHECK_CREATED_CONTEXT_NOT_NULL_RETURN(context, error, nullptr);

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

void VulkanSystem::DestroyContext(VulkanGpuContext* context)
{
    if (context != nullptr)
    {
        std::unique_ptr<VulkanGpuContext> ptr(context);

        auto it = std::find_if(impl->contexts.begin(), impl->contexts.end(), [&context](VulkanGpuContext* c) { return c == context; });
        if (it != impl->contexts.end())
            impl->contexts.erase(it);

        context->Destroy();
    }
}

VulkanSystemImpl* VulkanSystem::GetImpl()
{
    return impl.get();
}

#endif
