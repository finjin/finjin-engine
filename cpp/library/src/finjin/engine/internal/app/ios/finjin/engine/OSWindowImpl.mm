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
#include "OSWindow.hpp"
#import "FinjinUIWindowController.h"
#import "FinjinUIViewController.h"
#import <Foundation/NSString.h>
#import <UIKit/UIKit.h>
#import <CoreGraphics/CoreGraphics.h>

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define TouchToPointerID(touch) static_cast<int>(reinterpret_cast<intptr_t>(touch))


//Local types-------------------------------------------------------------------

//OSWindowUIViewDelegate-----------------
@interface OSWindowUIViewDelegate : NSObject<FinjinUIViewDelegate>
@end

@implementation OSWindowUIViewDelegate
{
@public
    OSWindow* osWindow;
}

//FinjinUIViewDelegate methods
- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)theEvent
{
    auto density = osWindow->GetDisplayDensity();

    for (UITouch* touch in touches)
    {
        auto pointerID = TouchToPointerID(touch);

        auto locationInView = [touch locationInView:osWindow->GetImpl()->GetWindowHandle().subviews.lastObject];
        InputCoordinate x(locationInView.x, InputCoordinate::Type::DIPS, density);
        InputCoordinate y(locationInView.y, InputCoordinate::Type::DIPS, density);

        for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
            osWindow->GetWindowEventListener(listenerIndex)->WindowOnPointerDown(osWindow, PointerType::TOUCH_SCREEN, pointerID, x, y, 0);
    }
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)theEvent
{
    [self touchesEnded:touches withEvent:theEvent];
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)theEvent
{
    auto density = osWindow->GetDisplayDensity();

    for (UITouch* touch in touches)
    {
        auto pointerID = TouchToPointerID(touch);

        auto locationInView = [touch locationInView:osWindow->GetImpl()->GetWindowHandle().subviews.lastObject];
        InputCoordinate x(locationInView.x, InputCoordinate::Type::DIPS, density);
        InputCoordinate y(locationInView.y, InputCoordinate::Type::DIPS, density);

        for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
            osWindow->GetWindowEventListener(listenerIndex)->WindowOnPointerUp(osWindow, PointerType::TOUCH_SCREEN, pointerID, x, y, 0);
    }
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)theEvent
{
    auto density = osWindow->GetDisplayDensity();

    for (UITouch* touch in touches)
    {
        auto pointerID = TouchToPointerID(touch);

        auto locationInView = [touch locationInView:osWindow->GetImpl()->GetWindowHandle().subviews.lastObject];
        InputCoordinate x(locationInView.x, InputCoordinate::Type::DIPS, density);
        InputCoordinate y(locationInView.y, InputCoordinate::Type::DIPS, density);

        for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
            osWindow->GetWindowEventListener(listenerIndex)->WindowOnPointerMove(osWindow, PointerType::TOUCH_SCREEN, pointerID, x, y, 0);
    }
}

@end


//Implementation----------------------------------------------------------------
OSWindowImpl::OSWindowImpl(Allocator* allocator, void* clientData) : AllocatedClass(allocator)
{
    Clear();

    this->clientData = clientData;
}

OSWindowImpl::~OSWindowImpl()
{
}

void OSWindowImpl::Clear()
{
    this->clientData = nullptr;
    this->title = nullptr;
    this->windowController = nullptr;
    this->listeners.clear();
    this->destroyed = false;
}

UIWindow* OSWindowImpl::GetWindowHandle()
{
    return this->windowController.window;
}

CAMetalLayer* OSWindowImpl::GetMetalLayer() const
{
    return this->windowController.metalLayer;
}
