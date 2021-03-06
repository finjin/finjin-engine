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
#include "finjin/engine/OSWindowDefs.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class DisplayInfo
    {
    public:
        DisplayInfo();

    public:
        size_t index;
        bool isPrimary;
        OSWindowRect frame;
        OSWindowRect clientFrame;
        Utf8String name;
        Utf8String deviceName;
        void* monitorHandle;
    };

    class DisplayInfos : public StaticVector<DisplayInfo, EngineConstants::MAX_DISPLAYS>
    {
    public:
        void Enumerate();

        void SortLeftToRight();

        const DisplayInfo* GetByName(const Utf8String& name) const;
    };

} }
