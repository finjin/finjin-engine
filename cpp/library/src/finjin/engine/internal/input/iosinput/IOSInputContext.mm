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
#include "IOSInputContext.hpp"
#include "IOSInputContext.hpp"
#include "IOSInputSystem.hpp"
#include "InputDeviceSerializer.hpp"
#include "finjin/common/StaticVector.hpp"
#include "OSWindow.hpp"
#include "../gcinput/GCInputSystem.hpp"
#include "../gcinput/GCInputGameController.hpp"
#import <GameController/GameController.h>

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
struct IOSInputContext::Impl : public AllocatedClass, public OSWindowEventListener
{
    Impl(Allocator* allocator, IOSInputSystem* inputSystem);

    void WindowOnPointerMove(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;
    void WindowOnPointerDown(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;
    void WindowOnPointerUp(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons) override;

    IOSInputSystem* inputSystem;

    IOSInputContext::Settings settings;

    AssetClassFileReader inputDevicesAssetReader;
    AssetClassFileReader gcinputDevicesAssetReader;
    AssetClassFileReader inputBindingsAssetReader;

    int gameControllerUpdateCount;
    int nongameControllerUpdateCount;

    StaticVector<GCInputGameController, GameControllerConstants::MAX_GAME_CONTROLLERS> gameControllers;

    InputTouchScreen touchScreen;

    ByteBuffer configFileBuffer;
};


//Implementation----------------------------------------------------------------

//IOSInputContext::Impl
IOSInputContext::Impl::Impl(Allocator* allocator, IOSInputSystem* inputSystem) :
    AllocatedClass(allocator),
    settings(allocator)
{
    this->inputSystem = inputSystem;

    this->gameControllerUpdateCount = 0;
    this->nongameControllerUpdateCount = 0;

    this->gameControllers.maximize();
    for (auto& gameController : this->gameControllers)
        gameController.SetAllocator(allocator);
    this->gameControllers.clear();
}

void IOSInputContext::Impl::WindowOnPointerMove(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
{
    auto pointer = this->touchScreen.GetPointer(pointerType, pointerID);
    if (pointer != nullptr)
    {
        pointer->Update(true, x.ToDipsValue(), y.ToDipsValue());
    }
}

void IOSInputContext::Impl::WindowOnPointerDown(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
{
    auto pointer = this->touchScreen.ConnectPointer(pointerType, pointerID);
    if (pointer != nullptr)
    {
        pointer->Update(true, x.ToDipsValue(), y.ToDipsValue());
    }
}

void IOSInputContext::Impl::WindowOnPointerUp(OSWindow* osWindow, PointerType pointerType, int pointerID, InputCoordinate x, InputCoordinate y, Buttons buttons)
{
    auto pointer = this->touchScreen.GetPointer(pointerType, pointerID);
    if (pointer != nullptr)
    {
        pointer->Update(false, x.ToDipsValue(), y.ToDipsValue());
    }
}

//IOSInputContext
IOSInputContext::IOSInputContext(Allocator* allocator, IOSInputSystem* inputSystem) :
    AllocatedClass(allocator),
    impl(AllocatedClass::New<Impl>(allocator, FINJIN_CALLER_ARGUMENTS, inputSystem))
{
}

IOSInputContext::~IOSInputContext()
{
}

IOSInputContext::Impl* IOSInputContext::GetImpl()
{
    return impl.get();
}

void IOSInputContext::Create(const Settings& settings, Error& error)
{
    assert(settings.osWindow != nullptr);
    assert(settings.assetFileReader != nullptr);

    FINJIN_ERROR_METHOD_START(error);

    FINJIN_ENGINE_CHECK_IMPL_NOT_NULL(impl, error);

    impl->settings = settings;

    if (!impl->configFileBuffer.Create(EngineConstants::DEFAULT_CONFIGURATION_BUFFER_SIZE, GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate config buffer.");
        return;
    }

    impl->inputDevicesAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_DEVICE, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input devices asset reader.");
        return;
    }

    auto gcinputDevicesAssetSelector = impl->settings.initialAssetFileSelector;
    gcinputDevicesAssetSelector.Set(AssetPathComponent::INPUT_API, GCInputSystem::GetSystemInternalName());
    impl->gcinputDevicesAssetReader.Create(*impl->settings.assetFileReader, gcinputDevicesAssetSelector, AssetClass::INPUT_DEVICE, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create GameController input devices asset reader.");
        return;
    }

    impl->inputBindingsAssetReader.Create(*impl->settings.assetFileReader, impl->settings.initialAssetFileSelector, AssetClass::INPUT_BINDINGS, GetAllocator(), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create input bindings asset reader.");
        return;
    }

    //Enumerate the devices-------------------------------------
    impl->touchScreen.SetDisplayName("Touch screen");
    impl->touchScreen.Connect(true);

    impl->gameControllerUpdateCount = 0;
    impl->nongameControllerUpdateCount = 0;
    Update(0);

    //Set up window event listener
    impl->settings.osWindow->AddWindowEventListener(impl.get());
}

void IOSInputContext::Destroy()
{
    //Remove window event listener
    if (impl->settings.osWindow != nullptr)
        impl->settings.osWindow->RemoveWindowEventListener(impl.get());

    //Destroy devices
    for (auto& device : impl->gameControllers)
        device.Destroy();
    impl->gameControllers.clear();

    impl->touchScreen.Reset();
}

void IOSInputContext::GetSelectorComponents(AssetPathSelector& result)
{
}

const IOSInputContext::Settings& IOSInputContext::GetSettings() const
{
    return impl->settings;
}

void IOSInputContext::Update(SimpleTimeDelta elapsedTime, InputDevicePollFlag flags)
{
    if (impl->gameControllerUpdateCount == 0)
    {
        auto controllers = [GCController controllers];
        if (controllers.count > 0)
        {
            GCInputGameController::UpdateGameControllers(impl->gameControllers, true, elapsedTime);
            impl->gameControllerUpdateCount++;
        }
    }
    else if (impl->gameControllerUpdateCount > 0)
    {
        GCInputGameController::UpdateGameControllers(impl->gameControllers, false, elapsedTime);
        impl->gameControllerUpdateCount++;
    }

    for (auto& gameController : impl->gameControllers)
    {
        if (gameController.IsNewConnection())
            ConfigureInputDevice(gameController, gameController.GetProductDescriptor(), impl->gcinputDevicesAssetReader, impl->configFileBuffer);
    }

    impl->nongameControllerUpdateCount++;
}

void IOSInputContext::Execute(InputEvents& events, InputCommands& commands, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);
}

void IOSInputContext::HandleDeviceChanges()
{
    for (auto& device : impl->gameControllers)
        device.ClearChanged();

    impl->touchScreen.ClearChanged();
}

void IOSInputContext::HandleApplicationViewportLostFocus()
{
}

void IOSInputContext::HandleApplicationViewportGainedFocus()
{
}

size_t IOSInputContext::GetDeviceCount(InputDeviceClass deviceClass) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers.size();
        case InputDeviceClass::TOUCH_SCREEN: return 1;
        default: break;
    }

    return 0;
}

const Utf8String& IOSInputContext::GetDeviceProductDescriptor(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].GetProductDescriptor();
        default: break;
    }

    return Utf8String::GetEmpty();
}

