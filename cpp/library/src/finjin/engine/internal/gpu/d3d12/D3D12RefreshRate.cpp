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

#include "D3D12RefreshRate.hpp"

using namespace Finjin::Engine;


//Local functions--------------------------------------------------------------
static D3D12RefreshRate NormalizeRatio(D3D12RefreshRate refresh)
{
    while (refresh.numerator > 0 && refresh.numerator % 10 == 0 && 
        refresh.denominator > 0 && refresh.denominator % 10 == 0)
    {
        refresh.numerator /= 10;
        refresh.denominator /= 10;
    }
    return refresh;
}


//Implementation---------------------------------------------------------------
D3D12RefreshRate::D3D12RefreshRate()
{
    this->numerator = this->denominator = 1;
}

D3D12RefreshRate::D3D12RefreshRate(int n, int d)
{
    this->numerator = n;
    this->denominator = d;
}

D3D12RefreshRate::D3D12RefreshRate(DXGI_RATIONAL dxgiRefresh)
{
    this->numerator = dxgiRefresh.Numerator;
    this->denominator = dxgiRefresh.Denominator;
}

bool D3D12RefreshRate::IsEquivalent(const D3D12RefreshRate& other) const
{
    D3D12RefreshRate a = NormalizeRatio(*this);
    D3D12RefreshRate b = NormalizeRatio(other);
    return a.numerator == b.numerator && a.denominator == b.denominator;
}

double D3D12RefreshRate::ToDouble() const
{
    D3D12RefreshRate r = NormalizeRatio(*this);
    return static_cast<double>(r.numerator) / static_cast<double>(r.denominator);
}

D3D12RefreshRate D3D12RefreshRate::GetDefault()
{
    return D3D12RefreshRate(60, 1);
}

#endif
