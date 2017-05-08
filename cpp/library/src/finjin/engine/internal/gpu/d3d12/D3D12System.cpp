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

#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12

#include "D3D12System.hpp"
#include "D3D12SystemImpl.hpp"
#include "D3D12Utilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
const Utf8String& D3D12System::GetSystemInternalName()
{
    static const Utf8String value("d3d12");
    return value;
}

D3D12System::D3D12System()
{
}

D3D12System::~D3D12System()
{
}

void D3D12System::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(settings.allocator != nullptr);
    if (settings.allocator == nullptr)
    {
        FINJIN_SET_ERROR(error, "No allocator was specified.");
        return;
    }

    impl.reset(AllocatedClass::New<D3D12SystemImpl>(settings.allocator, FINJIN_CALLER_ARGUMENTS));
    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->settings = settings;

    //Set up debug state-----------------------------------------
    if (impl->settings.enableDebug)
    {
        //Enable the D3D12 debug layer if possible
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
            debugController->EnableDebugLayer();
    }

    //Create factory------------------------------------------------------------------
    if (FINJIN_CHECK_HRESULT_FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&impl->dxgiFactory))))
    {
        FINJIN_SET_ERROR(error, "Failed to create DXGI factory.");
        return;
    }

    //Enumerate adapters----------------------------------------------------------
    impl->EnumerateHardwareGpus();
    auto disallowSoftwareGpu = impl->settings.softwareGpuRequirement == D3D12SystemSettings::SoftwareGpuRequirement::DISALLOW;
    if (disallowSoftwareGpu && impl->hardwareGpuDescriptions.empty())
    {
        FINJIN_SET_ERROR(error, "Failed to enumerate hardware adapters.");
        return;
    }

    impl->EnumerateSoftwareGpus();
    auto requireSoftwareGpu = impl->settings.softwareGpuRequirement == D3D12SystemSettings::SoftwareGpuRequirement::REQUIRE;
    if (requireSoftwareGpu && impl->softwareGpuDescriptions.empty())
    {
        FINJIN_SET_ERROR(error, "Failed to enumerate software adapter.");
        return;
    }

    if (impl->hardwareGpuDescriptions.empty() && impl->softwareGpuDescriptions.empty())
    {
        FINJIN_SET_ERROR(error, "Failed to enumerate either hardware or software adapters.");
        return;
    }
}

void D3D12System::Destroy()
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

    impl->dxgiFactory.Reset();

    impl.reset();
}

const D3D12HardwareGpuDescriptions& D3D12System::GetHardwareGpuDescriptions() const
{
    return impl->hardwareGpuDescriptions;
}

const D3D12SoftwareGpuDescriptions& D3D12System::GetSoftwareGpuDescriptions() const
{
    return impl->softwareGpuDescriptions;
}

D3D12GpuContext* D3D12System::CreateContext(const D3D12GpuContext::Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL_RETURN(impl, error, nullptr);

    if (impl->contexts.full())
    {
        FINJIN_SET_ERROR(error, "The maximum number of GPU device contexts have already been created.");
        return nullptr;
    }

    auto context = AllocatedClass::NewUnique<D3D12GpuContext>(impl->settings.allocator, FINJIN_CALLER_ARGUMENTS, this);
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

void D3D12System::DestroyContext(D3D12GpuContext* context)
{
    if (context != nullptr)
    {
        std::unique_ptr<D3D12GpuContext> ptr(context);

        auto it = std::find_if(impl->contexts.begin(), impl->contexts.end(), [&context](D3D12GpuContext* c) { return c == context; });
        if (it != impl->contexts.end())
            impl->contexts.erase(it);

        context->Destroy();
    }
}

D3D12SystemImpl* D3D12System::GetImpl()
{
    return impl.get();
}

#endif
