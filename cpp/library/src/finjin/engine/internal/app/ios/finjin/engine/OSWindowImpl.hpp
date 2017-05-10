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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/DisplayInfo.hpp"
#include "finjin/engine/OSWindowDefs.hpp"
#include "finjin/engine/OSWindowEventListener.hpp"
#include "finjin/engine/WindowSize.hpp"
#import "FinjinUIWindowController.h"
#import <UIKit/UIWindow.h>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct OSWindowImpl : public AllocatedClass
    {
        OSWindowImpl(Allocator* allocator, void* clientData);
        ~OSWindowImpl();

        void Clear();

        UIWindow* GetWindowHandle();

        CAMetalLayer* GetMetalLayer() const;

        void* clientData;

        NSString* title;
        Utf8String internalName;
        FinjinUIWindowController* windowController;

        WindowSize windowSize;
        StaticVector<OSWindowEventListener*, EngineConstants::MAX_WINDOW_LISTENERS> listeners;

        bool destroyed;
    };

} }
