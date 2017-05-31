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
#include "Application.hpp"
#include "finjin/common/ByteOrder.hpp"
#include "finjin/common/CommandLineArgsProcessor.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/MemorySize.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/ThisThread.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "ApplicationDelegate.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_D3D12 && FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_WINDOWS_WIN32
    #define TOGGLE_FULLSCREEN_EXCLUSIVE_ON_LOST_FOCUS 1 //Sometimes helpful to set this to 0 during multi-screen development on Windows
#else
    #define TOGGLE_FULLSCREEN_EXCLUSIVE_ON_LOST_FOCUS 0
#endif

#define ALLOW_USER_HOME_DIRECTORY_EXPANSION 1

#define FILE_SYSTEM_DATABASE_ENTRY_TYPES FileSystemEntryType::DIRECTORY


//Local types-------------------------------------------------------------------
class WindowedViewportUpdateContext : public ApplicationViewportUpdateContext
{
public:
    WindowedViewportUpdateContext(Allocator* allocator, ApplicationViewport* appViewport);

    bool IsMain() const override;
    bool HasFocus() const override;

    void RequestFullScreenToggle() override;
    void RequestClose() override;
    void RequestApplicationExit() override;

private:
    ApplicationViewport* appViewport;
};

class WindowedViewportRenderContext : public ApplicationViewportRenderContext
{
public:
    WindowedViewportRenderContext(Allocator* allocator, ApplicationViewport* appViewport);

    bool IsMain() const override;
    bool HasFocus() const override;

private:
    ApplicationViewport* appViewport;
};


//Implementation----------------------------------------------------------------

//Memory allocators
void* operator new(size_t byteCount) FINJIN_GLOBAL_NEW_THROW
{
    return Allocator::SystemAllocate(byteCount, FINJIN_CALLER_ARGUMENTS);
}

void operator delete(void* mem) noexcept
{
    Allocator::SystemDeallocate(mem);
}

void* operator new[](size_t byteCount) FINJIN_GLOBAL_NEW_THROW
{
    return Allocator::SystemAllocate(byteCount, FINJIN_CALLER_ARGUMENTS);
}

void operator delete[](void* mem) noexcept
{
    Allocator::SystemDeallocate(mem);
}

//WindowedViewportUpdateContext
WindowedViewportUpdateContext::WindowedViewportUpdateContext(Allocator* allocator, ApplicationViewport* appViewport) : ApplicationViewportUpdateContext(allocator)
{
    this->appViewport = appViewport;

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->vrContext = appViewport->GetVRContext();
#endif
    this->inputContext = appViewport->GetInputContext();
    this->soundContext = appViewport->GetSoundContext();
    this->gpuContext = appViewport->GetGpuContext();
}

bool WindowedViewportUpdateContext::IsMain() const
{
    assert(this->appViewport != nullptr);
    return this->appViewport->IsMain();
}

bool WindowedViewportUpdateContext::HasFocus() const
{
    assert(this->appViewport != nullptr);
    return this->appViewport->HasFocus();
}

void WindowedViewportUpdateContext::RequestFullScreenToggle()
{
    assert(this->appViewport != nullptr);
    return this->appViewport->RequestFullScreenToggle();
}

void WindowedViewportUpdateContext::RequestClose()
{
    assert(this->appViewport != nullptr);
    return this->appViewport->RequestClose();
}

void WindowedViewportUpdateContext::RequestApplicationExit()
{
    assert(this->appViewport != nullptr);
    this->appViewport->RequestApplicationExit();
}

//WindowedViewportRenderContext
WindowedViewportRenderContext::WindowedViewportRenderContext(Allocator* allocator, ApplicationViewport* appViewport) : ApplicationViewportRenderContext(allocator)
{
    this->appViewport = appViewport;

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->vrContext = appViewport->GetVRContext();
#endif
    this->inputContext = appViewport->GetInputContext();
    this->soundContext = appViewport->GetSoundContext();
    this->gpuContext = appViewport->GetGpuContext();
}

bool WindowedViewportRenderContext::IsMain() const
{
    assert(this->appViewport != nullptr);
    return this->appViewport->IsMain();
}

bool WindowedViewportRenderContext::HasFocus() const
{
    assert(this->appViewport != nullptr);
    return this->appViewport->HasFocus();
}

//Application
Application::Application(Allocator* allocator, ApplicationDelegate* applicationDelegate, void* applicationHandle) :
    AllocatedClass(allocator),
    memorySettings(allocator),
    standardPaths(allocator),
    workingPath(allocator),
    workingPath2(allocator),
    workingAssetRef(allocator),
    defaultStringsAssetFileNames(allocator),
    inputSystemSettings(allocator),
    inputContextSettings(allocator),
    gpuContextSettings(allocator),
    gpuSystemSettings(allocator),
    soundSystemSettings(allocator),
    soundContextSettings(allocator)
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    ,
    vrSystemSettings(allocator),
    vrContextSettings(allocator)
#endif
{
    assert(applicationDelegate != nullptr);

    this->applicationHandle = applicationHandle;

    this->estimatedBytesFreeAtInitialization = 0;

    this->applicationDelegate.reset(applicationDelegate);
}

Application::~Application()
{
}

ApplicationDelegate* Application::GetDelegate()
{
    return this->applicationDelegate.get();
}

StandardPaths& Application::GetStandardPaths()
{
    return this->standardPaths;
}

UserInformation& Application::GetUserInfo()
{
    return this->userInfo;
}

DomainInformation& Application::GetDomainInformation()
{
    return this->domainInfo;
}

ApplicationGlobals& Application::GetGlobals()
{
    return this->applicationGlobals;
}

VirtualFileSystem& Application::GetFileSystem(ApplicationFileSystem which)
{
    return this->fileSystems[which];
}

JobSystem& Application::GetJobSystem()
{
    return this->jobSystem;
}

InputSystem& Application::GetInputSystem()
{
    return this->inputSystem;
}

ApplicationViewportsController& Application::GetViewportsController()
{
    return this->appViewportsController;
}

bool Application::Run(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    CommandLineArgsProcessor argsProcessor(GetAllocator());
    auto result = Run(argsProcessor, error);
    if (error)
        FINJIN_SET_ERROR(error, "There was an error while running the application.");

    return result;
}

bool Application::Run(CommandLineArgsProcessor& argsProcessor, Error& error)
{
    assert(this->applicationDelegate != nullptr);

    FINJIN_ERROR_METHOD_START(error);

    //Command line settings----------------------------------------------------
    auto commandLineResult = ReadCommandLineSettings(argsProcessor, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read command line settings.");
        return false;
    }
    if (commandLineResult == ReadCommandLineResult::SHOW_USAGE)
    {
        Utf8String usage;
        this->applicationDelegate->GetCommandLineUsage(usage);
        if (!usage.empty())
            std::cout << usage << std::endl;
        return true;
    }

    //Initialize--------------------------------------------------
    Create(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize application.");
        return false;
    }

    //Main loop--------------------------------------------------
    MainLoop(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "An error occurred while running the application.");
        return false;
    }

    return true;
}

ReadCommandLineResult Application::ReadCommandLineSettings(CommandLineArgsProcessor& argsProcessor, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = this->applicationDelegate->ReadCommandLineSettings(argsProcessor, error);
    if (error)
        FINJIN_SET_ERROR(error, "Failed to read command line settings");

    return result;
}

