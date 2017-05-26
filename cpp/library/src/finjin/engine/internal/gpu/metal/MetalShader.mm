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

#include "MetalShader.hpp"
#include "finjin/common/Error.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------

//MetalShaderLibrary
MetalShaderLibrary::MetalShaderLibrary()
{
    this->dispatchData = nullptr;
    this->metalLibrary = nullptr;
}

void MetalShaderLibrary::Create(id<MTLDevice> device, const AssetReference& shaderFileAssetReference, AssetClassFileReader& assetClassFileReader, ByteBuffer& readBuffer, Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto readResult = assetClassFileReader.ReadAsset(readBuffer, shaderFileAssetReference);
    if (readResult != FileOperationResult::SUCCESS)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to reader shader library '%1%'.", shaderFileAssetReference.ToUriString()));
        return;
    }

    if (!this->fileBytes.Create(readBuffer.size(), allocator))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate shader buffer for '%1%'.", shaderFileAssetReference.ToUriString()));
        return;
    }

    FINJIN_COPY_MEMORY(this->fileBytes.data(), readBuffer.data(), readBuffer.size());

    {
        NSError* nserror = nullptr;
        this->dispatchData = dispatch_data_create(this->fileBytes.data(), this->fileBytes.size(), nullptr, ^(){});
        this->metalLibrary = [device newLibraryWithData:this->dispatchData error:&nserror];
        if (this->metalLibrary == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create Metal shader library for '%1%': %2%", shaderFileAssetReference.ToUriString(), nserror.localizedDescription.UTF8String));
            return;
        }
    }
}

void MetalShaderLibrary::Destroy()
{
    this->dispatchData = nullptr;
    this->metalLibrary = nullptr;
    this->fileBytes.Destroy();
}

//MetalShader
MetalShader::MetalShader(Allocator* allocator) : functionName(allocator)
{
    this->metalFunction = nullptr;
}

void MetalShader::Create(id<MTLLibrary> shaderLibrary, const Utf8String& functionName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->functionName.assign(functionName).HasError())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign shader function name '%1%'.", functionName));
        return;
    }

    auto nsfunctionName = [NSString stringWithUTF8String:functionName.c_str()];
    this->metalFunction = [shaderLibrary newFunctionWithName:nsfunctionName];
    if (this->metalFunction == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create shader function name '%1%'.", functionName));
        return;
    }
}

void MetalShader::Destroy()
{
    this->functionName.clear();
    this->metalFunction = nullptr;
}

void MetalShader::HandleCreationFailed()
{
    this->functionName.clear();
    this->metalFunction = nullptr;
}

#endif
