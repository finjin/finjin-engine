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

#include "D3D12Texture.hpp"

using namespace Finjin::Engine;


//Implementation---------------------------------------------------------------
D3D12Texture::D3D12Texture(Allocator* allocator) : name(allocator)
{
    this->textureIndex = 0;
    this->type = Type::STANDARD_2D;
    this->isResidentCountdown = 0;
    this->waitingToBeResidentNext = nullptr;
}

void D3D12Texture::HandleCreationFailure()
{
    this->resource = nullptr;
    this->uploadHeapResource = nullptr;
}

void D3D12Texture::DisposeUploaders()
{
    this->uploadHeapResource = nullptr;
    this->textureData.Destroy();
}

bool D3D12Texture::IsResidentOnGpu() const
{
    return this->isResidentCountdown == 0;
}

void D3D12Texture::UpdateResidentOnGpuStatus()
{
    if (this->isResidentCountdown > 0) 
        this->isResidentCountdown--;

    if (IsResidentOnGpu())
        DisposeUploaders();
}

#endif
