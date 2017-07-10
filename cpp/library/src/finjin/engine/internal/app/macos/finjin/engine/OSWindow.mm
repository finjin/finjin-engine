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
#define FINJIN_APPLE_OBJCPP_UTILITIES 1
#include "finjin/common/AppleUtilities.hpp"
#include "finjin/common/DynamicVector.hpp"
#include "OSWindowImpl.hpp"
#import "FinjinNSWindowController.h"
#import "FinjinNSViewController.h"
#import <Foundation/NSString.h>
#import <AppKit/NSScreen.h>
#import <CoreGraphics/CoreGraphics.h>

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#define BORDER_STYLE (NSWindowStyleMaskTitled | NSWindowStyleMaskResizable | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable)
#define BORDERLESS_STYLE NSWindowStyleMaskBorderless


//Local functions---------------------------------------------------------------
/*static bool operator == (const CGPoint a, const CGPoint b)
{
    return a.x == b.x && a.y == b.y;
}*/

static bool operator == (const NSSize a, const NSSize b)
{
    return a.width == b.width && a.height == b.height;
}

/*static bool operator == (const NSRect a, const NSRect b)
{
    return a.origin == b.origin && a.size == b.size;
}*/

static OSWindowRect NSRectToOSRect(NSRect windowFrame)
{
    return OSWindowRect(windowFrame.origin.x, windowFrame.origin.y, windowFrame.size.width, windowFrame.size.height);
}

static NSRect OSRectToNSRect(OSWindowRect windowFrame)
{
    return CGRectMake(windowFrame.x, windowFrame.y, windowFrame.width, windowFrame.height);
}


//OSWindowNSViewDelegate-----------------
@interface OSWindowNSViewDelegate : NSObject<FinjinNSViewDelegate, NSWindowDelegate>
@end

@implementation OSWindowNSViewDelegate
{
@public
    OSWindow* osWindow;
}

//FinjinNSViewDelegate methods
- (void)mouseDown:(NSEvent*)theEvent
{
    auto density = osWindow->GetDisplayDensity();
    InputCoordinate x(theEvent.locationInWindow.x, InputCoordinate::Type::DIPS, density);
    InputCoordinate y(theEvent.locationInWindow.y, InputCoordinate::Type::DIPS, density);
    auto buttons = static_cast<int>([NSEvent pressedMouseButtons]);

    for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
        osWindow->GetWindowEventListener(listenerIndex)->WindowOnPointerDown(osWindow, PointerType::MOUSE, 0, x, y, buttons);
}

- (void)mouseDragged:(NSEvent*)theEvent
{
    auto density = osWindow->GetDisplayDensity();
    InputCoordinate x(theEvent.locationInWindow.x, InputCoordinate::Type::DIPS, density);
    InputCoordinate y(theEvent.locationInWindow.y, InputCoordinate::Type::DIPS, density);
    auto buttons = static_cast<int>([NSEvent pressedMouseButtons]);

    for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
        osWindow->GetWindowEventListener(listenerIndex)->WindowOnPointerMove(osWindow, PointerType::MOUSE, 0, x, y, buttons);
}

- (void)mouseUp:(NSEvent*)theEvent
{
    auto density = osWindow->GetDisplayDensity();
    InputCoordinate x(theEvent.locationInWindow.x, InputCoordinate::Type::DIPS, density);
    InputCoordinate y(theEvent.locationInWindow.y, InputCoordinate::Type::DIPS, density);
    auto buttons = static_cast<int>([NSEvent pressedMouseButtons]);

    for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
        osWindow->GetWindowEventListener(listenerIndex)->WindowOnPointerUp(osWindow, PointerType::MOUSE, 0, x, y, buttons);
}

- (void)rightMouseDown:(NSEvent*)theEvent
{
    [self mouseDown:theEvent];
}

- (void)rightMouseDragged:(NSEvent*)theEvent
{
    [self mouseDragged:theEvent];
}

- (void)rightMouseUp:(NSEvent*)theEvent
{
    [self mouseUp:theEvent];
}

- (void)otherMouseDown:(NSEvent*)theEvent
{
    [self mouseDown:theEvent];
}

- (void)otherMouseDragged:(NSEvent*)theEvent
{
    [self mouseDragged:theEvent];
}

- (void)otherMouseUp:(NSEvent*)theEvent
{
    [self mouseUp:theEvent];
}

