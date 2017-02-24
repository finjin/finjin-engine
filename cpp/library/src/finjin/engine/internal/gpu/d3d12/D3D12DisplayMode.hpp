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


#pragma once


//Includes---------------------------------------------------------------------
#include "D3D12Includes.hpp"
#include "D3D12RefreshRate.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class D3D12DisplayMode
    {
    public:
        D3D12DisplayMode();
        D3D12DisplayMode(size_t w, size_t h, DXGI_FORMAT f, D3D12RefreshRate r);
        D3D12DisplayMode(float w, float h, DXGI_FORMAT f, D3D12RefreshRate r);
        D3D12DisplayMode(const DXGI_MODE_DESC& dxgiDesc);
    
        bool IsEquivalent(const D3D12DisplayMode& other) const;

    public:
        size_t width;
        size_t height;
        DXGI_FORMAT format;
        D3D12RefreshRate refresh;
    };

} }