void Application::OnSystemMessage(const ApplicationSystemMessage& message, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    switch (message.GetMessageType())
    {
        case ApplicationSystemMessage::SAVE_STATE:
        {
            break;
        }
        case ApplicationSystemMessage::PAUSE:
        {
            for (auto& appViewport : this->appViewportsController)
                appViewport->FinishWork(RenderStatus::GetRenderingRequired());
            this->jobSystem.Stop();
            break;
        }
        case ApplicationSystemMessage::RESUME:
        {
            this->jobSystem.Start(true, error);
            if (error)
                FINJIN_SET_ERROR(error, "Failed to start job system during resume.");
            break;
        }
        case ApplicationSystemMessage::LOW_MEMORY_WARNING:
        {
            break;
        }
    }
}

void Application::Tick(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (!this->appViewportsController.empty())
    {
        //Update file operation queue
        VirtualFileSystemOperationQueue::UpdateSettings updateSettings;
        updateSettings.minByteCountPerRead = this->assetReadQueue.GetSettings().streamingMaxBytesPerLine;

        auto now = this->fileSystemOperationQueueUpdateClock.Now();
        if (!this->lastOperationQueueUpdate.IsZero())
        {
            auto elapsedTime = now - this->lastOperationQueueUpdate;
            if (elapsedTime < TimeDuration::Milliseconds(100))
                updateSettings.elapsedTime = elapsedTime.ToSimpleTimeDelta();
        }
        this->lastOperationQueueUpdate = now;

        this->fileSystemOperationQueue.Update(updateSettings, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to update file system operation queue.");
            return;
        }

        //Update asset read queue
        this->assetReadQueue.Update(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to update asset read queue.");
            return;
        }

        //Update each viewport
        if (GetDelegate()->OnTickApplicationViewports(this->appViewportsController, error))
        {
            if (error)
            {
                FINJIN_SET_ERROR(error, "Application delegate failed to update application viewports.");
                return;
            }
        }
        else
        {
            for (auto& appViewport : this->appViewportsController)
            {
                appViewport->OnTick(jobSystem, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to update application viewport.");
                    return;
                }
            }
        }

        //Handle viewports that are in the process of being closed
        ApplicationViewportsClosing closingAppViewports;
        this->appViewportsController.GetViewportsClosing(closingAppViewports);
        for (auto& appViewport : closingAppViewports)
        {
            HandleApplicationViewportEndOfLife(appViewport.get(), true);
            appViewport.reset();
        }
    }
}

