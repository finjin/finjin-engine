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
#include "OpenSLESAdapterDescription.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
void OpenSLESAdapterDescriptions::SortBestToWorst()
{
    //Do nothing. Assume the descriptions are already in the best order
}

const OpenSLESAdapterDescription* OpenSLESAdapterDescriptions::GetByAdapterID(const OpenSLESAdapterID& adapterID) const
{
    for (const auto& item : *this)
    {
        if (item.adapterID == adapterID)
            return &item;
    }

    return nullptr;
}
