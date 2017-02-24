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

#include "D3D12Resources.hpp"

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
D3D12Resources::D3D12Resources()
{
    this->waitingToBeResidentTexturesHead = nullptr;
    this->waitingToBeResidentMeshesHead = nullptr;    
}

void D3D12Resources::ValidateTextureForCreation(const Utf8String& name, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->texturesByNameHash.full())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create texture '%1%'. Texture lookup is full.", name));
        return;
    }
    if (this->texturesByNameHash.contains(name.GetHash()))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create texture '%1%'. The name already exists.", name));
        return;
    }
}

D3D12Texture* D3D12Resources::GetTextureByName(const Utf8String& name)
{
    auto foundAt = this->texturesByNameHash.find(name.GetHash());
    if (foundAt != this->texturesByNameHash.end())
        return &foundAt->second;
    return nullptr;
}

void D3D12Resources::ValidateMeshForCreation(const Utf8String& name, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->meshesByNameHash.full())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create mesh '%1%'. Mesh lookup is full.", name));
        return;
    }
    if (this->meshesByNameHash.contains(name.GetHash()))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create mesh '%1%'. The name already exists.", name));
        return;
    }
}

D3D12Mesh* D3D12Resources::GetMeshByName(const Utf8String& name)
{
    auto foundAt = this->meshesByNameHash.find(name.GetHash());
    if (foundAt != this->meshesByNameHash.end())
        return &foundAt->second;
    return nullptr;
}

void D3D12Resources::ValidateMaterialForCreation(const Utf8String& name, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->materialsByNameHash.full())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create material '%1%'. Material lookup is full.", name));
        return;
    }
    if (this->materialsByNameHash.contains(name.GetHash()))
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create material '%1%'. The name already exists.", name));
        return;
    }
}

D3D12Material* D3D12Resources::GetMaterialByName(const Utf8String& name)
{
    auto foundAt = this->materialsByNameHash.find(name.GetHash());
    if (foundAt != this->materialsByNameHash.end())
        return &foundAt->second;
    return nullptr;
}

bool D3D12Resources::ValidateShaderForCreation(D3D12ShaderType shaderType, const Utf8String& name, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto& shadersByNameHash = this->shadersByShaderTypeAndNameHash[shaderType];

    if (shadersByNameHash.full())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create shader '%1%'. Shader lookup is full.", name));
        return false;
    }

    //Encountering a duplicate shader isn't an error condition
    if (shadersByNameHash.contains(name.GetHash()))
        return false;
    
    return true;
}

D3D12Shader* D3D12Resources::GetShaderByName(D3D12ShaderType shaderType, const Utf8String& name)
{
    auto& shadersByNameHash = this->shadersByShaderTypeAndNameHash[shaderType];

    auto foundAt = shadersByNameHash.find(name.GetHash());
    if (foundAt != shadersByNameHash.end())
        return &foundAt->second;
    return nullptr;
}

D3D12InputFormat* D3D12Resources::GetInputFormatByTypeName(const Utf8String& name)
{
    auto foundAt = this->inputFormatsByNameHash.find(name.GetHash());
    if (foundAt != this->inputFormatsByNameHash.end())
        return &foundAt->second;    
    return nullptr;
}

#endif
