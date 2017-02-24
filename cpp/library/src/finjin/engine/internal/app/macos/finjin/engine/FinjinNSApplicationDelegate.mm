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
#import "FinjinNSApplicationDelegate.h"
#include "finjin/common/AppleUtilities.hpp"
#include "finjin/common/CommandLineArgsProcessor.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/DefaultAllocator.hpp"
#include "finjin/common/PassthroughSystemAllocator.hpp"
#include "finjin/engine/ApplicationDelegate.hpp"
#include "finjin/engine/Application.hpp"

using namespace Finjin::Common;
using namespace Finjin::Engine;


//Implementation--------------------------------------------------------------------------
@implementation FinjinNSApplicationDelegate
{
    DefaultAllocator<PassthroughSystemAllocator> _defaultAllocator;

    NSTimer* _tickTimer;
    Finjin::Common::Error _tickError;
    
    std::unique_ptr<ApplicationDelegate> _applicationDelegate;
    std::unique_ptr<Application> _app;
}

- (Finjin::Common::Allocator*)engineAllocator
{
    return _defaultAllocator;
}

- (void)setEngineApplicationDelegate:(Finjin::Engine::ApplicationDelegate*)del
{
    _applicationDelegate.reset(del);
}

- (Finjin::Engine::ApplicationDelegate*)engineApplicationDelegate
{
    if (_applicationDelegate != nullptr)
        return _applicationDelegate.get();
    else if (_app != nullptr)
        return _app->GetDelegate();
    else
        return nullptr;
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
    FINJIN_DECLARE_ERROR(error);
    
    //Create application---------------------------------------------------
    auto allocator = _applicationDelegate->GetAllocator();
    _app = AllocatedClass::NewUnique<Application>(allocator, FINJIN_CALLER_ARGUMENTS, _applicationDelegate.release());
    
    //Run initialization sequence------------------------------------------
    CommandLineArgsProcessor argsProcessor(allocator);
    AppleUtilities::GetCommandLineArgs(argsProcessor);
    _app->Run(argsProcessor, error);
    if (error)
    {
        _app->Destroy();
        
        _app->ReportError(error);
              
        return;
    }
    
    //Start main loop timer------------------------------------------------
    [self startTimer];
}

- (void)applicationDidBecomeActive:(NSNotification*)notification
{
    if (!self.engineApplicationDelegate->GetApplicationSettings().updateWhenNotFocused)
    {
        FINJIN_DECLARE_ERROR(error);
        
        _app->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::MessageType::RESUME), error);
        if (error)
        {
            auto errorString = error.ToString();
            NSLog(@"%s", errorString.c_str());
        }
        else if (!_tickError)
            [self startTimer];
    }
}

- (void)applicationDidResignActive:(NSNotification*)notification
{
    if (!self.engineApplicationDelegate->GetApplicationSettings().updateWhenNotFocused)
    {
        [self stopTimer];
        
        FINJIN_DECLARE_ERROR(error);
        
        @autoreleasepool
        {
            _app->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::MessageType::PAUSE), error);
            if (error)
            {
                auto errorString = error.ToString();
                NSLog(@"%s", errorString.c_str());
            }
        }
    }
}

- (void)applicationWillTerminate:(NSNotification*)aNotification
{
    [self stopTimer];
    
    if (_app != nullptr)
    {
        _app->Destroy();
        _app.reset();
    }
    
    _applicationDelegate.reset();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender
{
    return YES;
}

- (void)startTimer
{
    if (_tickTimer == nullptr)
    {
        _tickTimer = [NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(tick:) userInfo:nil repeats:YES];
        [[NSRunLoop currentRunLoop] addTimer:_tickTimer forMode:NSEventTrackingRunLoopMode];
    }
}

- (void)stopTimer
{
    if (_tickTimer != nullptr)
    {
        [_tickTimer invalidate];
        _tickTimer = nullptr;
    }
}

- (void)tick:(id)sender
{
    if (!_tickError)
    {
        FINJIN_ERROR_METHOD_START(_tickError);
        
        @autoreleasepool
        {
            _app->Tick(_tickError);
        }
        
        if (_tickError)
        {
            FINJIN_DEBUG_LOG_INFO("Failed to tick: %1%", _tickError.ToString());
            
            [self stopTimer];
        }
    }
}

@end