void Application::Create(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Preliminary initialization-------------------------------------------------------------

    this->estimatedBytesFreeAtInitialization = GetAllocator()->GetBytesFree();
    //auto bytesFree = MemorySize::ToString(this->estimatedBytesFreeAtInitialization);
    //auto bytesUsed = MemorySize::ToString(GetAllocator()->GetBytesUsed());

    this->configFileBuffer.Create(EngineConstants::DEFAULT_CONFIGURATION_BUFFER_SIZE, GetAllocator());

    {
        VirtualFileSystem::Settings fileSystemSettings;
        fileSystemSettings.allocator = GetAllocator();
        fileSystemSettings.searchEntryTypes = FILE_SYSTEM_DATABASE_ENTRY_TYPES;

        for (size_t applicationFileSystemIndex = 0; applicationFileSystemIndex < (size_t)ApplicationFileSystem::COUNT; applicationFileSystemIndex++)
        {
            auto& fileSystem = this->fileSystems[applicationFileSystemIndex];
            auto applicationFileSystem = static_cast<ApplicationFileSystem>(applicationFileSystemIndex);

            fileSystemSettings.maxEntries = this->applicationDelegate->GetMaxFileSystemEntries(applicationFileSystem, fileSystemSettings.searchEntryTypes);
            if (fileSystemSettings.maxEntries > 0)
            {
                fileSystem.Create(fileSystemSettings, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to initialize virtual file system '%1%'.", ApplicationFileSystemUtilities::ToString(applicationFileSystem)));
                    return;
                }
            }
        }
    }

    //Application standard paths
    {
        auto& appDirectoryName = this->applicationDelegate->GetName(this->directoryApplicationNameFormat);

        this->standardPaths.Create(appDirectoryName, this->applicationHandle, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create application standard paths.");
            return;
        }
    }

    //Platform-specific globals, including asset file system
    InitializeGlobals(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize application globals.");
        return;
    }

    FINJIN_DEBUG_LOG_INFO("Memory usage after global initialization: %1%", MemorySize::ToString(GetBytesUsed()));

    //Read and process boot files
    if (this->applicationDelegate->GetBootFileNameCount() > 0)
    {
        for (size_t fileIndex = 0; fileIndex < this->applicationDelegate->GetBootFileNameCount(); fileIndex++)
        {
            auto required = false;
            auto& bootFileName = this->applicationDelegate->GetBootFileName(fileIndex, required);

            //Boot files are read from the assets file system
            auto result = this->fileSystems[ApplicationFileSystem::READ_APPLICATION_ASSETS].Read(bootFileName, this->configFileBuffer);
            if (result == FileOperationResult::NOT_FOUND && !required)
            {
                //Not found and not required. Do nothing
            }
            else if (result != FileOperationResult::SUCCESS)
            {
                //Something went wrong
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read boot file '%1%'.", bootFileName));
                return;
            }
            else
            {
                ReadBootFile(error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to process boot file '%1%'.", bootFileName));
                    return;
                }
            }
        }

        //Update asset file system database
        this->fileSystems[ApplicationFileSystem::READ_APPLICATION_ASSETS].UpdateDatabase(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to update file system database.");
            return;
        }
    }

    FINJIN_DEBUG_LOG_INFO("Memory usage after reading boot file: %1%", MemorySize::ToString(GetBytesUsed()));

    //Set up file systems----------------------------------------------------------------------------

    //Set up user data file system
    this->standardPaths.ForEachUserPath([&](const StandardPath& standardPath, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        standardPath.CreateDirectories();

        this->fileSystems[ApplicationFileSystem::READ_USER_DATA].AddDirectory(standardPath.path, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' standard path at '%2%' for use as a user read file system path.", standardPath.GetDisplayName(), standardPath.path));
            return false;
        }

        return true;
    }, error);
    if (error)
    {
        //The error was set while adding user paths
        FINJIN_SET_ERROR(error, "Failed to add all standard user paths to virtual file system.");
        return;
    }

    if (!this->standardPaths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY].path.empty())
    {
        auto& standardPath = this->standardPaths[WhichStandardPath::USER_APPLICATION_SETTINGS_DIRECTORY];

        standardPath.CreateDirectories();

        this->fileSystems[ApplicationFileSystem::READ_WRITE_USER_APPLICATION_DATA].AddDirectory(standardPath.path, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' standard path at '%2%' for use as a user read/write application settings path.", standardPath.GetDisplayName(), standardPath.path));
            return;
        }
    }

    //Handle temp directory
    if (!this->standardPaths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY].path.empty())
    {
        auto& standardPath = this->standardPaths[WhichStandardPath::USER_APPLICATION_TEMPORARY_DIRECTORY];

        standardPath.CreateDirectories();

        //Add to read user/application cache file system
        this->fileSystems[ApplicationFileSystem::READ_WRITE_USER_APPLICATION_CACHE_DATA].AddDirectory(standardPath.path, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' standard path at '%2%' for use as a temporary/cache read/write file system path.", standardPath.GetDisplayName(), standardPath.path));
            return;
        }
    }

    //Handle application settings directory
    if (!this->standardPaths[WhichStandardPath::APPLICATION_SETTINGS_DIRECTORY].path.empty())
    {
        auto& standardPath = this->standardPaths[WhichStandardPath::APPLICATION_SETTINGS_DIRECTORY];

        standardPath.CreateDirectories();

        //Add to asset file system
        this->fileSystems[ApplicationFileSystem::READ_APPLICATION_ASSETS].AddDirectory(standardPath.path, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' standard path at '%2%' for use as a read settings path.", standardPath.GetDisplayName(), standardPath.path));
            return;
        }

        //Add to application data file system
        this->fileSystems[ApplicationFileSystem::READ_WRITE_APPLICATION_DATA].AddDirectory(standardPath.path, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' standard path at '%2%' for use as a read/write application settings path.", standardPath.GetDisplayName(), standardPath.path));
            return;
        }
    }
    if (!this->applicationDelegate->GetApplicationSettings().additionalReadApplicationAssetsDirectory.value.empty())
    {
        this->fileSystems[ApplicationFileSystem::READ_WRITE_APPLICATION_DATA].AddDirectory(this->applicationDelegate->GetApplicationSettings().additionalReadApplicationAssetsDirectory.value, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add path at '%1%' for use as an additional read settings path.", this->applicationDelegate->GetApplicationSettings().additionalReadApplicationAssetsDirectory.value));
            return;
        }
    }

    //Build asset file system database
    this->fileSystems[ApplicationFileSystem::READ_APPLICATION_ASSETS].UpdateDatabase(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to build file system database.");
        return;
    }

    FINJIN_DEBUG_LOG_INFO("Memory usage after updating file database: %1%", MemorySize::ToString(GetBytesUsed()));

    //Set up asset readers------------------------------------------------------------------

    //Preliminary initialization of asset reader
    if (!this->assetFileReader.Create(GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create asset file reader.");
        return;
    }
    this->assetFileReader.SetFileSystemOperationQueue(&this->fileSystemOperationQueue);
    this->assetFileReader.AddFileSystem(&this->fileSystems[ApplicationFileSystem::READ_APPLICATION_ASSETS], error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add 'read application assets' file system to asset file reader.");
        return;
    }

    //Finish initializing asset reader
    if (this->assetFileReader.GetRootDirectoryNameCount() == 0)
    {
        for (auto rootName : { "app" , "engine" })
        {
            this->assetFileReader.AddRootDirectoryName(rootName, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' root directory name.", rootName));
                return;
            }
        }
    }

    //Initialize asset selector
    if (!this->assetFileSelector.Create(GetAllocator()))
    {
        FINJIN_SET_ERROR(error, "Failed to create asset file selector.");
        return;
    }
    this->assetFileSelector.Set(AssetPathComponent::LANGUAGE, this->lang);
    this->assetFileSelector.Set(AssetPathComponent::COUNTRY, this->country);
    this->assetFileSelector.Set(AssetPathComponent::LAYOUT_DIRECTION, this->layoutDirection);
    this->assetFileSelector.SetCpuArchitecture();
    this->assetFileSelector.SetCpuByteOrder();
    this->assetFileSelector.Set(AssetPathComponent::OPERATING_SYSTEM, this->operatingSystemInternalName);
    this->assetFileSelector.Set(AssetPathComponent::OPERATING_SYSTEM_VERSION, this->operatingSystemInternalVersion);
    this->assetFileSelector.Set(AssetPathComponent::APPLICATION_PLATFORM, this->applicationPlatformInternalName);
    this->assetFileSelector.Set(AssetPathComponent::DEVICE_MODEL, this->deviceModel);
    this->assetFileSelector.Set(AssetPathComponent::GPU_SYSTEM, GpuSystem::GetSystemInternalName());
    this->assetFileSelector.Set(AssetPathComponent::SOUND_SYSTEM, SoundSystem::GetSystemInternalName());
    this->assetFileSelector.Set(AssetPathComponent::INPUT_SYSTEM, InputSystem::GetSystemInternalName());
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->assetFileSelector.Set(AssetPathComponent::VR_SYSTEM, VRSystem::GetSystemInternalName());
#endif

    for (size_t assetClass = 0; assetClass < (size_t)AssetClass::COUNT; assetClass++)
    {
        this->assetClassFileReaders[assetClass].Create(this->assetFileReader, this->assetFileSelector, static_cast<AssetClass>(assetClass), GetAllocator(), error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create asset class reader for asset class '%1%'.", AssetClassUtilities::ToString(assetClass)));
            return;
        }
    }

    FINJIN_DEBUG_LOG_INFO("Memory usage after asset class file readers: %1%", MemorySize::ToString(GetBytesUsed()));

    //Memory settings/allocator--------------------------------------------

    //Read settings
    if (this->workingAssetRef.ForLocalFile("memory.cfg").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign 'memory.cfg' to working asset reference.");
        return;
    }
    this->assetClassFileReaders[AssetClass::SETTINGS].ReadAndParseSettingsFile(this->memorySettings, this->configFileBuffer, this->workingAssetRef, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //Allocator
    MemoryArenaSettings applicationArenaSettings;
    auto foundApplicationArenaSettings = this->memorySettings.GetArena(this->memoryArchitecture, MemoryLocation::SYSTEM_DEDICATED, this->estimatedBytesFreeAtInitialization, "application");
    if (foundApplicationArenaSettings != nullptr)
        applicationArenaSettings.byteCount = foundApplicationArenaSettings->byteCount;
    else
        applicationArenaSettings.byteCount = MemorySize::MEBIBYTE * 250;
    {
        auto applicationSystemMemoryFree = GetAllocator()->GetBytesFree();
        if (this->applicationDelegate->GetApplicationSettings().checkSystemMemoryFree.value &&
            applicationArenaSettings.byteCount > applicationSystemMemoryFree)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate %1% from system memory. Only %2% is available. Try closing some applications.", MemorySize::ToString(applicationArenaSettings.byteCount), MemorySize::ToString(applicationSystemMemoryFree)));
            return;
        }
    }
    auto applicationArena = GetAllocator()->AllocateArena(applicationArenaSettings, FINJIN_CALLER_ARGUMENTS);
    if (applicationArena.IsNull())
    {
        FINJIN_SET_ERROR(error, "Failed to allocate application memory arena.");
        return;
    }
    this->applicationAllocator.Create(GeneralAllocator::Settings(), std::move(applicationArena), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create application memory allocator.");
        return;
    }
    if (this->applicationAllocator.SetName("application").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to set application allocator name.");
        return;
    }

    FINJIN_DEBUG_LOG_INFO("Memory usage after initializing application allocator: %1%", MemorySize::ToString(GetBytesUsed()));

    //Read various settings-------------------------------------------------------------------------

    //Read default strings asset
    if (this->defaultStringsAssetFileNames.empty())
    {
        this->defaultStringsAssetFileNames.AddLocalFile("default.cfg", error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add 'default.cfg' default strings asset file.");
            return;
        }
    }
    this->defaultStrings.SetAllocator(&this->applicationAllocator);
    this->assetClassFileReaders[AssetClass::STRING_TABLE].ReadAndParseSettingsFiles(this->defaultStrings, this->configFileBuffer, this->defaultStringsAssetFileNames, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to look up default string resources.");
        return;
    }

    //File operaton queue settings
    if (this->workingAssetRef.ForLocalFile("file-system-operations-queue.cfg").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign 'file-system-operations-queue.cfg' to working asset reference.");
        return;
    }
    this->fileSystemOperationQueueSettings.allocator = &this->applicationAllocator;
    this->assetClassFileReaders[AssetClass::SETTINGS].ReadAndParseSettingsFile(this->fileSystemOperationQueueSettings, this->configFileBuffer, this->workingAssetRef, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //Asset read queue settings
    if (this->workingAssetRef.ForLocalFile("asset-read-queue.cfg").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign 'asset-read-queue.cfg' to working asset reference.");
        return;
    }
    this->assetReadQueueSettings.allocator = &this->applicationAllocator;
    this->assetClassFileReaders[AssetClass::SETTINGS].ReadAndParseSettingsFile(this->assetReadQueueSettings, this->configFileBuffer, this->workingAssetRef, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //Job system settings
    if (this->workingAssetRef.ForLocalFile("job-system.cfg").HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign 'job-system.cfg' to working asset reference.");
        return;
    }
    this->jobSystemSettings.allocator = &this->applicationAllocator;
    this->assetClassFileReaders[AssetClass::SETTINGS].ReadAndParseSettingsFile(this->jobSystemSettings, this->configFileBuffer, this->workingAssetRef, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //Generic application settings
    for (size_t fileIndex = 0; fileIndex < this->applicationDelegate->GetSettingsFileNameCount(); fileIndex++)
    {
        auto required = false;
        auto& settingsFileName = this->applicationDelegate->GetSettingsFileName(fileIndex, required);

        auto result = this->assetClassFileReaders[AssetClass::SETTINGS].ReadAsset(this->configFileBuffer, settingsFileName);
        if (result == FileOperationResult::SUCCESS)
        {
            this->applicationDelegate->ReadSettings(fileIndex, this->configFileBuffer, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to process '%1%' settings file.", settingsFileName.ToUriString()));
                return;
            }
        }
        else if (result == FileOperationResult::NOT_FOUND && !required)
        {
            //Not found and not required. Do nothing
        }
        else if (result != FileOperationResult::NOT_FOUND)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read '%1%' settings file.", settingsFileName.ToUriString()));
            return;
        }
    }

    //Input context settings
    {
        this->inputContextSettings.applicationHandle = this->applicationHandle;
        this->inputContextSettings.assetFileReader = &this->assetFileReader;
        this->inputContextSettings.initialAssetFileSelector = this->assetFileSelector;
        if (this->inputContextSettings.settingsFileNames.empty())
        {
            for (auto fileName : { "input-context.cfg" })
            {
                this->inputContextSettings.settingsFileNames.AddLocalFile(fileName, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' input context settings file.", fileName));
                    return;
                }
            }
        }
    }

    //Sound context settings
    {
        this->soundContextSettings.applicationHandle = this->applicationHandle;
        this->soundContextSettings.assetFileReader = &this->assetFileReader;
        this->soundContextSettings.initialAssetFileSelector = this->assetFileSelector;
        if (this->soundContextSettings.settingsFileNames.empty())
        {
            for (auto fileName : { "sound-context.cfg" })
            {
                this->soundContextSettings.settingsFileNames.AddLocalFile(fileName, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' sound context settings file.", fileName));
                    return;
                }
            }
        }
    }

    //GPU context settings
    {
        this->gpuContextSettings.applicationHandle = this->applicationHandle;
        this->gpuContextSettings.assetFileReader = &this->assetFileReader;
        this->gpuContextSettings.initialAssetFileSelector = this->assetFileSelector;
        if (this->gpuContextSettings.contextSettingsFileNames.empty())
        {
            for (auto fileName : { "gpu-context.cfg", "gpu-context-input-formats.cfg" })
            {
                this->gpuContextSettings.contextSettingsFileNames.AddLocalFile(fileName, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' GPU context settings file.", fileName));
                    return;
                }
            }
        }
        if (this->gpuContextSettings.staticMeshRendererSettingsFileNames.empty())
        {
            for (auto fileName : { "gpu-context-static-mesh-renderer-buffer-formats.cfg" })
            {
                this->gpuContextSettings.staticMeshRendererSettingsFileNames.AddLocalFile(fileName, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' GPU context static mesh renderer settings file.", fileName));
                    return;
                }
            }
        }
        this->gpuContextSettings.presentSyncInterval = this->applicationDelegate->GetApplicationSettings().vsync ? 1 : 0;
    }

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    //VR context settings
    {
        this->vrContextSettings.assetReader = &this->assetFileReader;
        this->vrContextSettings.initialAssetSelector = this->assetFileSelector;
        if (this->vrContextSettings.settingsFileNames.empty())
        {
            for (auto fileName : { "vr-context.cfg" })
            {
                this->vrContextSettings.settingsFileNames.AddLocalFile(fileName, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' VR context settings file.", fileName));
                    return;
                }
            }
        }
    }
#endif

    FINJIN_DEBUG_LOG_INFO("Memory usage after reading settings: %1%", MemorySize::ToString(GetBytesUsed()));

    //Initialize various systems------------------------------------------------------------

    this->fileSystemOperationQueueSettings.allocator = &this->applicationAllocator;
    this->fileSystemOperationQueue.Create(this->fileSystemOperationQueueSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize file system operation queue.");
        return;
    }

    this->assetReadQueueSettings.allocator = &this->applicationAllocator;
    this->assetReadQueue.Create(assetReadQueueSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize asset operation queue.");
        return;
    }

    //Threading
    {
        //Set main (this) thread name
        this->mainThreadName.Create(&this->applicationAllocator);
        if (this->mainThreadName.assign(this->applicationDelegate->GetName(ApplicationNameFormat::LOWER_CASE)).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign camel case name to working main thread name.");
            return;
        }
        if (this->mainThreadName.append("-main-thread").HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to append '-main-thread' to working main thread name.");
            return;
        }
        ThisThread::SetName(this->mainThreadName);

        //Detect CPUs and assign main (this) thread to a suitable core
        this->logicalCpus.Enumerate();
        if (this->logicalCpus.size() < 2)
        {
            FINJIN_SET_ERROR(error, "Unsupported CPU. Must have at least 2 cores.");
            return;
        }
        this->logicalCpus.AssociateCurrentThreadAndRemove(&this->mainThreadLogicalCpu);
        //this->logicalCpus.Truncate(1); //For testing
        //this->logicalCpus.Output(std::cout); //For testing
    }

    FINJIN_DEBUG_LOG_INFO("Memory usage after asset read queue: %1%", MemorySize::ToString(GetBytesUsed()));

    //Job system
    {
        this->jobSystemSettings.Finalize(this->logicalCpus);
        this->jobSystem.Create(this->jobSystemSettings, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to initialize job system.");
            return;
        }
    }

    FINJIN_DEBUG_LOG_INFO("Memory usage after creating job system: %1%", MemorySize::ToString(GetBytesUsed()));

    //Input, sound, GPU, VR
    this->inputSystemSettings.allocator = &this->applicationAllocator;
    this->soundSystemSettings.allocator = &this->applicationAllocator;
    if (this->gpuSystemSettings.applicationName.assign(this->applicationDelegate->GetName(ApplicationNameFormat::LOWER_CASE)).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign application name to GPU system settings.");
        return;
    }
    this->gpuSystemSettings.allocator = &this->applicationAllocator;
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->vrSystemSettings.allocator = &this->applicationAllocator;
#endif
    CreateSystems(error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize systems objects.");
        return;
    }

    FINJIN_DEBUG_LOG_INFO("Memory usage after creating systems: %1%", MemorySize::ToString(GetBytesUsed()));

    //Initialize windows------------------------------------------------------------------------
    Setting<size_t> mainApplicationViewportIndex(0);

    auto viewportDescriptionCount = this->applicationDelegate->GetApplicationViewportDescriptionCount();
    this->applicationDelegate->SetActualApplicationViewportDescriptionCount(viewportDescriptionCount);
    this->workingWindowInternalName.Create(&this->applicationAllocator);
    for (size_t viewportIndex = 0; viewportIndex < viewportDescriptionCount; viewportIndex++)
    {
        const auto& viewportDescription = this->applicationDelegate->GetApplicationViewportDescription(viewportIndex);

        //Create ApplicationViewport----------------------------
        std::unique_ptr<ApplicationViewport> appViewport(this->applicationDelegate->CreateApplicationViewport(&this->applicationAllocator, viewportIndex));
        if (appViewport == nullptr)
        {
            FINJIN_SET_ERROR(error, "Failed to allocate application viewport.");
            return;
        }
        if (appViewport->IsMain() && !mainApplicationViewportIndex.IsSet())
            mainApplicationViewportIndex = viewportIndex;

        std::unique_ptr<ApplicationViewportDelegate> appWindowDelegate(this->applicationDelegate->CreateApplicationViewportDelegate(&this->applicationAllocator, viewportIndex));
        if (appWindowDelegate == nullptr)
        {
            FINJIN_SET_ERROR(error, "Failed to allocate application viewport delegate.");
            return;
        }

        //Create OSWindow----------------------------

        //Internal name
        if (!viewportDescription.internalName.empty())
        {
            if (this->workingWindowInternalName.assign(viewportDescription.internalName).HasError())
            {
                FINJIN_SET_ERROR(error, "Failed to assign window description's internal name to working window internal name.");
                return;
            }
        }
        else
        {
            if (this->workingWindowInternalName.assign(this->applicationDelegate->GetName(ApplicationNameFormat::LOWER_CASE)).HasError())
            {
                FINJIN_SET_ERROR(error, "Failed to assign application delegates's camel lower name to working window internal name.");
                return;
            }
            if (this->workingWindowInternalName.append("-").HasError())
            {
                FINJIN_SET_ERROR(error, "Failed to append dash to working window internal name.");
                return;
            }
            if (this->workingWindowInternalName.append(Convert::ToString(viewportIndex)).HasError())
            {
                FINJIN_SET_ERROR(error, "Failed to append window index to working window internal name.");
                return;
            }
        }

        //Title
        const Utf8String* windowTitleOrSubtitle;
        if (this->titleBarUsesSubtitle)
            windowTitleOrSubtitle = &viewportDescription.subtitle;
        else
            windowTitleOrSubtitle = !viewportDescription.title.empty() ? &viewportDescription.title : &this->applicationDelegate->GetName(ApplicationNameFormat::DISPLAY);

        //Size
        WindowSize windowSize;
        GetConfiguredApplicationViewportSize(windowSize, viewportDescription.windowFrame);

        //Create
        auto osWindow = AllocatedClass::NewUnique<OSWindow>(&this->applicationAllocator, FINJIN_CALLER_ARGUMENTS, appViewport.get()); //Pass appViewport as 'client data' for faster lookup in OSWindowToApplicationViewport()
        if (osWindow == nullptr)
        {
            FINJIN_SET_ERROR(error, "Failed to allocated OS window.");
            return;
        }
        osWindow->Create(this->workingWindowInternalName, *windowTitleOrSubtitle, viewportDescription.displayName, viewportDescription.windowFrame, windowSize, error);
        if (error)
        {
            osWindow->Destroy();

            FINJIN_SET_ERROR(error, "Failed to create application OS window.");
            return;
        }
        appViewport->SetOSWindow(std::move(osWindow));
        appViewport->SetDelegate(std::move(appWindowDelegate));

        //Add to collection-----------------------------------------
        this->appViewportsController.push_back(std::move(appViewport));
    }

    FINJIN_DEBUG_LOG_INFO("Memory usage after creating windows: %1%", MemorySize::ToString(GetBytesUsed()));

    //Set first window to be the main window if no windows were designated as being the main window
    if (!mainApplicationViewportIndex.IsSet() && !this->appViewportsController.empty())
        this->appViewportsController[0]->SetMain(true);

    //Finish creating windows if all of them have window handles
    //If they don't all have window handles then HandleApplicationViewportsCreated() needs to be called somewhere else
    if (!this->appViewportsController.empty() && this->appViewportsController.AllHaveOutputHandle())
    {
        HandleApplicationViewportsCreated(error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to finish creating application windows.");
            return;
        }

        this->applicationDelegate->OnInitializedApplicationViewportsController(this->appViewportsController);

        this->jobSystem.Start(true, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to start job system.");
            return;
        }
    }

    FINJIN_DEBUG_LOG_INFO("Memory usage at end of application initialization: %1%", MemorySize::ToString(GetBytesUsed()));
}

void Application::Destroy()
{
    if (this->applicationDelegate != nullptr)
        this->applicationDelegate->OnDestroyStart();
    
    this->assetReadQueue.Destroy();
    this->fileSystemOperationQueue.Destroy();

    ApplicationViewportsClosing closingAppViewports;
    this->appViewportsController.GetAllViewports(closingAppViewports);
    for (auto& appViewport : closingAppViewports)
    {
        HandleApplicationViewportEndOfLife(appViewport.get(), true);
        appViewport.reset();
    }

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->vrSystem.Destroy();
#endif

    this->gpuSystem.Destroy();

    this->soundSystem.Destroy();

    this->inputSystem.Destroy();

    this->jobSystem.Destroy();
    
    if (this->applicationDelegate != nullptr)
        this->applicationDelegate->OnDestroyFinish();
}

void Application::WindowMoved(OSWindow* osWindow)
{
}

void Application::WindowResized(OSWindow* osWindow)
{
    auto appViewport = OSWindowToApplicationViewport(osWindow);

    if (appViewport->NotifyWindowResized())
        appViewport->GetDelegate()->OnApplicationViewportResized(appViewport);
}

void Application::WindowGainedFocus(OSWindow* osWindow)
{
    auto appViewport = OSWindowToApplicationViewport(osWindow);
    HandleApplicationViewportGainedFocus(appViewport);
}

void Application::WindowLostFocus(OSWindow* osWindow)
{
    auto appViewport = OSWindowToApplicationViewport(osWindow);
    HandleApplicationViewportLostFocus(appViewport);
}

void Application::WindowClosing(OSWindow* osWindow)
{
    auto appViewport = OSWindowToApplicationViewport(osWindow);
    HandleApplicationViewportEndOfLife(appViewport, true);

    //One window closing indicates all windows should close and that the application should exit
    for (auto& appViewport : this->appViewportsController)
        appViewport->RequestApplicationExit();
}

bool Application::WindowOnDropFiles(OSWindow* osWindow, const Path* fileNames, size_t count)
{
    for (size_t droppedFileIndex = 0; droppedFileIndex < count; droppedFileIndex++)
        FINJIN_DEBUG_LOG_INFO("Dropped: %1%", fileNames[droppedFileIndex]);

    osWindow->Raise();

    return false;
}

void Application::GetConfiguredApplicationViewportSize(WindowSize& windowSize, const Setting<OSWindowRect>& windowFrame)
{
    assert(this->applicationDelegate != nullptr);

    const auto& appSettings = this->applicationDelegate->GetApplicationSettings();

    if (appSettings.isFullScreen && appSettings.isFullScreenExclusive)
        windowSize.SetFullScreenState(WindowSizeState::EXCLUSIVE_FULLSCREEN);

    if (appSettings.windowWidth.IsSet() || appSettings.windowHeight.IsSet())
    {
        auto bounds = windowSize.GetBounds(WindowSizeState::WINDOWED_NORMAL);
        if (appSettings.windowWidth.IsSet())
            bounds.width = appSettings.windowWidth;
        if (appSettings.windowHeight.IsSet())
            bounds.height = appSettings.windowHeight;
        windowSize.SetBounds(WindowSizeState::WINDOWED_NORMAL, bounds);
    }
    else
    {
        auto bounds = windowSize.GetBounds(WindowSizeState::WINDOWED_NORMAL);
        bounds.x = windowFrame.value.GetX();
        bounds.y = windowFrame.value.GetY();
        bounds.width = windowFrame.value.GetWidth();
        bounds.height = windowFrame.value.GetHeight();
        windowSize.SetBounds(WindowSizeState::WINDOWED_NORMAL, bounds);
    }

    if (appSettings.fullScreenWidth.IsSet() || appSettings.fullScreenHeight.IsSet())
    {
        auto bounds = windowSize.GetBounds(WindowSizeState::EXCLUSIVE_FULLSCREEN);
        if (appSettings.fullScreenWidth.IsSet())
            bounds.width = appSettings.fullScreenWidth;
        if (appSettings.fullScreenHeight.IsSet())
            bounds.height = appSettings.fullScreenHeight;
        windowSize.SetBounds(WindowSizeState::EXCLUSIVE_FULLSCREEN, bounds);
    }
}

void Application::HandleApplicationViewportLostFocus(ApplicationViewport* appViewport)
{
    if (appViewport->GetInputContext() != nullptr)
        appViewport->GetInputContext()->HandleApplicationViewportLostFocus();

    if (appViewport->GetSoundContext() != nullptr)
        appViewport->GetSoundContext()->HandleApplicationViewportLostFocus();

#if TOGGLE_FULLSCREEN_EXCLUSIVE_ON_LOST_FOCUS
    if (appViewport->GetOSWindow()->GetWindowSize().IsFullScreenExclusive() && !this->appViewportsController.AnyHasFocus())
    {
        //The window is full screen and none of the application windows have focus
        //Have to assume that Windows is going to forcibly remove the full screen exclusive state of at least one window
        //So to be safe, let all windows finish their scheduled work and then toggle out of full screen exclusive

        for (auto& otherAppViewport : this->appViewportsController)
            otherAppViewport->FinishWork(RenderStatus::GetFinishing());

        this->appViewportsController.RequestFullScreenToggle();
    }
#endif

    appViewport->GetDelegate()->OnApplicationViewportLostFocus(appViewport);
}

void Application::HandleApplicationViewportGainedFocus(ApplicationViewport* appViewport)
{
    if (appViewport->GetInputContext() != nullptr)
        appViewport->GetInputContext()->HandleApplicationViewportGainedFocus();

    if (appViewport->GetSoundContext() != nullptr)
        appViewport->GetSoundContext()->HandleApplicationViewportGainedFocus();

    appViewport->GetDelegate()->OnApplicationViewportGainedFocus(appViewport);
}

void Application::HandleApplicationViewportsCreated(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    for (size_t viewportIndex = 0; viewportIndex < this->applicationDelegate->GetApplicationViewportDescriptionCount(); viewportIndex++)
    {
        auto& appViewport = this->appViewportsController[viewportIndex];
        const auto& viewportDescription = this->applicationDelegate->GetApplicationViewportDescription(viewportIndex);

        HandleApplicationViewportCreated(appViewport.get(), viewportDescription, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to finish creating application window.");
            return;
        }
    }

    //Apply full screen toggle if necessary
    if (this->applicationDelegate->GetApplicationSettings().isFullScreen && !this->appViewportsController.RequestFullScreenToggle())
    {
        FINJIN_SET_ERROR(error, "Failed to apply full screen toggle to window(s).");
        return;
    }
}

void Application::HandleApplicationViewportCreated(ApplicationViewport* appViewport, const ApplicationViewportDescription& viewportDescription, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Input
    {
        this->inputContextSettings.osWindow = appViewport->GetOSWindow();
        std::unique_ptr<InputContext> inputContext(this->inputSystem.CreateContext(this->inputContextSettings, error));
        this->inputContextSettings.osWindow = nullptr;
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to initialize input system for application viewport.");
            return;
        }
        appViewport->SetInputContext(std::move(inputContext));
    }

    //VR
    auto isVRInitialized = false;
#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    if (this->applicationDelegate->GetApplicationSettings().IsVRRequired() && !this->vrSystem.IsAvailable())
    {
        FINJIN_SET_ERROR(error, "VR is requred by the application settings but is unavailable. Try installing the necessary drivers and turning on the hardware.");
        return;
    }
    if (appViewport->IsMain() && this->applicationDelegate->GetApplicationSettings().IsVRRequested())
    {
        this->vrContextSettings.addDeviceHandler = [this, appViewport](VRContext* vrContext, size_t vrDeviceIndex)
        {
            auto deviceClass = vrContext->GetDeviceClass(vrDeviceIndex);
            if (deviceClass == InputDeviceClass::GAME_CONTROLLER)
            {
                auto foundUnused = this->vrGameControllersForInputSystem.FindUnused();
                if (foundUnused != nullptr)
                {
                    foundUnused->vrContext = vrContext;
                    foundUnused->vrDeviceIndex = vrDeviceIndex;
                    this->vrGameControllersForInputSystem.Use(foundUnused);

                    FINJIN_DECLARE_ERROR(error);
                    appViewport->GetInputContext()->AddExternalGameController(foundUnused, true, error);
                    if (error)
                        FINJIN_DEBUG_LOG_ERROR("Failed to add external VR game controller: %1%", error.GetLastNonEmptyErrorMessage());
                }
            }
            else if (deviceClass == InputDeviceClass::HEADSET)
            {
                auto foundUnused = this->vrHeadsetsForInputSystem.FindUnused();
                if (foundUnused != nullptr)
                {
                    foundUnused->vrContext = vrContext;
                    foundUnused->vrDeviceIndex = vrDeviceIndex;
                    this->vrHeadsetsForInputSystem.Use(foundUnused);

                    FINJIN_DECLARE_ERROR(error);
                    appViewport->GetInputContext()->AddExternalHeadset(foundUnused, true, error);
                    if (error)
                        FINJIN_DEBUG_LOG_ERROR("Failed to add external VR headset: %1%", error.GetLastNonEmptyErrorMessage());
                }
            }
        };
        this->vrContextSettings.removeDeviceHandler = [this, appViewport](VRContext* vrContext, size_t vrDeviceIndex)
        {
            auto deviceClass = vrContext->GetDeviceClass(vrDeviceIndex);
            if (deviceClass == InputDeviceClass::GAME_CONTROLLER)
            {
                auto foundUsed = this->vrGameControllersForInputSystem.FindUsed(VRGameControllerForInputSystem(vrContext, vrDeviceIndex));
                if (foundUsed != nullptr)
                {
                    this->vrGameControllersForInputSystem.Unuse(foundUsed);
                    appViewport->GetInputContext()->RemoveExternalGameController(foundUsed);
                }
            }
            else if (deviceClass == InputDeviceClass::HEADSET)
            {
                auto foundUsed = this->vrHeadsetsForInputSystem.FindUsed(VRHeadsetForInputSystem(vrContext, vrDeviceIndex));
                if (foundUsed != nullptr)
                {
                    this->vrHeadsetsForInputSystem.Unuse(foundUsed);
                    appViewport->GetInputContext()->RemoveExternalHeadset(foundUsed);
                }
            }
        };
        this->vrContextSettings.deviceRenderModelLoadedHandler = [this, appViewport](VRContext* vrContext, size_t vrDeviceIndex)
        {
            auto deviceClass = vrContext->GetDeviceClass(vrDeviceIndex);
            if (deviceClass == InputDeviceClass::GAME_CONTROLLER)
            {
                auto foundUnused = this->vrGameControllersForInputSystem.FindUsed(VRGameControllerForInputSystem(vrContext, vrDeviceIndex));
                if (foundUnused != nullptr)
                {
                    //TODO: Do something
                }
            }
        };
        std::unique_ptr<VRContext> vrContext(this->vrSystem.CreateContext(this->vrContextSettings, error));
        this->vrContextSettings.addDeviceHandler = nullptr;
        this->vrContextSettings.removeDeviceHandler = nullptr;
        this->vrContextSettings.deviceRenderModelLoadedHandler = nullptr;
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to initialize VR system for application viewport.");
            return;
        }
        isVRInitialized = vrContext->GetInitializationStatus() == VRContextInitializationStatus::INITIALIZED;
        if (this->applicationDelegate->GetApplicationSettings().IsVRRequired() && !isVRInitialized)
        {
            this->vrSystem.DestroyContext(vrContext.release());
            FINJIN_SET_ERROR(error, "VR is requred by the application settings but failed to initialize. Ensure the hardware is turned on and recognized by the system.");
            return;
        }
        appViewport->SetVRContext(std::move(vrContext));
    }
#endif

    //Sound
    if (appViewport->IsMain())
    {
        this->soundContextSettings.osWindow = appViewport->GetOSWindow();
        std::unique_ptr<SoundContext> soundContext(this->soundSystem.CreateContext(this->soundContextSettings, error));
        this->soundContextSettings.osWindow = nullptr;
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to initialize sound system for application viewport.");
            return;
        }
        appViewport->SetSoundContext(std::move(soundContext));
    }

    //GPU
    {
        this->gpuContextSettings.osWindow = appViewport->GetOSWindow();
        this->gpuContextSettings.gpuID = viewportDescription.gpuID;
    #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
        this->gpuContextSettings.vrContext = appViewport->GetVRContext(); //Needs to be set for a possible mode change later
    #endif
        if (isVRInitialized && this->applicationDelegate->GetApplicationSettings().StartInVR())
        {
        #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
            auto preferredDimensions = appViewport->GetVRContext()->GetPreferredRenderTargetDimensions();
            if (this->applicationDelegate->GetApplicationSettings().mirrorVR)
                this->gpuContextSettings.frameDestination |= GpuFrameDestination::VR_CONTEXT;
            else
                this->gpuContextSettings.frameDestination = GpuFrameDestination::VR_CONTEXT;
            this->gpuContextSettings.renderTargetSize.SetExplicit(preferredDimensions[0], preferredDimensions[1]);
        #endif
        }
        this->gpuContextSettings.frameBufferCount.requested = Limited(viewportDescription.requestedFrameBufferCount, (size_t)EngineConstants::MIN_FRAME_BUFFERS, (size_t)EngineConstants::MAX_FRAME_BUFFERS);
        std::unique_ptr<GpuContext> gpuContext(this->gpuSystem.CreateContext(this->gpuContextSettings, error));
        this->gpuContextSettings.osWindow = nullptr;
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to initialize GPU device for application viewport.");
            return;
        }
        appViewport->SetGpuContext(std::move(gpuContext));
    }

    appViewport->CreateAssetClassFileReaders(this->assetFileReader, this->assetFileSelector, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create asset class file readers for application viewport.");
        return;
    }

    //Job processing pipeline
    {
        auto jobProcessingPipelineSize = appViewport->GetGpuContext()->GetSettings().jobProcessingPipelineSize;
        appViewport->ConfigureJobPipeline
            (
            Limited(viewportDescription.jobProcessingPipelineSize, 1, jobProcessingPipelineSize),
            jobProcessingPipelineSize
            );
        for (size_t stageIndex = 0; stageIndex < appViewport->GetJobPipelineSize(); stageIndex++)
        {
            //Create update context
            auto updateContext = AllocatedClass::NewUnique<WindowedViewportUpdateContext>(&this->applicationAllocator, FINJIN_CALLER_ARGUMENTS, appViewport);
            updateContext->jobSystem = &this->jobSystem;
            updateContext->assetReadQueue = &this->assetReadQueue;
            updateContext->assetClassFileReaders = &appViewport->GetAssetClassFileReaders();
            updateContext->standardPaths = &this->standardPaths;
            updateContext->userInfo = &this->userInfo;
            updateContext->domainInfo = &this->domainInfo;

        #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
            if (!updateContext->vrEvents.CreateEmpty(this->vrContextSettings.maxCommandsPerUpdate, &this->applicationAllocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate VR events collection.");
                return;
            }
            {
                MemoryArenaSettings arenaSettings;
                arenaSettings.byteCount = sizeof(VRCommand) * this->vrContextSettings.maxCommandsPerUpdate;
                auto arena = this->applicationAllocator.AllocateArena(arenaSettings, FINJIN_CALLER_ARGUMENTS);
                if (arena.IsNull())
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate VR commands arena.");
                    return;
                }
                updateContext->inputCommands.Create(std::move(arena), error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate VR commands collection.");
                    return;
                }
            }
        #endif

            if (!updateContext->inputEvents.CreateEmpty(this->inputContextSettings.maxCommandsPerUpdate, &this->applicationAllocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate input events collection.");
                return;
            }
            {
                MemoryArenaSettings arenaSettings;
                arenaSettings.byteCount = sizeof(InputCommand) * this->inputContextSettings.maxCommandsPerUpdate;
                auto arena = this->applicationAllocator.AllocateArena(arenaSettings, FINJIN_CALLER_ARGUMENTS);
                if (arena.IsNull())
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate input commands arena.");
                    return;
                }
                updateContext->inputCommands.Create(std::move(arena), error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate input commands collection.");
                    return;
                }
            }

            if (!updateContext->soundEvents.CreateEmpty(this->soundContextSettings.maxCommandsPerUpdate, &this->applicationAllocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate sound events collection.");
                return;
            }
            {
                MemoryArenaSettings arenaSettings;
                arenaSettings.byteCount = sizeof(SoundCommand) * this->soundContextSettings.maxCommandsPerUpdate;
                auto arena = this->applicationAllocator.AllocateArena(arenaSettings, FINJIN_CALLER_ARGUMENTS);
                if (arena.IsNull())
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate sound commands arena.");
                    return;
                }
                updateContext->soundCommands.Create(std::move(arena), error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate sound commands collection.");
                    return;
                }
            }

            if (!updateContext->gpuEvents.CreateEmpty(this->gpuContextSettings.maxCommandsPerUpdate, &this->applicationAllocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate GPU events collection.");
                return;
            }

            {
                MemoryArenaSettings arenaSettings;
                arenaSettings.byteCount = sizeof(GpuCommand) * this->gpuContextSettings.maxCommandsPerUpdate;
                auto arena = this->applicationAllocator.AllocateArena(arenaSettings, FINJIN_CALLER_ARGUMENTS);
                if (arena.IsNull())
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate GPU commands arena.");
                    return;
                }
                updateContext->gpuCommands.Create(std::move(arena), error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate GPU commands collection.");
                    return;
                }
            }

            //Create render context
            auto renderContext = AllocatedClass::NewUnique<WindowedViewportRenderContext>(&this->applicationAllocator, FINJIN_CALLER_ARGUMENTS, appViewport);

            //Set update and render contexts into the stage
            appViewport->SetJobPipelineStageData(stageIndex, std::move(updateContext), std::move(renderContext));
        }
    }

    //At this point the appViewport is successfully set up so add a listener
    appViewport->GetOSWindow()->AddWindowEventListener(this);

    //Apply window size if it isn't full screen
    if (!this->applicationDelegate->GetApplicationSettings().isFullScreen)
        appViewport->GetOSWindow()->ApplyWindowSize();
}