const Utf8String& IOSInputContext::GetDeviceInstanceDescriptor(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].GetInstanceDescriptor();
        default: break;
    }

    return Utf8String::GetEmpty();
}

InputDeviceSemantic IOSInputContext::GetDeviceSemantic(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].GetSemantic();
        default: break;
    }

    return InputDeviceSemantic::NONE;
}

bool IOSInputContext::IsDeviceConnected(InputDeviceClass deviceClass, size_t index) const
{
    switch (deviceClass)
    {
        case InputDeviceClass::GAME_CONTROLLER: return impl->gameControllers[index].IsConnected();
        case InputDeviceClass::TOUCH_SCREEN: return impl->touchScreen.IsConnected();
        default: break;
    }

    return false;
}

size_t IOSInputContext::GetGameControllerCount() const
{
    return impl->gameControllers.size();
}

GCInputGameController* IOSInputContext::GetGameController(size_t index)
{
    return &impl->gameControllers[index];
}

size_t IOSInputContext::GetTouchScreenCount() const
{
    return impl->touchScreen.IsConnected() ? 1 : 0;
}

InputTouchScreen* IOSInputContext::GetTouchScreen(size_t index)
{
    return &impl->touchScreen;
}

FileOperationResult IOSInputContext::ReadInputBinding(InputDeviceClass deviceClass, size_t deviceIndex, const AssetReference& configAssetRef, ByteBuffer& fileBuffer)
{
    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_TYPE, InputDeviceClassUtilities::ToString(deviceClass));

    if (deviceClass == InputDeviceClass::GAME_CONTROLLER)
        impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, GCInputSystem::GetSystemInternalName());
    else
        impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_API, Utf8String::GetEmpty());

    impl->inputBindingsAssetReader.GetSelector().Set(AssetPathComponent::INPUT_DEVICE_DESCRIPTOR, GetDeviceProductDescriptor(deviceClass, deviceIndex));

    return impl->inputBindingsAssetReader.ReadAsset(fileBuffer, configAssetRef);
}
