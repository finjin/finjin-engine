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

//D3D12AssetResources
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

//D3D12CommonResources
D3D12CommonResources::D3D12CommonResources(Allocator* allocator)
{
}

void D3D12CommonResources::Destroy()
{
    this->fullScreenShaderRootSignature = nullptr;
    this->fullScreenShaderGraphicsPipelineState = nullptr;
    for (auto& buffer : this->fullScreenShaderFileBytes)
        buffer.Destroy();

    this->srvTextureDescHeap.Destroy();
}

#endif
