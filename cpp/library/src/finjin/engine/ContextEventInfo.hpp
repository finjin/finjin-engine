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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    struct ContextEventInfo
    {
        using ID = uint32_t;

        ContextEventInfo(ID id = (ID)-1, size_t index = 0, void* contextPointer = nullptr)
        {
            this->id = id;
            this->index = index;
            this->contextPointer = contextPointer;
        }

        bool HasEventID() const
        {
            return this->id != (ID)-1;
        }

        static const ContextEventInfo& GetEmpty()
        {
            static const ContextEventInfo value;
            return value;
        }

        ID id;
        size_t index;
        void* contextPointer;
    };

} }
