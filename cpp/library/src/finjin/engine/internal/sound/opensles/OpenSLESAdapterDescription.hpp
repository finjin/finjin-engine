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
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "OpenSLESAdapterID.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct OpenSLESAdapterDescription
    {
        OpenSLESAdapterID adapterID;

        OpenSLESAdapterDescription()
        {
        }

        const OpenSLESAdapterID& GetAdapterID() const
        {
            return this->adapterID;
        }
    };
    class OpenSLESAdapterDescriptions : public StaticVector<OpenSLESAdapterDescription, EngineConstants::MAX_SOUND_ADAPTERS>
    {
    public:
        void SortBestToWorst();

        const OpenSLESAdapterDescription* GetByAdapterID(const OpenSLESAdapterID& adapterID) const;
    };

} }
