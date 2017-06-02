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


#pragma once


//Includes----------------------------------------------------------------------
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/CommandLineArgsProcessor.hpp"
#include "finjin/common/DomainInformation.hpp"
#include "finjin/common/EnumArray.hpp"
#include "finjin/common/GeneralAllocator.hpp"
#include "finjin/common/JobSystem.hpp"
#include "finjin/common/StandardPaths.hpp"
#include "finjin/common/UsableStaticVector.hpp"
#include "finjin/common/UserInformation.hpp"
#include "finjin/common/VirtualFileSystemOperationQueue.hpp"
#include "finjin/engine/Application.hpp"
#include "finjin/engine/ApplicationFileSystem.hpp"
#include "finjin/engine/ApplicationGlobals.hpp"
#include "finjin/engine/ApplicationViewportsController.hpp"
#include "finjin/engine/AssetClass.hpp"
#include "finjin/engine/AssetFileReader.hpp"
#include "finjin/engine/AssetReadQueue.hpp"
#include "finjin/engine/GpuSystem.hpp"
#include "finjin/engine/InputSystem.hpp"
#include "finjin/engine/MemorySettings.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "finjin/engine/OSWindowEventListener.hpp"
#include "finjin/engine/PlatformCapabilities.hpp"
#include "finjin/engine/SoundSystem.hpp"
#include "finjin/engine/StringTable.hpp"
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    #include "finjin/engine/VRSystem.hpp"
    #include "GenericVRInputDevice.hpp"
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class ApplicationDelegate;
    class ApplicationViewportDescription;

    class ApplicationSystemMessage
    {
    public:
        enum MessageType
        {
            SAVE_STATE,
            PAUSE,
            RESUME,
            LOW_MEMORY_WARNING
        };

        ApplicationSystemMessage(MessageType messageType)
        {
            this->messageType = messageType;
        }

        MessageType GetMessageType() const
        {
            return this->messageType;
        }

    private:
        MessageType messageType;
    };

    class Application :
        public AllocatedClass,
        public OSWindowEventListener,
        public PlatformCapabilities
    {
    public:
        Application(Allocator* allocator, ApplicationDelegate* applicationDelegate, void* applicationHandle = nullptr);
        ~Application();

        ApplicationDelegate* GetDelegate();

        StandardPaths& GetStandardPaths();
        UserInformation& GetUserInfo();
        DomainInformation& GetDomainInformation();
        ApplicationGlobals& GetGlobals();
        VirtualFileSystem& GetFileSystem(ApplicationFileSystem which);
        JobSystem& GetJobSystem();
        InputSystem& GetInputSystem();
        ApplicationViewportsController& GetViewportsController();

        bool Run(Error& error);
        bool Run(CommandLineArgsProcessor& argsProcessor, Error& error);

        ReadCommandLineResult ReadCommandLineSettings(CommandLineArgsProcessor& argsProcessor, Error& error);

        void OnSystemMessage(const ApplicationSystemMessage& message, Error& error);
        void Tick(Error& error);

        void Create(Error& error);
        void Destroy();

        bool MainLoop(Error& error); //Platform-specific

        void ReportError(const Error& error); //Platform-specific

    protected:
        //OSWindowEventListener callbacks
        void WindowMoved(OSWindow* osWindow) override;
        void WindowResized(OSWindow* osWindow) override;
        void WindowGainedFocus(OSWindow* osWindow) override;
        void WindowLostFocus(OSWindow* osWindow) override;
        void WindowClosing(OSWindow* osWindow) override;
        bool WindowOnDropFiles(OSWindow* osWindow, const Path* fileNames, size_t count) override;

        void InitializeGlobals(Error& error); //Platform-specific
        void CreateSystems(Error& error); //Platform-specific
        void ShowTheCursor(); //Platform-specific
        void HideTheCursor(); //Platform-specific

    public:
        void GetConfiguredApplicationViewportSize(WindowSize& windowSize, const Setting<OSWindowRect>& windowFrame);

        void HandleApplicationViewportLostFocus(ApplicationViewport* appViewport);
        void HandleApplicationViewportGainedFocus(ApplicationViewport* appViewport);

        void HandleApplicationViewportsCreated(Error& error);
        void HandleApplicationViewportCreated(ApplicationViewport* appViewport, const ApplicationViewportDescription& windowDescription, Error& error);

        void HandleApplicationViewportEndOfLife(ApplicationViewport* appViewport, bool isPermanent);

        ApplicationViewport* OSWindowToApplicationViewport(OSWindow* osWindow);

    private:
        void ReadBootFile(Error& error);
        size_t GetBytesUsed();

    private:
        void* applicationHandle; //Platform-specific pointer to the application

        size_t estimatedBytesFreeAtInitialization; //Might be MemorySize::UNKNOWN_SIZE

        MemorySettings memorySettings;
        GeneralAllocator applicationAllocator; //Top level application memory

        //Logger logger

        std::unique_ptr<ApplicationDelegate> applicationDelegate;

        ByteBuffer configFileBuffer;

        UserInformation userInfo;
        DomainInformation domainInfo;

        StandardPaths standardPaths;
        Path workingPath;
        Path workingPath2;
        EnumArray<ApplicationFileSystem, ApplicationFileSystem::COUNT, VirtualFileSystem> fileSystems;
        VirtualFileSystemOperationQueue::Settings fileSystemOperationQueueSettings;
        VirtualFileSystemOperationQueue fileSystemOperationQueue;
        HighResolutionClock fileSystemOperationQueueUpdateClock;
        HighResolutionTimeStamp lastOperationQueueUpdate;

        AssetReference workingAssetRef;
        AssetReadQueue::Settings assetReadQueueSettings;
        AssetReadQueue assetReadQueue;

        AssetFileReader assetFileReader;
        AssetPathSelector assetFileSelector;
        EnumArray<AssetClass, AssetClass::COUNT, AssetClassFileReader> assetClassFileReaders;

        AssetReferences<StringTable::MAX_INSTANCES> defaultStringsAssetFileNames;
        StringTable defaultStrings;

        ApplicationGlobals applicationGlobals;

        InputContext::Settings inputContextSettings;
        InputSystem::Settings inputSystemSettings;
        InputSystem inputSystem;

        GpuContext::Settings gpuContextSettings;
        GpuSystem::Settings gpuSystemSettings;
        GpuSystem gpuSystem;

        SoundContext::Settings soundContextSettings;
        SoundSystem::Settings soundSystemSettings;
        SoundSystem soundSystem;

    #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
        VRContext::Settings vrContextSettings;
        VRSystem::Settings vrSystemSettings;
        VRSystem vrSystem;
        UsableStaticVector<VRGameControllerForInputSystem, GameControllerConstants::MAX_GAME_CONTROLLERS> vrGameControllersForInputSystem;
        UsableStaticVector<VRHeadsetForInputSystem, HeadsetConstants::MAX_HEADSETS> vrHeadsetsForInputSystem;
    #endif

        //PhysicsContext::Settings physicsContextSettings;
        //PhysicsSystem::Settings physicsSystemSettings;
        //PhysicsSystem physicsSystem;

        //PathfindingContext::Settings pathfindingContextSettings;
        //PathfindingSystem::Settings pathfindingSystemSettings;
        //PathfindingSystem pathfindingSystem;

        ApplicationViewportsController appViewportsController;
        Utf8String workingWindowInternalName;

        Utf8String mainThreadName;
        LogicalCpus logicalCpus;
        LogicalCpu mainThreadLogicalCpu;

        JobSystem::Settings jobSystemSettings;
        JobSystem jobSystem;
    };

} }
