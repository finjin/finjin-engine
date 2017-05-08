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
#include "finjin/engine/FinjinEngineLibrary.hpp"


//Macros------------------------------------------------------------------------
#define FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error) \
    if (impl == nullptr) \
    { \
        FINJIN_SET_ERROR(error, "Internal implementation is null."); \
        return; \
    }
#define FINJIN_ENGINE_CHECK_IMPL_NOT_NULL_RETURN(impl, error, returnValue) \
    if (impl == nullptr) \
    { \
        FINJIN_SET_ERROR(error, "Internal implementation is null."); \
        return returnValue; \
    }
#define FINJIN_ENGINE_CHECK_CREATED_CONTEXT_NOT_NULL_RETURN(context, error, returnValue) \
    if (context == nullptr) \
    { \
        FINJIN_SET_ERROR(error, "Failed to create context."); \
        return returnValue; \
    }
