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
#include "OSWindow.hpp"
#include "OSWindowImpl.hpp"
#import "FinjinUIWindowController.h"
#import "FinjinUIViewController.h"
#import <Foundation/NSString.h>
#import <UIKit/UIKit.h>
#import <CoreGraphics/CoreGraphics.h>

#define TouchToPointerID(touch) static_cast<int>(reinterpret_cast<intptr_t>(touch))

using namespace Finjin::Engine;


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


//Local functions---------------------------------------------------------------
static OSWindowRect CGRectToOSRect(CGRect windowFrame)
{
    return OSWindowRect(windowFrame.origin.x, windowFrame.origin.y, windowFrame.size.width, windowFrame.size.height);
}

static CGRect OSRectToCGRect(OSWindowRect windowFrame)
{
    return CGRectMake(windowFrame.x, windowFrame.y, windowFrame.width, windowFrame.height);
}


//Implementation----------------------------------------------------------------
OSWindow::OSWindow(Allocator* allocator, void* clientData) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<OSWindowImpl>(allocator, FINJIN_CALLER_ARGUMENTS, clientData))
{
}

OSWindow::~OSWindow()
{
    Destroy();
}

void OSWindow::Create(const Utf8String& internalName, const Utf8String& titleOrSubtitle, const Utf8String& displayName, OSWindowRect windowRect, const WindowSize& windowSize, Error& error)
{
    //Note: There is a ViewerWindow.xib file in this project but it isn't used

    FINJIN_ERROR_METHOD_START(error);

    //Make copy of settings
    impl->title = [[NSString alloc] initWithUTF8String:titleOrSubtitle.c_str()];
    impl->internalName = internalName;

    impl->windowSize = windowSize;
    impl->windowSize.SetWindow(this);

    impl->windowController = [FinjinUIWindowController createFromWindowFrame:OSRectToCGRect(windowRect) withScreen:[UIScreen mainScreen]];

    auto viewDelegate = [[OSWindowUIViewDelegate alloc] init];
    viewDelegate->osWindow = this;
    impl->windowController.view.delegate = viewDelegate;

    impl->windowSize.SetApplyingToWindow(true);
    Raise();
    impl->windowSize.SetApplyingToWindow(false);
}

void OSWindow::Destroy()
{
    if (!impl->destroyed)
    {
        impl->destroyed = true;
    }
}

const Utf8String& OSWindow::GetInternalName() const
{
    return impl->internalName;
}

void OSWindow::ShowMessage(const Utf8String& message, const Utf8String& title)
{
    NSString* nsMessage = [[NSString alloc] initWithUTF8String:message.c_str()];
    NSString* nsTitle = [[NSString alloc] initWithUTF8String:title.c_str()];

    UIAlertController* alert = [UIAlertController alertControllerWithTitle:nsTitle message:nsMessage preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"Ok" style:UIAlertActionStyleDefault handler:^(UIAlertAction * action) {}];
    [alert addAction:defaultAction];

    [impl->windowController.window.rootViewController presentViewController:alert animated:YES completion:nullptr];
}

void OSWindow::ShowErrorMessage(const Utf8String& message, const Utf8String& title)
{
    //OSWindow::ShowErrorMessage(...);
    //UIAlertView* alertView = [[UIAlertView alloc] initWithTitle:@"EXCEPTION" message:[NSString stringWithFormat:@"(%s) EXCEPTION: %@",__PRETTY_FUNCTION__,exception.description] delegate:nullptr cancelButtonTitle:@"Ok" otherButtonTitles:nullptr];
    //[alertView show];

    /*Utf8String errorMessage("Error - " + message);
    NSString* nsMessage = [[NSString alloc] initWithUTF8String:errorMessage.c_str()];
    NSString* nsTitle = [[NSString alloc] initWithUTF8String:title.c_str()];

    UIAlertController* alert = [UIAlertController alertControllerWithTitle:nsTitle message:nsMessage preferredStyle:UIAlertControllerStyleAlert];

    UIAlertAction* defaultAction = [UIAlertAction actionWithTitle:@"Ok" style:UIAlertActionStyleDefault handler:^(UIAlertAction * action) {}];
    [alert addAction:defaultAction];

    [impl->windowController.window.rootViewController presentViewController:alert animated:YES completion:nullptr];*/
}

void OSWindow::ApplyWindowSize()
{
    impl->windowSize.SetApplyingToWindow(true);

    auto bounds = impl->windowSize.GetCurrentBounds();

    LimitBounds(bounds);

    Move(bounds.x, bounds.y, bounds.GetClientWidth(), bounds.GetClientHeight(), false);

    impl->windowSize.SetApplyingToWindow(false);
}

