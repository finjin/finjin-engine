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
#import <Cocoa/Cocoa.h>
#import <MetalKit/MTKView.h>


//Types-------------------------------------------------------------------------
@class FinjinNSView;

@interface FinjinNSWindowController : NSWindowController

- (FinjinNSView*)view;
- (CAMetalLayer*)metalLayer;

- (void)disableZoom;

+ (FinjinNSWindowController*)createFromWindowFrame:(NSRect)frameRect withTitle:(NSString*)title withWindowStyle:(NSUInteger)windowStyle withScreen:(NSScreen*)screen;

@end
