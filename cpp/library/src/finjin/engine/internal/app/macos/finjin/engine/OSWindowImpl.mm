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
#include "OSWindowImpl.hpp"
#import "FinjinNSWindowController.h"
#import "FinjinNSViewController.h"
#import <Foundation/NSString.h>
#import <AppKit/NSScreen.h>
#import <CoreGraphics/CoreGraphics.h>

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static bool operator == (const CGPoint a, const CGPoint b)
{
    return a.x == b.x && a.y == b.y;
}

static bool operator == (const NSSize a, const NSSize b)
{
    return a.width == b.width && a.height == b.height;
}

static bool operator == (const NSRect a, const NSRect b)
{
    return a.origin == b.origin && a.size == b.size;
}


//Implementation----------------------------------------------------------------
OSWindowImpl::OSWindowImpl(Allocator* allocator, void* clientData) : AllocatedClass(allocator)
{
    Clear();

    this->clientData = clientData;
}

void OSWindowImpl::Clear()
{
    this->clientData = nullptr;
    this->title = nullptr;
    this->windowController = nullptr;
    this->listeners.clear();
    this->destroyed = false;
}

CAMetalLayer* OSWindowImpl::GetMetalLayer() const
{
    return this->windowController.metalLayer;
}

bool OSWindowImpl::IsMaximized() const
{
    //There's no such thing as a maximized window in macOS.
    //isZoomed is the closest approximation, but that also returns true:
    //  -In full screen mode.
    //  -For windows with NSWindowStyleMaskBorderless style.
    if ((this->windowController.window.styleMask & NSWindowStyleMaskTitled) == NSWindowStyleMaskTitled)
    {
        return
            this->windowController.window.isZoomed ||
            this->windowController.window.frame == this->windowController.window.screen.visibleFrame;
    }
    else
        return false;
}
