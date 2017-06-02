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
#include "finjin/engine/FinjinEngineLibrary.hpp"
#import "FinjinNSWindowController.h"
#define FINJIN_APPLE_OBJCPP_UTILITIES 1
#include "finjin/common/AppleUtilities.hpp"
#include "finjin/engine/OSWindowDefs.hpp"
#import "FinjinNSViewController.h"

using namespace Finjin::Common;


//Implementation----------------------------------------------------------------
@class FinjinNSViewController;
@class FinjinNSView;

@interface FinjinNSWindowController ()
@end

@implementation FinjinNSWindowController
{
}

- (FinjinNSView*)view
{
    auto viewController = (FinjinNSViewController*)self.window.contentViewController;
    return (FinjinNSView*)viewController.view;
}

- (CAMetalLayer*)metalLayer
{
    return self.view.metalLayer;
}

- (void)disableZoom
{
    auto button = [self.window standardWindowButton:NSWindowZoomButton];
    button.enabled = NO;
}

+ (FinjinNSWindowController*)createFromWindowFrame:(NSRect)frameRect withTitle:(NSString*)title withWindowStyle:(NSUInteger)windowStyle withScreen:(NSScreen*)screen
{
    AppleUtilities::PositionWindowRect(frameRect, [NSScreen mainScreen].visibleFrame, FINJIN_OS_WINDOW_COORDINATE_DEFAULT);
    
    auto contentRect = [NSWindow contentRectForFrameRect:frameRect styleMask:windowStyle];
    contentRect.origin = frameRect.origin;

    auto windowController = [[FinjinNSWindowController alloc] init];
    windowController.window = [[NSWindow alloc] initWithContentRect:contentRect styleMask:windowStyle backing:NSBackingStoreBuffered defer:YES screen:screen];
    windowController.window.title = title;
    windowController.window.acceptsMouseMovedEvents = YES;
    [windowController.window setFrame:frameRect display:NO];

    auto viewController = [[FinjinNSViewController alloc] init];
    viewController.preferredContentSize = contentRect.size;

    [windowController.window setContentViewController:viewController];

    [windowController disableZoom];

    return windowController;
}

@end
