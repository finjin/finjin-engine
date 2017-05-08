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

#include "MetalSystemImpl.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
MetalSystemImpl::MetalSystemImpl(Allocator* allocator) : AllocatedClass(allocator)
{
}

MetalDevices& MetalSystemImpl::GetDevices()
{
    return this->devices;
}

id<MTLDevice> MetalSystemImpl::GetBestMetalDevice(Utf8String& gpuID, MetalGpuDescription& deviceDescription, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    id<MTLDevice> deviceResult = nullptr;

    if (!this->devices.empty())
    {
        //Find by ID
        if (!gpuID.empty())
        {
            deviceResult = this->devices.GetByGpuID(gpuID);
            if (deviceResult != nullptr)
            {
                //Find the associated description
                for (auto& desc : this->deviceDescriptions)
                {
                    if (desc.GetGpuID() == gpuID)
                    {
                        deviceDescription = desc;
                        break;
                    }
                }
            }
        }

        //If the device couldn't be found, use the best default device
        if (deviceResult == nullptr)
        {
            for (auto& desc : this->deviceDescriptions)
            {
                deviceResult = this->devices.GetByGpuID(desc.GetGpuID());
                assert(deviceResult != nullptr);
                if (deviceResult != nullptr)
                {
                    deviceDescription = desc;
                    break;
                }
            }
        }
    }

    if (deviceResult == nullptr)
        FINJIN_SET_ERROR(error, "Failed to get best Metal device.");

    return deviceResult;
}

#endif
