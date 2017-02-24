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
#import "FinjinUIView.h"
#import <QuartzCore/CATransaction.h>


//Implementation---------------------------------------------------------------
@interface FinjinUIView ()
@end

@implementation FinjinUIView
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

- (id)initWithFrame:(CGRect)rect
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
    
    [self.layer addSublayer:_metalLayer];
    
    self.userInteractionEnabled = YES;
    
#if !TARGET_OS_TV
    self.multipleTouchEnabled = YES;
#endif
}

- (CAMetalLayer*)metalLayer
{
    return _metalLayer;
}

- (void)layoutSubviews
{
    _metalLayer.frame = self.bounds;
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

#if !TARGET_OS_TV
- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinUIViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(touchesBegan:withEvent:)])
    {
        id<FinjinUIViewDelegate> del = (id<FinjinUIViewDelegate>)self.delegate;
        [del touchesBegan:touches withEvent:theEvent];
    }
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinUIViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(touchesCancelled:withEvent:)])
    {
        id<FinjinUIViewDelegate> del = (id<FinjinUIViewDelegate>)self.delegate;
        [del touchesCancelled:touches withEvent:theEvent];
    }
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinUIViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(touchesEnded:withEvent:)])
    {
        id<FinjinUIViewDelegate> del = (id<FinjinUIViewDelegate>)self.delegate;
        [del touchesEnded:touches withEvent:theEvent];
    }
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)theEvent
{
    if (self.delegate != nullptr &&
        [self.delegate conformsToProtocol:@protocol(FinjinUIViewDelegate)] &&
        [self.delegate respondsToSelector:@selector(touchesMoved:withEvent:)])
    {
        id<FinjinUIViewDelegate> del = (id<FinjinUIViewDelegate>)self.delegate;
        [del touchesMoved:touches withEvent:theEvent];
    }
}
#endif

@end
