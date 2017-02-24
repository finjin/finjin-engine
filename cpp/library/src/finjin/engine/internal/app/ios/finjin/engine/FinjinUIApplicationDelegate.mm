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
#import "FinjinUIApplicationDelegate.h"
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
@implementation FinjinUIApplicationDelegate
{
    DefaultAllocator<PassthroughSystemAllocator> _defaultAllocator;

    CADisplayLink* _tickTimer;
    Finjin::Common::Error _tickError;
    
    std::unique_ptr<Finjin::Engine::ApplicationDelegate> _applicationDelegate;
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

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    FINJIN_DECLARE_ERROR(error);
    
    //Create application-----------------------------------------------------
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
            
        return YES;
    }
    
    //Start main loop timer--------------------------------------------------
    [self startTimer];
    
    return YES;
}

- (void)applicationDidBecomeActive:(UIApplication*)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
    
    FINJIN_DECLARE_ERROR(error);
    
    _app->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::MessageType::RESUME), error);
    if (error)
    {
        _app->ReportError(error);
    }
    else if (!_tickError)
        [self startTimer];
}

- (void)applicationWillResignActive:(UIApplication*)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    [self stopTimer];
    
    FINJIN_DECLARE_ERROR(error);
    
    _app->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::MessageType::PAUSE), error);
    if (error)
    {
        auto errorString = error.ToString();
        NSLog(@"%s", errorString.c_str());
    }
}

- (void)applicationWillTerminate:(UIApplication*)application
{
    [self stopTimer];
    
    if (_app != nullptr)
    {
        _app->Destroy();
        _app.reset();
    }
    
    _applicationDelegate.reset();
}

- (void)applicationDidEnterBackground:(UIApplication*)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication*)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    FINJIN_DECLARE_ERROR(error);
    
    _app->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::MessageType::LOW_MEMORY_WARNING), error);
    if (error)
    {
        auto errorString = error.ToString();
        NSLog(@"%s", errorString.c_str());
    }
}

- (void)startTimer
{
    if (_tickTimer == nullptr)
    {
        _tickTimer = [CADisplayLink displayLinkWithTarget:self selector:@selector(tick:)];
        _tickTimer.preferredFramesPerSecond = 0; //Native display refresh
        [_tickTimer addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
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