void Application::HandleApplicationViewportEndOfLife(ApplicationViewport* appViewport, bool isPermanent)
{
    //Let the application delegate perform application specific logic on the window
    //At a minimum it should let any async work finish
    appViewport->FinishWork(RenderStatus::GetFinishing());
    appViewport->GetDelegate()->OnApplicationViewportClosing(appViewport);

#if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
    this->vrSystem.DestroyContext(appViewport->DetachVRContext());
#endif
    this->gpuSystem.DestroyContext(appViewport->DetachGpuContext());
    this->soundSystem.DestroyContext(appViewport->DetachSoundContext());
    this->inputSystem.DestroyContext(appViewport->DetachInputContext());

    //Remove event listener to prevent triggering any more events
    if (appViewport->GetOSWindow() != nullptr)
        appViewport->GetOSWindow()->RemoveWindowEventListener(this);

    if (isPermanent)
    {
        //An application-owned window, the usual case
        this->appViewportsController.FindAndDeleteAndRemove(appViewport);
    }
    else if (appViewport->GetOSWindow() != nullptr)
    {
        //A system-owned window. Occurs on Android and Windows universal
        appViewport->GetOSWindow()->ClearSystemWindowHandle();
    }
}

ApplicationViewport* Application::OSWindowToApplicationViewport(OSWindow* osWindow)
{
    auto data = osWindow->GetClientData();
    assert(data != nullptr);
    return reinterpret_cast<ApplicationViewport*>(data);
}

