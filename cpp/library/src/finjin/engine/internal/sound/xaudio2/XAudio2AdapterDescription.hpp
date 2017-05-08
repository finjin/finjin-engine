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
#include "XAudio2AdapterID.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct XAudio2AdapterDescription
    {
        XAudio2AdapterID adapterID;

        XAudio2AdapterDescription()
        {
        }

        const XAudio2AdapterID& GetAdapterID() const
        {
            return this->adapterID;
        }
    };
    class XAudio2AdapterDescriptions : public StaticVector<XAudio2AdapterDescription, EngineConstants::MAX_SOUND_ADAPTERS>
    {
    public:
        void SortBestToWorst();

        const XAudio2AdapterDescription* GetByAdapterID(const XAudio2AdapterID& adapterID) const;
    };

} }
