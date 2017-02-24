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
#include <Windows.h>


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    struct D3D12GpuID : LUID
    {
        D3D12GpuID()
        {
            this->LowPart = 0;
            this->HighPart = 0;
        }

        D3D12GpuID(const LUID& luid)
        {
            this->HighPart = luid.HighPart;
            this->LowPart = luid.LowPart;
        }

        bool empty() const
        {
            return IsZero();
        }

        bool IsZero() const
        {
            return this->HighPart == 0 && this->LowPart == 0;
        }

        bool operator == (const LUID& other) const
        {
            return this->HighPart == other.HighPart && this->LowPart == other.LowPart;
        }

        D3D12GpuID& operator = (const LUID& other)
        {
            this->LowPart = other.LowPart;
            this->HighPart = other.HighPart;
            return *this;
        }

        D3D12GpuID& operator = (const D3D12GpuID& other)
        {
            this->LowPart = other.LowPart;
            this->HighPart = other.HighPart;
            return *this;
        }
    };

} }