- (void)mouseMoved:(NSEvent*)theEvent
{
    auto density = osWindow->GetDisplayDensity();
    InputCoordinate x(theEvent.locationInWindow.x, InputCoordinate::Type::DIPS, density);
    InputCoordinate y(theEvent.locationInWindow.y, InputCoordinate::Type::DIPS, density);
    auto buttons = static_cast<int>([NSEvent pressedMouseButtons]);

    for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
        osWindow->GetWindowEventListener(listenerIndex)->WindowOnPointerMove(osWindow, PointerType::MOUSE, 0, x, y, buttons);
}

- (void)scrollWheel:(NSEvent*)theEvent
{
    auto density = osWindow->GetDisplayDensity();
    InputCoordinate x(theEvent.locationInWindow.x, InputCoordinate::Type::DIPS, density);
    InputCoordinate y(theEvent.locationInWindow.y, InputCoordinate::Type::DIPS, density);
    auto buttons = static_cast<int>([NSEvent pressedMouseButtons]);
    auto wheelDelta = theEvent.scrollingDeltaY;

    for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
        osWindow->GetWindowEventListener(listenerIndex)->WindowOnMouseWheel(osWindow, PointerType::MOUSE, 0, x, y, buttons, wheelDelta);
}

- (void)keyDown:(NSEvent*)theEvent
{
    [self handleKeyEvent:theEvent pressed:true];
}

- (void)keyUp:(NSEvent*)theEvent
{
    [self handleKeyEvent:theEvent pressed:false];
}

- (void)droppedFiles:(NSArray*)files
{
    if (osWindow->GetWindowEventListenerCount() > 0)
    {
        DynamicVector<Path> fileNames;
        if (fileNames.Create(files.count, FINJIN_ALLOCATOR_NULL, FINJIN_ALLOCATOR_NULL))
        {
            for (NSUInteger fileIndex = 0; fileIndex < files.count; fileIndex++)
            {
                auto* s = (NSString*)files[fileIndex];
                fileNames[fileIndex] = s.UTF8String;
            }

            for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
                osWindow->GetWindowEventListener(listenerIndex)->WindowOnDropFiles(osWindow, fileNames.data(), fileNames.size());
        }
    }
}

- (void)handleKeyEvent:(NSEvent*)theEvent pressed:(bool)pressed
{
    NSString* inputCharacters = theEvent.charactersIgnoringModifiers;
    auto scanCode = theEvent.keyCode;
    auto virtualKey = inputCharacters.length > 0 ? [inputCharacters characterAtIndex:0] : 0;
    auto controlDown = (theEvent.modifierFlags & NSEventModifierFlagControl) != 0;
    auto shiftDown = (theEvent.modifierFlags & NSEventModifierFlagShift) != 0;
    auto altDown = (theEvent.modifierFlags & NSEventModifierFlagOption) != 0;

    for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
    {
        if (pressed)
            osWindow->GetWindowEventListener(listenerIndex)->WindowOnKeyDown(osWindow, virtualKey, scanCode, controlDown, shiftDown, altDown);
        else
            osWindow->GetWindowEventListener(listenerIndex)->WindowOnKeyUp(osWindow, virtualKey, scanCode, controlDown, shiftDown, altDown);
    }
}

//NSWindowDelegate methods
- (void)windowDidEndLiveResize:(NSNotification*)notification
{
    if (osWindow != nullptr)
    {
        osWindow->GetWindowSize().WindowResized(osWindow->GetImpl()->IsMaximized());

        for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
            osWindow->GetWindowEventListener(listenerIndex)->WindowResized(osWindow);
    }
}

- (void)windowDidMove:(NSNotification*)notification
{
    if (osWindow != nullptr)
    {
        osWindow->GetWindowSize().WindowMoved(osWindow->GetImpl()->IsMaximized());

        for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
            osWindow->GetWindowEventListener(listenerIndex)->WindowMoved(osWindow);
    }
}

- (void)windowDidBecomeKey:(NSNotification*)notification
{
    if (osWindow != nullptr)
    {
        for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
            osWindow->GetWindowEventListener(listenerIndex)->WindowGainedFocus(osWindow);
    }
}

- (void)windowDidResignKey:(NSNotification*)notification
{
    if (osWindow != nullptr)
    {
        for (size_t listenerIndex = 0; listenerIndex < osWindow->GetWindowEventListenerCount(); listenerIndex++)
            osWindow->GetWindowEventListener(listenerIndex)->WindowLostFocus(osWindow);
    }
}

