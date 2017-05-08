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
#import "FinjinUIWindowController.h"
#import "FinjinUIViewController.h"


//Implementation----------------------------------------------------------------
@class FinjinUIViewController;
@class FinjinUIView;

@interface FinjinUIWindowController ()
@end

@implementation FinjinUIWindowController
{
}

- (FinjinUIView*)view
{
    return (FinjinUIView*)self.window.rootViewController.view;
}

- (CAMetalLayer*)metalLayer
{
    return self.view.metalLayer;
}

+ (FinjinUIWindowController*)createFromWindowFrame:(CGRect)frameRect withScreen:(UIScreen*)screen
{
    auto fullScreenRect = [UIScreen mainScreen].bounds;

    FinjinUIWindowController* windowController = [[FinjinUIWindowController alloc] init];
    windowController.window = [[UIWindow alloc] initWithFrame:fullScreenRect];

    FinjinUIViewController* viewController = [[FinjinUIViewController alloc] init];
    viewController.preferredContentSize = fullScreenRect.size;

    windowController.window.rootViewController = viewController;

    return windowController;
}

@end
