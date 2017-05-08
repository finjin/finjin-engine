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


//Implementation----------------------------------------------------------------
D3D12AssetResources::D3D12AssetResources()
{
    this->waitingToBeResidentTexturesHead = nullptr;
    this->waitingToBeResidentMeshesHead = nullptr;
}

void D3D12AssetResources::Destroy()
{
    this->inputFormatsByNameHash.Destroy();

    for (auto& texture : this->texturesByNameHash)
        texture.second.Destroy();
    this->texturesByNameHash.Destroy();

    for (auto& count : this->textureOffsetsByDimension)
        count.Reset();

    for (auto& shaders : this->shadersByShaderTypeAndNameHash)
        shaders.Destroy();

    this->materialsByNameHash.Destroy();

    for (auto& mesh : this->meshesByNameHash)
        mesh.second.Destroy();
    this->meshesByNameHash.Destroy();

    this->waitingToBeResidentTexturesHead = nullptr;
    this->waitingToBeResidentMeshesHead = nullptr;
}

void D3D12AssetResources::ValidateTextureForCreation(const Utf8String& name, Error& error)
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

D3D12Texture* D3D12AssetResources::GetTextureByName(const Utf8String& name)
{
    auto foundAt = this->texturesByNameHash.find(name.GetHash());
    if (foundAt != this->texturesByNameHash.end())
        return &foundAt->second;
    return nullptr;
}

void D3D12AssetResources::ValidateMeshForCreation(const Utf8String& name, Error& error)
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

D3D12Mesh* D3D12AssetResources::GetMeshByName(const Utf8String& name)
{
    auto foundAt = this->meshesByNameHash.find(name.GetHash());
    if (foundAt != this->meshesByNameHash.end())
        return &foundAt->second;
    return nullptr;
}

void D3D12AssetResources::ValidateMaterialForCreation(const Utf8String& name, Error& error)
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

D3D12Material* D3D12AssetResources::GetMaterialByName(const Utf8String& name)
{
    auto foundAt = this->materialsByNameHash.find(name.GetHash());
    if (foundAt != this->materialsByNameHash.end())
        return &foundAt->second;
    return nullptr;
}

bool D3D12AssetResources::ValidateShaderForCreation(D3D12ShaderType shaderType, const Utf8String& name, Error& error)
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

D3D12Shader* D3D12AssetResources::GetShaderByName(D3D12ShaderType shaderType, const Utf8String& name)
{
    auto& shadersByNameHash = this->shadersByShaderTypeAndNameHash[shaderType];

    auto foundAt = shadersByNameHash.find(name.GetHash());
    if (foundAt != shadersByNameHash.end())
        return &foundAt->second;
    return nullptr;
}

D3D12InputFormat* D3D12AssetResources::GetInputFormatByTypeName(const Utf8String& name)
{
    auto foundAt = this->inputFormatsByNameHash.find(name.GetHash());
    if (foundAt != this->inputFormatsByNameHash.end())
        return &foundAt->second;
    return nullptr;
}

#endif