- (void)windowWillClose:(NSNotification*)notification
{
    if (osWindow != nullptr)
        osWindow->NotifyListenersClosing();
}

@end


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

    impl->windowController = [FinjinNSWindowController createFromWindowFrame:OSRectToNSRect(windowRect) withTitle:impl->title withWindowStyle:BORDER_STYLE withScreen:[NSScreen mainScreen]];

    auto viewDelegate = [[OSWindowNSViewDelegate alloc] init];
    viewDelegate->osWindow = this;
    impl->windowController.window.delegate = viewDelegate;
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

        [impl->windowController close];
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

    NSAlert* alert = [[NSAlert alloc] init];
    [alert setMessageText:nsTitle];
    [alert setInformativeText:nsMessage];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
}

void OSWindow::ShowErrorMessage(const Utf8String& message, const Utf8String& title)
{
    Utf8String errorMessage;
    errorMessage = "Error - ";
    errorMessage += message;
    NSString* nsMessage = [[NSString alloc] initWithUTF8String:errorMessage.c_str()];
    NSString* nsTitle = [[NSString alloc] initWithUTF8String:title.c_str()];

    NSAlert* alert = [[NSAlert alloc] init];
    [alert setMessageText:nsTitle];
    [alert setInformativeText:nsMessage];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
}

