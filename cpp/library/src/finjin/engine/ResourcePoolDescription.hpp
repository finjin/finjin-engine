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

    struct ResourcePoolDescription
    {
        ResourcePoolDescription()
        {
            Reset();
        }

        ResourcePoolDescription(size_t format, size_t count)
        {
            this->format = format;
            this->count = count;
        }

        void Reset()
        {
            this->format = (size_t)-1;
            this->count = 0;
        }

        bool IsValid() const
        {
            return this->format != (size_t)-1 && this->count > 0;
        }

        size_t format;
        size_t count;
    };

} }