WindowSize& OSWindow::GetWindowSize()
{
    return impl->windowSize;
}

const WindowSize& OSWindow::GetWindowSize() const
{
    return impl->windowSize;
}

float OSWindow::GetBackingScale() const
{
    auto window = impl->windowController.window;
    auto screen = window.screen;
    return screen.nativeScale;
}

OSWindowSize OSWindow::GetBackingSize() const
{
    auto nativeScale = GetBackingScale();
    CGSize size = impl->windowController.view.frame.size;
    size.width *= nativeScale;
    size.height *= nativeScale;
    return OSWindowSize(size.width, size.height);
}

bool OSWindow::IsMaximized() const
{
    return false;
}

bool OSWindow::IsMinimized() const
{
    return false;
}

bool OSWindow::IsVisible() const
{
    return true;
}

bool OSWindow::IsSizeLocked() const
{
    return false;
}

void OSWindow::LockSize(OSWindowSize size)
{
}

void OSWindow::UnlockSize(OSWindowSize minSize)
{
}

bool OSWindow::HasFocus() const
{
    return impl->windowController.window.isKeyWindow;
}

void OSWindow::Raise()
{
    [impl->windowController.window makeKeyAndVisible];
}

int OSWindow::GetDisplayID() const
{
    return 0;
}

float OSWindow::GetDisplayDensity() const
{
    return GetBackingScale();
}

OSWindowRect OSWindow::GetDisplayRect() const
{
    return CGRectToOSRect(impl->windowController.window.screen.bounds);
}

OSWindowRect OSWindow::GetDisplayVisibleRect() const
{
    return CGRectToOSRect(impl->windowController.window.screen.bounds);
}

OSWindowRect OSWindow::GetRect() const
{
    return CGRectToOSRect(impl->windowController.window.frame);
}

OSWindowSize OSWindow::GetClientSize() const
{
    auto clientSize = impl->windowController.window.rootViewController.view.frame.size;
    return OSWindowSize(clientSize.width, clientSize.height);
}

bool OSWindow::Move(OSWindowCoordinate x, OSWindowCoordinate y, bool animate)
{
    auto windowFrame = impl->windowController.window.frame;
    windowFrame.origin.x = x;
    windowFrame.origin.y = y;
    impl->windowController.window.frame = windowFrame;

    return true;
}

bool OSWindow::Move(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension clientWidth, OSWindowDimension clientHeight, bool animate)
{
    auto currentContentSize = impl->windowController.window.rootViewController.view.frame;
    auto windowFrame = impl->windowController.window.frame;

    windowFrame.origin.x = x;
    windowFrame.origin.y = y;
    windowFrame.size.width += (clientWidth - currentContentSize.size.width);
    windowFrame.size.height += (clientHeight - currentContentSize.size.height);

    impl->windowController.window.frame = windowFrame;

    return true;
}

bool OSWindow::HasWindowHandle() const
{
    return impl->windowController != nullptr && impl->windowController.window != nullptr;
}

void OSWindow::ClearSystemWindowHandle()
{
    //Do nothing
}

size_t OSWindow::GetWindowEventListenerCount() const
{
    return impl->listeners.size();
}

OSWindowEventListener* OSWindow::GetWindowEventListener(size_t index)
{
    return impl->listeners[index];
}

void OSWindow::AddWindowEventListener(OSWindowEventListener* listener)
{
    impl->listeners.push_back(listener);
}

void OSWindow::RemoveWindowEventListener(OSWindowEventListener* listener)
{
    auto foundAt = impl->listeners.find(listener);
    if (foundAt != impl->listeners.end())
        impl->listeners.erase(foundAt);
}

void OSWindow::NotifyListenersClosing()
{
    auto listeners = impl->listeners;
    for (auto listener : listeners)
        listener->WindowClosing(this);
}

void* OSWindow::GetClientData()
{
    return impl->clientData;
}

void OSWindow::SetClientData(void* data)
{
    impl->clientData = data;
}

bool OSWindow::CenterCursor()
{
    return false;
}

void OSWindow::LimitBounds(WindowBounds& bounds) const
{
    auto screen = impl->windowController.window.screen;
    auto frame = screen.bounds;
    bounds.x = frame.origin.x;
    bounds.y = frame.origin.y;
    bounds.width = bounds.clientWidth = frame.size.width;
    bounds.height = bounds.clientHeight = frame.size.height;
}

void OSWindow::GetMetalLayerSize(OSWindowCoordinate& width, OSWindowCoordinate& height)
{
    auto frameSize = impl->windowController.metalLayer.frame.size;
    width = frameSize.width;
    height = frameSize.height;
}

OSWindowImpl* OSWindow::GetImpl()
{
    return impl.get();
}
