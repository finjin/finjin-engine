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


#include "finjin/engine/FinjinEngineLibrary.hpp"
#import "FinjinNSView.h"
#import <QuartzCore/CATransaction.h>


//Implementation---------------------------------------------------------------
@interface FinjinNSView ()
@end

@implementation FinjinNSView
{
    CAMetalLayer* _metalLayer;
}

- (id)init
{
    self = [super init];
    if (self)
    {
        [self _finishInit];
    }
    
    return self;
}

- (id)initWithFrame:(NSRect)rect
{
    self = [super initWithFrame:rect];
    if (self)
    {
        [self _finishInit];
    }

    return self;
}

- (void)_finishInit
{
    _metalLayer = [CAMetalLayer layer];
    _metalLayer.framebufferOnly = YES;
    
    self.wantsLayer = YES;
    [self makeBackingLayer];
    [self.layer addSublayer:_metalLayer];
}

- (CAMetalLayer*)metalLayer
{
    return _metalLayer;
}

- (void)_syncLayerToViewSize
{
    //Resizes layer, disabling animation on the resize so that it's immediate
    [CATransaction begin];
    [CATransaction setValue:(id)kCFBooleanTrue forKey:kCATransactionDisableActions];
    [_metalLayer setFrame:self.bounds];
    [CATransaction commit];
}

- (void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];
    
    [self _syncLayerToViewSize];
}

- (void)viewDidEndLiveResize
{
    [self _syncLayerToViewSize];
}

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (void)mouseDown:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(mouseDown:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del mouseDown:theEvent];
    }
}

- (void)mouseDragged:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(mouseDragged:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del mouseDragged:theEvent];
    }
}

- (void)mouseUp:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(mouseUp:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del mouseUp:theEvent];
    }
}

- (void)rightMouseDown:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(rightMouseDown:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del rightMouseDown:theEvent];
    }
}

- (void)rightMouseDragged:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(rightMouseDragged:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del rightMouseDragged:theEvent];
    }
}

- (void)rightMouseUp:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(rightMouseUp:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del rightMouseUp:theEvent];
    }
}

- (void)otherMouseDown:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(otherMouseDown:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del otherMouseDown:theEvent];
    }
}

- (void)otherMouseDragged:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(otherMouseDragged:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del otherMouseDragged:theEvent];
    }
}

- (void)otherMouseUp:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(otherMouseUp:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del otherMouseUp:theEvent];
    }
}

- (void)mouseMoved:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(mouseMoved:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del mouseMoved:theEvent];
    }
}

- (void)scrollWheel:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(scrollWheel:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del scrollWheel:theEvent];
    }
}

- (void)touchesBeganWithEvent:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(touchesBeganWithEvent:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del touchesBeganWithEvent:theEvent];
    }
}

- (void)touchesCancelledWithEvent:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(touchesCancelledWithEvent:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del touchesCancelledWithEvent:theEvent];
    }
}

- (void)touchesEndedWithEvent:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(touchesEndedWithEvent:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del touchesEndedWithEvent:theEvent];
    }
}

- (void)touchesMovedWithEvent:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(touchesMovedWithEvent:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del touchesMovedWithEvent:theEvent];
    }
}

- (void)keyDown:(NSEvent*)theEvent
{
    if (!theEvent.isARepeat &&
        self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(keyDown:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del keyDown:theEvent];
    }
}

- (void)keyUp:(NSEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(keyUp:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        [del keyUp:theEvent];
    }
}

- (BOOL)prepareForDragOperation:(id<NSDraggingInfo>)sender
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(prepareForDragOperation:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        return [del prepareForDragOperation:sender];
    }
    else
    {
        NSDragOperation sourceDragMask = sender.draggingSourceOperationMask;
        NSPasteboard* pboard = sender.draggingPasteboard;
        if ([self getSupportedDropType:pboard] && (sourceDragMask & NSDragOperationLink))
            return YES;
        
        return NO;
    }
}

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(draggingEntered:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        return [del draggingEntered:sender];
    }
    else
    {
        NSDragOperation sourceDragMask = sender.draggingSourceOperationMask;
        NSPasteboard* pboard = sender.draggingPasteboard;
        if ([self getSupportedDropType:pboard] && (sourceDragMask & NSDragOperationLink))
            return NSDragOperationLink;
        
        return NSDragOperationNone;
    }
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(performDragOperation:)])
    {
        id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
        return [del performDragOperation:sender];
    }
    else
    {
        NSDragOperation sourceDragMask = sender.draggingSourceOperationMask;
        NSPasteboard* pboard = sender.draggingPasteboard;
        NSString* uti = [self getSupportedDropType:pboard];
        if (uti && (sourceDragMask & NSDragOperationLink))
        {
            NSArray* files = [pboard propertyListForType:uti];
            NSMutableArray* supportedFiles = [NSMutableArray arrayWithCapacity:files.count];
            for (NSString* file in files)
            {
                if ([self isSupportedDroppedFile:file])
                    [supportedFiles addObject:file];
            }
            
            if (supportedFiles.count > 0 &&
                self.delegate != nullptr &&
                [self.delegate conformsToProtocol:@protocol(FinjinNSViewDelegate)] &&
                [self.delegate respondsToSelector:@selector(droppedFiles:)])
            {
                id<FinjinNSViewDelegate> del = (id<FinjinNSViewDelegate>)self.delegate;
                [del droppedFiles:supportedFiles];
            }
            
            return YES;
        }
        
        return NO;
    }
}

- (NSString*)getSupportedDropType:(NSPasteboard*)pboard
{
    for (NSString* uti in self.dropTypes)
    {
        for (NSString* pboardType in pboard.types)
        {
            if ([pboardType isEqualToString:uti])
                return uti;
        }
    }
    return nil;
}

- (bool)isSupportedDroppedFile:(NSString*)fileName
{
    NSString* fileNameExt = fileName.pathExtension;
    
    for (NSString* extension in self.supportedFileExtensions)
    {
        if ([fileNameExt isEqualToString:extension])
            return true;
    }
    
    return false;
}

@end