void Application::ReadBootFile(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView section, key, value;

    ConfigDocumentReader reader;
    for (auto line = reader.Start(this->configFileBuffer); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                line->GetSectionName(section);

                if (section == "asset-reader" || section == "asset-file-system" || section == "settings-files" || section == "strings-files")
                {
                    //Do nothing
                }
                else if (this->applicationDelegate->ReadBootFileSection(reader, section, error))
                {
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read '%1%' section in boot file.", section));
                        return;
                    }
                }
                else
                    reader.SkipScope();

                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                line->GetKeyAndValue(key, value);

                if (section == "asset-reader")
                {
                    if (key == "root-directory-name")
                    {
                        this->assetFileReader.AddRootDirectoryName(value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' root directory name.", value));
                            return;
                        }
                    }
                    else if (key == "file-system")
                    {
                        ApplicationFileSystem applicationFileSystem;
                        ApplicationFileSystemUtilities::Parse(applicationFileSystem, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid application file system value '%1%'.", value));
                            return;
                        }

                        this->assetFileReader.AddFileSystem(&this->fileSystems[applicationFileSystem], error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' file system to asset file reader.", value));
                            return;
                        }
                    }
                    else
                    {
                        this->applicationDelegate->ReadBootFileItem(section, key, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Application delegate failed to read boot file item '%1%'='%2%' in section '%3%'.", key, value, section));
                            return;
                        }
                    }
                }
                else if (section == "asset-file-system")
                {
                    if (key == "zip")
                    {
                        if (this->workingPath2.assign(value).HasError())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign zip path '%1%' to working path.", value));
                            return;
                        }
                    #if ALLOW_USER_HOME_DIRECTORY_EXPANSION
                        if (this->workingPath2.ExpandUserHomeDirectory().HasError())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to expand zip path '%1%' user home directory component.", value));
                            return;
                        }
                    #endif

                        if (this->workingPath.assign(this->standardPaths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].path).HasError())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign application bundle directory '%1%' to working path.", this->standardPaths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].path));
                            return;
                        }
                        if ((this->workingPath /= this->workingPath2).HasError())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to append zip file '%1%' to working path.", this->workingPath2));
                            return;
                        }

                        this->fileSystems[ApplicationFileSystem::READ_APPLICATION_ASSETS].AddZipArchive(this->workingPath, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add zip file archive '%1%' to mapped file system.", this->workingPath));
                            return;
                        }
                    }
                    else if (key == "directory")
                    {
                        if (this->workingPath2.assign(value).HasError())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign directory path '%1%' to working path.", value));
                            return;
                        }
                    #if ALLOW_USER_HOME_DIRECTORY_EXPANSION
                        if (this->workingPath2.ExpandUserHomeDirectory().HasError())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to expand directory path '%1%' user home directory component.", value));
                            return;
                        }
                    #endif

                        if (this->workingPath.assign(this->standardPaths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].path).HasError())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign application bundle directory '%1%' to working path.", this->standardPaths[WhichStandardPath::APPLICATION_BUNDLE_DIRECTORY].path));
                            return;
                        }
                        if ((this->workingPath /= this->workingPath2).HasError())
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to append directory '%1%' to working path.", this->workingPath2));
                            return;
                        }

                        this->fileSystems[ApplicationFileSystem::READ_APPLICATION_ASSETS].AddDirectory(this->workingPath, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add directory '%1%' to mapped file system.", this->workingPath));
                            return;
                        }
                    }
                    else
                    {
                        this->applicationDelegate->ReadBootFileItem(section, key, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Application delegate failed to read boot file item '%1%'='%2%' in section '%3%'.", key, value, section));
                            return;
                        }
                    }
                }
                else if (section == "gpu")
                {
                    if (key == "settings")
                    {
                        this->gpuContextSettings.contextSettingsFileNames.AddLocalFile(value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' GPU context settings file.", value));
                            return;
                        }
                    }
                    else if (key == "static-mesh-renderer-settings")
                    {
                        this->gpuContextSettings.staticMeshRendererSettingsFileNames.AddLocalFile(value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' GPU context static mesh renderer settings file.", value));
                            return;
                        }
                    }
                    else
                    {
                        this->gpuContextSettings.HandleTopLevelSetting(key, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to parse 'gpu' section setting.");
                            return;
                        }
                    }
                }
                else if (section == "input")
                {
                    if (key == "settings")
                    {
                        this->inputContextSettings.settingsFileNames.AddLocalFile(value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' input context asset file.", value));
                            return;
                        }
                    }
                    else
                    {
                        this->inputContextSettings.HandleTopLevelSetting(key, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to parse 'input' section setting.");
                            return;
                        }
                    }
                }
                else if (section == "sound")
                {
                    if (key == "settings")
                    {
                        this->soundContextSettings.settingsFileNames.AddLocalFile(value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' sound context settings file.", value));
                            return;
                        }
                    }
                    else
                    {
                        this->soundContextSettings.HandleTopLevelSetting(key, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to parse 'sound' section setting.");
                            return;
                        }
                    }
                }
                #if FINJIN_TARGET_VR_SYSTEM != FINJIN_TARGET_VR_SYSTEM_NONE
                else if (section == "vr")
                {
                    if (key == "settings")
                    {
                        this->vrContextSettings.settingsFileNames.AddLocalFile(value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' VR context settings file.", value));
                            return;
                        }
                    }
                    else
                    {
                        this->vrContextSettings.HandleTopLevelSetting(key, value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to parse 'vr' section setting.");
                            return;
                        }
                    }
                }
                #endif
                else if (section == "string-table-files")
                {
                    if (key == "default")
                    {
                        this->defaultStringsAssetFileNames.AddLocalFile(value, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add '%1%' default strings file.", value));
                            return;
                        }
                    }
                }
                else
                {
                    this->applicationDelegate->ReadBootFileItem(section, key, value, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Application delegate failed to read boot file item '%1%'='%2%' in section '%3%'.", key, value, section));
                        return;
                    }
                }

                break;
            }
            default: break;
        }
    }
}

size_t Application::GetBytesUsed()
{
    auto bytesFreeNow = GetAllocator()->GetBytesFree();
    if (bytesFreeNow <= this->estimatedBytesFreeAtInitialization)
        return this->estimatedBytesFreeAtInitialization - bytesFreeNow;
    else
        return 0;
}
