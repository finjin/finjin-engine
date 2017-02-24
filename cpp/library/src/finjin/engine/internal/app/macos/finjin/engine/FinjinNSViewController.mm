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
#import "FinjinNSViewController.h"
#import "FinjinNSView.h"


//Implementation---------------------------------------------------------------
@interface FinjinNSViewController ()
@end

@implementation FinjinNSViewController
{
}

- (void)loadView
{
    auto view = [[FinjinNSView alloc] initWithFrame:CGRectMake(0, 0, self.preferredContentSize.width, self.preferredContentSize.height)];
    view.dropTypes = [NSArray arrayWithObjects:NSFilenamesPboardType, nil];
    view.supportedFileExtensions = [NSArray arrayWithObjects:@"fstd-scene", @"fsbd-scene", @"cfg-scene", @"json-scene", nil];
    [view registerForDraggedTypes:view.dropTypes];
    
    [self setView:view];
}

@end