void OSWindow::ApplyWindowSize()
{
    impl->windowSize.SetApplyingToWindow(true);

    auto bounds = impl->windowSize.GetCurrentBounds();

    if (bounds.HasBorder())
        SetBorderedStyle();
    else
        SetBorderlessStyle();

    [NSMenu setMenuBarVisible:!impl->windowSize.IsFullScreenExclusive()];

    LimitBounds(bounds);

    switch (impl->windowSize.GetState())
    {
        case WindowSizeState::WINDOWED_NORMAL:
        {
            Move(bounds.x, bounds.y, bounds.GetClientWidth(), bounds.GetClientHeight(), true);
            break;
        }
        case WindowSizeState::WINDOWED_MAXIMIZED:
        {
            Maximize();
            break;
        }
        case WindowSizeState::BORDERLESS_FULLSCREEN:
        {
            Move(bounds.x, bounds.y, bounds.GetClientWidth(), bounds.GetClientHeight(), true);
            break;
        }
        case WindowSizeState::EXCLUSIVE_FULLSCREEN:
        {
            Move(bounds.x, bounds.y, bounds.GetClientWidth(), bounds.GetClientHeight(), true);
            break;
        }
        default: break;
    }

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

OSWindowCoordinate OSWindow::GetBackingScale() const
{
    auto window = impl->windowController.window;
    auto screen = window.screen;
    return screen.backingScaleFactor;
}

OSWindowSize OSWindow::GetBackingSize() const
{
    auto nativeScale = GetBackingScale();
    CGSize size = impl->GetMetalLayer().bounds.size;
    size.width *= nativeScale;
    size.height *= nativeScale;
    return OSWindowSize(size.width, size.height);
}

void OSWindow::Center()
{
    [impl->windowController.window center];
}

void OSWindow::Maximize()
{
    auto theFrame = impl->windowController.window.screen.visibleFrame;

    //This is kind of a hack to prevent IsMaximized() from returning true. This should be addressed at some point
    theFrame.origin.y += 1;
    theFrame.size.height -= 1;

    [impl->windowController.window setFrame:theFrame display:YES animate:YES];
}

bool OSWindow::IsMaximized() const
{
    return impl->IsMaximized();
}

bool OSWindow::IsMinimized() const
{
    return impl->windowController.window.isMiniaturized;
}

bool OSWindow::IsVisible() const
{
    return impl->windowController.window.isOnActiveSpace;
}

bool OSWindow::IsSizeLocked() const
{
    return impl->windowController.window.minSize == impl->windowController.window.maxSize;
}

void OSWindow::LockSize(OSWindowSize size)
{
    impl->windowController.window.minSize = impl->windowController.window.maxSize = CGSizeMake(size.width, size.height);
}

void OSWindow::UnlockSize(OSWindowSize minSize)
{
    impl->windowController.window.minSize = CGSizeMake(minSize.width, minSize.height);
    impl->windowController.window.maxSize = CGSizeMake(10000, 10000);
}

bool OSWindow::HasFocus() const
{
    return impl->windowController.window.isKeyWindow;
}

void OSWindow::Raise()
{
    [impl->windowController.window makeKeyAndOrderFront:nullptr];
    [impl->windowController.window makeFirstResponder:impl->windowController.view];
}

void OSWindow::SetBorderedStyle()
{
    [impl->windowController.window setStyleMask:BORDER_STYLE];
    impl->windowController.window.title = impl->title; //Re-set the title since it is lost when toggling to borderless

    //Re-disable the zoom button
    [impl->windowController disableZoom];

    //Restore first responder
    [impl->windowController.window makeFirstResponder:impl->windowController.view];
}

void OSWindow::SetBorderlessStyle()
{
    [impl->windowController.window setStyleMask:BORDERLESS_STYLE];

    //Re-disable the zoom button
    [impl->windowController disableZoom];

    //Restore first responder
    [impl->windowController.window makeFirstResponder:impl->windowController.view];
}

float OSWindow::GetDisplayDensity() const
{
    return GetBackingScale();
}

int OSWindow::GetDisplayID() const
{
    NSDictionary* screenDictionary = [impl->windowController.window.screen deviceDescription];
    NSNumber* screenID = [screenDictionary objectForKey:@"NSScreenNumber"];
    return screenID.intValue;
}

OSWindowRect OSWindow::GetDisplayRect() const
{
    return NSRectToOSRect(impl->windowController.window.screen.frame);
}

OSWindowRect OSWindow::GetDisplayVisibleRect() const
{
    return NSRectToOSRect(impl->windowController.window.screen.visibleFrame);
}

OSWindowRect OSWindow::GetRect() const
{
    return NSRectToOSRect(impl->windowController.window.frame);
}

OSWindowSize OSWindow::GetClientSize() const
{
    auto clientSize = impl->windowController.window.contentView.frame.size;
    return OSWindowSize(clientSize.width, clientSize.height);
}

bool OSWindow::Move(OSWindowCoordinate x, OSWindowCoordinate y, bool animate)
{
    auto windowFrame = impl->windowController.window.frame;
    windowFrame.origin.x = x;
    windowFrame.origin.y = y;
    [impl->windowController.window setFrame:windowFrame display:YES animate:(animate ? YES : NO)];

    return true;
}

bool OSWindow::Move(OSWindowCoordinate x, OSWindowCoordinate y, OSWindowDimension clientWidth, OSWindowDimension clientHeight, bool animate)
{
    auto currentContentSize = impl->windowController.window.contentView.frame;
    auto windowFrame = impl->windowController.window.frame;

    windowFrame.origin.x = x;
    windowFrame.origin.y = y;
    windowFrame.size.width += (clientWidth - currentContentSize.size.width);
    windowFrame.size.height += (clientHeight - currentContentSize.size.height);

    [impl->windowController.window setFrame:windowFrame display:YES animate:(animate ? YES : NO)];

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
    auto displayID = GetDisplayID();
    auto screenFrame = CGDisplayBounds(displayID);

    auto windowFrame = impl->windowController.window.frame;

    auto center = CGPointMake(windowFrame.origin.x + windowFrame.size.width/2, windowFrame.origin.y + windowFrame.size.height/2);
    center.y = screenFrame.size.height - center.y + screenFrame.origin.y; //Convert from lower left screen origin to upper left global origin

    CGWarpMouseCursorPosition(center);

    return true;
}

void OSWindow::LimitBounds(WindowBounds& bounds) const
{
    auto screen = impl->windowController.window.screen;
    auto frame = impl->windowSize.IsFullScreenExclusive() ? screen.frame : screen.visibleFrame;
    if (impl->windowSize.IsFullScreen() || (bounds.width == 0 && bounds.height == 0))
    {
        bounds.x = frame.origin.x;
        bounds.y = frame.origin.y;
        bounds.width = bounds.clientWidth = frame.size.width;
        bounds.height = bounds.clientHeight = frame.size.height;
    }

    auto nsbounds = CGRectMake(bounds.x, bounds.y, bounds.width, bounds.height);
    AppleUtilities::CenterDefaultsInParent(nsbounds, screen.visibleFrame, FINJIN_OS_WINDOW_COORDINATE_DEFAULT);

    auto constrainedBounds = [impl->windowController.window constrainFrameRect:nsbounds toScreen:screen];
    bounds.x = constrainedBounds.origin.x;
    bounds.y = constrainedBounds.origin.y;
    bounds.AdjustSize(constrainedBounds.size.width, constrainedBounds.size.height);
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
