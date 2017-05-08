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

#include "D3D12DisplayMode.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
D3D12DisplayMode::D3D12DisplayMode()
{
    this->width = this->height = 0;
    this->format = DXGI_FORMAT_UNKNOWN;
}

D3D12DisplayMode::D3D12DisplayMode(size_t w, size_t h, DXGI_FORMAT f, D3D12RefreshRate r)
{
    this->width = w;
    this->height = h;
    this->format = f;
    this->refresh = r;
}

D3D12DisplayMode::D3D12DisplayMode(const DXGI_MODE_DESC& dxgiDesc)
{
    this->width = dxgiDesc.Width;
    this->height = dxgiDesc.Height;
    this->format = dxgiDesc.Format;
    this->refresh = D3D12RefreshRate(dxgiDesc.RefreshRate);
}

bool D3D12DisplayMode::IsEquivalent(const D3D12DisplayMode& other) const
{
    return
        this->width == other.width &&
        this->height == other.height &&
        this->format == other.format &&
        this->refresh.IsEquivalent(other.refresh);
}

#endif
