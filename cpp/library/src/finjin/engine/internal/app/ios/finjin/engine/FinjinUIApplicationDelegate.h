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


#import <UIKit/UIKit.h>

namespace Finjin { namespace Common {
    class Allocator;
} }

namespace Finjin { namespace Engine {
    class ApplicationDelegate;
} }

@interface FinjinUIApplicationDelegate : UIResponder<UIApplicationDelegate>

- (Finjin::Common::Allocator*)engineAllocator;

- (void)setEngineApplicationDelegate:(Finjin::Engine::ApplicationDelegate*)del;
- (Finjin::Engine::ApplicationDelegate*)engineApplicationDelegate;

@end
