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
#import <Foundation/Foundation.h>


//Types-------------------------------------------------------------------------

//A delegate that can be used to receive touch events sent to the FinjinNSView
@protocol FinjinNSViewDelegate<NSObject>

@optional

- (void)mouseDown:(NSEvent*)theEvent;
- (void)mouseDragged:(NSEvent*)theEvent;
- (void)mouseUp:(NSEvent*)theEvent;

- (void)rightMouseDown:(NSEvent*)theEvent;
- (void)rightMouseDragged:(NSEvent*)theEvent;
- (void)rightMouseUp:(NSEvent*)theEvent;

- (void)otherMouseDown:(NSEvent*)theEvent;
- (void)otherMouseDragged:(NSEvent*)theEvent;
- (void)otherMouseUp:(NSEvent*)theEvent;

- (void)mouseMoved:(NSEvent*)theEvent;

- (void)scrollWheel:(NSEvent*)theEvent;

- (void)touchesBeganWithEvent:(NSEvent*)theEvent;
- (void)touchesCancelledWithEvent:(NSEvent*)theEvent;
- (void)touchesEndedWithEvent:(NSEvent*)theEvent;
- (void)touchesMovedWithEvent:(NSEvent*)theEvent;

- (void)keyDown:(NSEvent*)theEvent;
- (void)keyUp:(NSEvent*)theEvent;

- (BOOL)prepareForDragOperation:(id<NSDraggingInfo>)sender;
- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender;
- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender;
- (void)droppedFiles:(NSArray*)files; //Typically the only drag&drop method that needs to implemented

@end
