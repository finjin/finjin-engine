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
#include "FinjinPrecompiled.hpp"
#include "finjin/engine/ApplicationDelegate.hpp"
#include "finjin/engine/Application.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/DefaultAllocator.hpp"
#include "finjin/common/PassthroughSystemAllocator.hpp"
#include <ppltasks.h>

using namespace Finjin::Engine;
using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::System;


//Library references------------------------------------------------------------
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "Kernel32")
#pragma comment(lib, "Shell32")
#pragma comment(lib, "winmm")


//Local types-------------------------------------------------------------------
ref class ApplicationFrameworkView sealed : public IFrameworkView
{
public:
    ApplicationFrameworkView(intptr_t applicationDelegate, Platform::Array<Platform::String^>^ args);

    //IFrameworkView methods
    virtual void Initialize(CoreApplicationView^ applicationView);
    virtual void SetWindow(CoreWindow^ window);
    virtual void Load(Platform::String^ entryPoint);
    virtual void Run();
    virtual void Uninitialize();

protected:
    //Application lifecycle event handlers
    void OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args);
    void OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args);
    void OnResuming(Platform::Object^ sender, Platform::Object^ args);

    //DisplayInformation event handlers
    void OnDpiChanged(DisplayInformation^ sender, Platform::Object^ args);
    void OnOrientationChanged(DisplayInformation^ sender, Platform::Object^ args);
    void OnDisplayContentsInvalidated(DisplayInformation^ sender, Platform::Object^ args);

private:
    Platform::Array<Platform::String^>^ args;

    std::unique_ptr<ApplicationDelegate> applicationDelegate;
    std::unique_ptr<Application> app;
};

ref class ApplicationFrameworkViewSource sealed : IFrameworkViewSource
{
public:
    ApplicationFrameworkViewSource(intptr_t applicationDelegate, Platform::Array<Platform::String^>^ args);
    virtual IFrameworkView^ CreateView();

private:
    intptr_t applicationDelegate;
    Platform::Array<Platform::String^>^ args;
};


//Implementation----------------------------------------------------------------
ApplicationFrameworkView::ApplicationFrameworkView(intptr_t _applicationDelegate, Platform::Array<Platform::String^>^ _args) :
    applicationDelegate(reinterpret_cast<ApplicationDelegate*>(_applicationDelegate)),
    args(_args)
{
}

void ApplicationFrameworkView::Initialize(CoreApplicationView^ applicationView)
{
    //The first method called when the IFrameworkView is being created.

    //Register event handlers for app lifecycle. This example includes Activated, so that we can make the CoreWindow active and start rendering on the window.
    applicationView->Activated += ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &ApplicationFrameworkView::OnActivated);
    CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs^>(this, &ApplicationFrameworkView::OnSuspending);
    CoreApplication::Resuming += ref new EventHandler<Platform::Object^>(this, &ApplicationFrameworkView::OnResuming);
}

void ApplicationFrameworkView::SetWindow(CoreWindow^ window)
{
    auto currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &ApplicationFrameworkView::OnDpiChanged);
    currentDisplayInformation->OrientationChanged += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &ApplicationFrameworkView::OnOrientationChanged);
    DisplayInformation::DisplayContentsInvalidated += ref new TypedEventHandler<DisplayInformation^, Object^>(this, &ApplicationFrameworkView::OnDisplayContentsInvalidated);
}

void ApplicationFrameworkView::Load(Platform::String^ entryPoint)
{
    if (this->app == nullptr)
    {
        ApplicationView::TerminateAppOnFinalViewClose = true; //Ensures the application is terminated when closed

        auto allocator = this->applicationDelegate->GetAllocator();
        this->app.reset(AllocatedClass::New<Application>(allocator, FINJIN_CALLER_ARGUMENTS, this->applicationDelegate.release()));

        //Parse command line
        CommandLineArgsProcessor argsProcessor(allocator);
        argsProcessor.Create(this->args->Length);
        for (unsigned int argIndex = 0; argIndex < this->args->Length; argIndex++)
            argsProcessor[argIndex] = this->args[argIndex]->Data();

        //Run
        FINJIN_DECLARE_ERROR(error);
        this->app->ReadCommandLineSettings(argsProcessor, error);
        if (error)
        {
            FINJIN_DEBUG_LOG_ERROR(error.ToString().c_str());
            return;
        }

        this->app->Initialize(error);
        if (error)
        {
            FINJIN_DEBUG_LOG_ERROR(error.ToString().c_str());
            return;
        }
    }
}

void ApplicationFrameworkView::Run()
{
    //This method is called after the window becomes active.
    FINJIN_DECLARE_ERROR(error);

    for (auto& appViewport : this->app->GetViewportsController())
        appViewport->GetOSWindow()->SetTitle();

    this->app->MainLoop(error);
    if (error)
    {
        this->app->ReportError(error);
    }

    //Shut down
    this->app->Destroy();
    this->app.reset();

    this->applicationDelegate.reset();
}

void ApplicationFrameworkView::Uninitialize()
{
    //Called if IFrameworkView class is torn down while the app is in the foreground
}

void ApplicationFrameworkView::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    //Activate core window so that Run() will run
    CoreWindow::GetForCurrentThread()->Activate();
}

void ApplicationFrameworkView::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
    //Save app state asynchronously after requesting a deferral. Holding a deferral indicates that the application is busy performing suspending operations.
    //Be aware that a deferral may not be held indefinitely. After about five seconds, the app will be forced to exit.

    FINJIN_DECLARE_ERROR(error);
    this->app->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::PAUSE), error);

    SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();
    create_task([deferral]()
    {
        //TODO: Implement more logic here?

        deferral->Complete();
    });
}

void ApplicationFrameworkView::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
    //Restore any data or state that was unloaded on suspend.
    //By default, data and state are persisted when resuming from suspend.
    //Note that this event does not occur if the app was previously terminated.

    FINJIN_DECLARE_ERROR(error);
    this->app->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::RESUME), error);
}

void ApplicationFrameworkView::OnDpiChanged(DisplayInformation^ sender, Object^ args)
{
    //sender->LogicalDpi
    //this->mainAppDelegate->OnWindowSizeChanged();
}

void ApplicationFrameworkView::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
    //this->mainAppDelegate->OnWindowSizeChanged();
}

void ApplicationFrameworkView::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
}

//ApplicationFrameworkViewSource
ApplicationFrameworkViewSource::ApplicationFrameworkViewSource(intptr_t _applicationDelegate, Platform::Array<Platform::String^>^ _args) :
    applicationDelegate(_applicationDelegate),
    args(_args)
{
}

IFrameworkView^ ApplicationFrameworkViewSource::CreateView()
{
    //This logic only allows for a single framework view/window
    if (this->applicationDelegate != 0)
    {
        auto result = ref new ApplicationFrameworkView(this->applicationDelegate, this->args);
        this->applicationDelegate = 0; //Don't use delegate again
        return result;
    }
    else
    {
        assert(0 && "ApplicationFrameworkViewSource::CreateView() can only be called once (to create a single view).");
        return nullptr;
    }
}


//ApplicationFrameworkView------------------------------------------------------
int FinjinMain(ApplicationDelegate* applicationDelegate, Platform::Array<Platform::String^>^ args)
{
    auto mainSource = ref new ApplicationFrameworkViewSource(reinterpret_cast<intptr_t>(applicationDelegate), args);
    CoreApplication::Run(mainSource);
    return 0;
}
