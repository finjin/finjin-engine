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
#import "FinjinUIViewController.h"
#import "FinjinUIView.h"


//Implementation---------------------------------------------------------------
@interface FinjinUIViewController ()
@end

@implementation FinjinUIViewController
{
}

- (void)loadView
{
    auto size = self.preferredContentSize;
    FinjinUIView* view = [[FinjinUIView alloc] initWithFrame:CGRectMake(0, 0, size.width, size.height)];
            
    [self setView:view];
}

@end
