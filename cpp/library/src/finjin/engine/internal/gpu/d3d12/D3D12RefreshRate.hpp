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


//Includes----------------------------------------------------------------------
#include "D3D12Includes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class D3D12RefreshRate
    {
    public:
        D3D12RefreshRate();
        D3D12RefreshRate(int n, int d);
        D3D12RefreshRate(DXGI_RATIONAL dxgiRefresh);

        bool IsEquivalent(const D3D12RefreshRate& other) const;

        double ToDouble() const;

        static D3D12RefreshRate GetDefault();

    public:
        int numerator;
        int denominator;
    };

} }
