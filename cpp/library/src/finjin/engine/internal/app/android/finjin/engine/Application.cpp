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
#include "finjin/engine/Application.hpp"
#include "finjin/common/AndroidJniUtilities.hpp"
#include "finjin/common/AndroidUtilities.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/NvAndroidNativeAppGlue.h"
#include "finjin/engine/ApplicationDelegate.hpp"
#include "finjin/engine/DisplayInfo.hpp"
#include "finjin/engine/OSWindow.hpp"
#include "finjin/engine/PlatformCapabilities.hpp"
#include <sys/system_properties.h>

using namespace Finjin::Engine;


//Local types-------------------------------------------------------------------
class AndroidApplicationAssetsVirtualFileSystemRoot : public VirtualFileSystemRoot
{
public:
    AndroidApplicationAssetsVirtualFileSystemRoot(Allocator* allocator) :
        VirtualFileSystemRoot(allocator)
    {
        this->androidApp = nullptr;
        this->rootAssetDir = nullptr;
    }

    ~AndroidApplicationAssetsVirtualFileSystemRoot()
    {
        CloseRoot();
    }

    void OpenRoot(android_app* androidApp, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        this->androidApp = androidApp;

        this->rootAssetDir = AAssetManager_openDir(androidApp->activity->assetManager, "");
        if (this->rootAssetDir == nullptr)
        {
            FINJIN_SET_ERROR(error, "Failed to open root asset directory.");
            return;
        }

        if (this->description.assign("Application assets").HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign description.");
            return;
        }
    }

    void CloseRoot()
    {
        if (this->rootAssetDir != nullptr)
        {
            AAssetDir_close(this->rootAssetDir);
            this->rootAssetDir = nullptr;
        }
    }

    EnumerationResult Enumerate(FileSystemEntries& entries, FileSystemEntryType findTypes, size_t maxDepth, Error& error) override
    {
        FINJIN_ERROR_METHOD_START(error);

        AndroidJniUtilities jniUtils(androidApp);

        int apkAssetPathCount;
        if (jniUtils.GetIntField(apkAssetPathCount, "apkAssetPathCount") && apkAssetPathCount > 0)
        {
            for (int i = 0; i < apkAssetPathCount; i++)
            {
                int apkAssetPathType;
                if (!jniUtils.GetIntArrayFieldElement(apkAssetPathType, "apkAssetPathTypes", i))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get APK asset path type for item at index %1%.", i));
                    return EnumerationResult::INCOMPLETE;
                }

                assert(apkAssetPathType == 0 || apkAssetPathType == 1); //Values are defined in FinjinNativeActivity.java
                auto type = apkAssetPathType == 0 ? FileSystemEntryType::FILE : FileSystemEntryType::DIRECTORY;
                if (AnySet(type & findTypes))
                {
                    auto fileSystemEntry = entries.Add();
                    if (fileSystemEntry == nullptr)
                    {
                        FINJIN_SET_ERROR(error, "Failed to get free database entry for Android APK path.");
                        return EnumerationResult::INCOMPLETE;
                    }

                    if (!jniUtils.GetStringArrayFieldElement(fileSystemEntry->relativePath, "apkAssetPaths", i))
                    {
                        entries.CancelAdd(fileSystemEntry);

                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get APK asset path for item at index %1%.", i));
                        return EnumerationResult::INCOMPLETE;
                    }

                    int64_t apkAssetPathSize;
                    if (!jniUtils.GetLongArrayFieldElement(apkAssetPathSize, "apkAssetPathSizes", i))
                    {
                        entries.CancelAdd(fileSystemEntry);

                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get APK asset path size for item at index %1%.", i));
                        return EnumerationResult::INCOMPLETE;
                    }

                    fileSystemEntry->decompressedSize = static_cast<size_t>(apkAssetPathSize);
                    fileSystemEntry->type = type;
                }
            }
        }

        return EnumerationResult::COMPLETE;
    }

    const Utf8String& GetInternalVolumeID() const override
    {
        static Utf8String volumeID("/");
        return volumeID;
    }

    const Path& GetFileSystemPath() const override
    {
        return Path::GetEmpty();
    }

    FileOperationResult Read(const Path& relativeFilePath, ByteBuffer& buffer, Error& error) override
    {
        FINJIN_ERROR_METHOD_START(error);

        auto asset = AAssetManager_open(this->androidApp->activity->assetManager, relativeFilePath.c_str(), AASSET_MODE_STREAMING);
        if (asset == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to open asset '%1%'.", relativeFilePath));
            return FileOperationResult::NOT_FOUND;
        }

        auto byteCount = AAsset_getRemainingLength64(asset);
        if (byteCount > 0)
        {
            if (buffer.resize(byteCount) < byteCount)
            {
                AAsset_close(asset);

                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate buffer for asset '%1%'.", relativeFilePath));
                return FileOperationResult::FAILURE;
            }

            auto result = AAsset_read(asset, buffer.data(), byteCount);
            if (result < 0)
            {
                AAsset_close(asset);

                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("There was an error while reading asset '%1%': %2%", relativeFilePath, result));
                return FileOperationResult::FAILURE;
            }
        }
        else
            buffer.clear();

        AAsset_close(asset);

        return FileOperationResult::SUCCESS;
    }

    FileOperationResult Read(const Path& relativeFilePath, ByteBuffer& buffer) override
    {
        auto asset = AAssetManager_open(this->androidApp->activity->assetManager, relativeFilePath.c_str(), AASSET_MODE_STREAMING);
        if (asset == nullptr)
            return FileOperationResult::NOT_FOUND;

        auto byteCount = AAsset_getRemainingLength64(asset);
        if (byteCount > 0)
        {
            if (buffer.resize(byteCount) < byteCount)
            {
                AAsset_close(asset);
                return FileOperationResult::FAILURE;
            }

            auto res = AAsset_read(asset, buffer.data(), byteCount);
            if (res < 0)
            {
                AAsset_close(asset);
                return FileOperationResult::FAILURE;
            }
        }
        else
            buffer.clear();

        AAsset_close(asset);
        return FileOperationResult::SUCCESS;
    }

    bool CanOpen(const Path& relativeFilePath, FileOpenMode mode) override
    {
        if (mode == FileOpenMode::WRITE)
            return false;

        auto asset = AAssetManager_open(this->androidApp->activity->assetManager, relativeFilePath.c_str(), AASSET_MODE_STREAMING);
        if (asset == nullptr)
            return false;

        AAsset_close(asset);

        return true;
    }

    FileOperationResult Open(const Path& relativeFilePath, FileOpenMode mode, VirtualFileHandle& fileHandle) override
    {
        assert(!fileHandle.IsOpen());

        auto asset = AAssetManager_open(this->androidApp->activity->assetManager, relativeFilePath.c_str(), AASSET_MODE_STREAMING);
        if (asset == nullptr)
            return FileOperationResult::NOT_FOUND;

        fileHandle.ptr = asset;

        fileHandle.fileSystemRoot = this;

        return FileOperationResult::SUCCESS;
    }

    uint64_t Skip(VirtualFileHandle& fileHandle, uint64_t byteCount) override
    {
        if (fileHandle.IsOpen())
        {
            auto asset = static_cast<AAsset*>(fileHandle.ptr);

            auto oldPosition = AAsset_seek64(asset, 0, SEEK_CUR);

            auto newPosition = AAsset_seek64(asset, byteCount, SEEK_CUR);
            if (newPosition < 0)
                return newPosition = oldPosition;

            auto bytesSkipped = newPosition - oldPosition;
            return bytesSkipped;
        }

        return 0;
    }

    size_t Read(VirtualFileHandle& fileHandle, void* bytes, size_t byteCount) override
    {
        if (fileHandle.IsOpen())
        {
            auto asset = static_cast<AAsset*>(fileHandle.ptr);

            auto bytesRead = AAsset_read(asset, bytes, byteCount);
            if (bytesRead < 0)
                bytesRead = 0;

            return bytesRead;
        }

        return 0;
    }

    size_t Write(VirtualFileHandle& fileHandle, const void* bytes, size_t byteCount) override
    {
        return 0;
    }

    void Close(VirtualFileHandle& fileHandle) override
    {
        if (fileHandle.IsOpen())
        {
            auto asset = static_cast<AAsset*>(fileHandle.ptr);

            AAsset_close(asset);

            fileHandle.ptr = nullptr;
        }
    }

    const Utf8String& GetDescription() const
    {
        return this->description;
    }

private:
    android_app* androidApp;
    AAssetDir* rootAssetDir;
};


//Local functions---------------------------------------------------------------
static bool IsAppFocused(android_app* androidApp)
{
    return nv_app_status_active(androidApp) && nv_app_status_focused(androidApp);
}

static void EnumerateAndCacheDisplays(android_app* androidApp)
{
    DisplayInfos displays;
    displays.Enumerate(androidApp);

    PlatformCapabilities::GetInstance().CacheDisplays(displays);
}

static void OnAndroidApplicationCommand(android_app* androidApp, int32_t cmd)
{
    FINJIN_DECLARE_ERROR(error);

    auto windowedApp = reinterpret_cast<Application*>(androidApp->userData);

    static int windowInitCount = 0;
    static OSWindowSize oldWindowSize(0, 0);

    switch (cmd)
    {
        case APP_CMD_INPUT_CHANGED:
        {
            break;
        }
        case APP_CMD_SAVE_STATE:
        {
            windowedApp->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::SAVE_STATE), error);
            /*this->androidApp->savedState = malloc(sizeof(ViewerRestorableState));
            this->androidApp->savedStateSize = sizeof(ViewerRestorableState);
            GetRestorableState(*((ViewerRestorableState*)this->androidApp->savedState));*/

            break;
        }
        case APP_CMD_INIT_WINDOW:
        {
            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_INIT_WINDOW");
            //androidApp->savedState

            EnumerateAndCacheDisplays(androidApp);
            oldWindowSize = OSWindowSize(0, 0);

            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_INIT_WINDOW: 1");

            //Set window handle for the one and only window
            if (!windowedApp->GetViewportsController().empty())
                windowedApp->GetViewportsController()[0]->GetOSWindow()->SetWindowHandle(androidApp->window);

            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_INIT_WINDOW: 2");

            //Perform window-related initialization
            windowedApp->HandleApplicationViewportsCreated(error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to finish initializing windows.");
                FINJIN_DEBUG_LOG_ERROR("Error in OnAndroidApplicationCommand(): %1%", error.ToString());
                break;
            }

            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_INIT_WINDOW: 3");

            //Update window size
            if (!windowedApp->GetViewportsController().empty())
                oldWindowSize = windowedApp->GetViewportsController()[0]->GetOSWindow()->GetClientSize();

            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_INIT_WINDOW: 4");

            //Call windows controller initialized callback for the first time
            if (windowInitCount == 0)
                windowedApp->GetDelegate()->OnInitializedApplicationViewportsController(windowedApp->GetViewportsController());

            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_INIT_WINDOW: 5");

            //Start job system
            windowedApp->GetJobSystem().Start(true, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to start job system.");
                FINJIN_DEBUG_LOG_ERROR("Error in OnAndroidApplicationCommand(): %1%", error.ToString());
                break;
            }

            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_INIT_WINDOW: 6");

            //Increment counter
            windowInitCount++;

            FINJIN_DEBUG_LOG_INFO("Leaving OnAndroidApplicationCommand: APP_CMD_INIT_WINDOW");

            break;
        }
        case APP_CMD_TERM_WINDOW:
        {
            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_TERM_WINDOW");

            for (auto& appViewport : windowedApp->GetViewportsController())
                windowedApp->HandleApplicationViewportEndOfLife(appViewport.get(), false);

            windowedApp->GetJobSystem().Stop();

            break;
        }
        case APP_CMD_WINDOW_RESIZED:
        {
            EnumerateAndCacheDisplays(androidApp);

            if (!windowedApp->GetViewportsController().empty() && windowedApp->GetViewportsController().AllHaveOutputHandle())
            {
                auto newWindowSize = windowedApp->GetViewportsController()[0]->GetOSWindow()->GetClientSize();
                if (newWindowSize != oldWindowSize)
                {
                    //NOTE: This will not occur during a screen rotation since it happens after APP_CMD_INIT_WINDOW,
                    //when the window is created and oldWindowSize is updated
                    //So, if we got here, the window really was resized. This could occur in versions of Android
                    //that allow the multiple apps on screen
                    oldWindowSize = newWindowSize;
                    windowedApp->GetViewportsController()[0]->NotifyWindowResized();
                }
            }
            break;
        }
        case APP_CMD_WINDOW_REDRAW_NEEDED:
        {
            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_WINDOW_REDRAW_NEEDED");
            break;
        }
        case APP_CMD_CONTENT_RECT_CHANGED:
        {
            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_CONTENT_RECT_CHANGED");
            EnumerateAndCacheDisplays(androidApp);
            break;
        }
        case APP_CMD_PAUSE:
        {
            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_PAUSE");
            windowedApp->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::PAUSE), error);
            break;
        }
        case APP_CMD_RESUME:
        {
            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_RESUME");
            windowedApp->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::RESUME), error);
            break;
        }
        case APP_CMD_CONFIG_CHANGED:
        {
            FINJIN_DEBUG_LOG_INFO("OnAndroidApplicationCommand: APP_CMD_CONFIG_CHANGED");
            EnumerateAndCacheDisplays(androidApp);
            break;
        }
        case APP_CMD_LOW_MEMORY:
        {
            windowedApp->OnSystemMessage(ApplicationSystemMessage(ApplicationSystemMessage::LOW_MEMORY_WARNING), error);
            break;
        }
    }
}

static int32_t OnAndroidApplicationInputCommand(android_app* androidApp, AInputEvent* event)
{
    auto windowedApp = reinterpret_cast<Application*>(androidApp->userData);

    auto& inputSystem = windowedApp->GetInputSystem();
    return inputSystem.HandleApplicationInputEvent(event);
}

static void GetAndroidInternalVersion(Utf8String& result)
{
    char osVersion[PROP_VALUE_MAX + 1];
    int osVersionLength = __system_property_get("ro.build.version.release", osVersion);
    result = osVersion;
}


//Implementation----------------------------------------------------------------
void Application::InitializeGlobals(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(this->applicationHandle != nullptr);
    auto androidApp = reinterpret_cast<android_app*>(this->applicationHandle);

    //Set up root file system
    auto applicationAssetsRoot = AllocatedClass::NewUnique<AndroidApplicationAssetsVirtualFileSystemRoot>(GetAllocator(), FINJIN_CALLER_ARGUMENTS);
    applicationAssetsRoot->OpenRoot(androidApp, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add application assets to file system.");
        return;
    }
    GetFileSystem(ApplicationFileSystem::READ_APPLICATION_ASSETS).AddRoot(std::move(applicationAssetsRoot), error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add application root.");
        return;
    }

    //One application window
    this->maxWindows = 1;

    //Application name format
    this->directoryApplicationNameFormat = ApplicationNameFormat::DOTTED_WITH_ORGANIZATION_PREFIX;

    //Can't move or resize windows
    this->canCloseWindowsInternally = false;
    this->canExitInternally = false;
    this->canMoveWindows = false;
    this->canResizeWindows = false;
    this->hasClipboard = AndroidUtilities::IsAndroidMobile(androidApp);

    //Layout direction
    if (AndroidUtilities::HasRightToLeftLayout(androidApp))
        this->layoutDirection = LayoutDirection::RIGHT_TO_LEFT;

    //Get language/country
    char lang[100] = {};
    char country[100] = {};
    AConfiguration_getLanguage(androidApp->config, lang);
    AConfiguration_getCountry(androidApp->config, country);
    SetLanguageAndCountry(lang, country);

    //Device model/manufacturer
    AndroidJniUtilities jniUtils(androidApp);
    jniUtils.GetStringField(this->deviceManufacturer, "manufacturer");
    jniUtils.GetStringField(this->deviceModel, "model");

    //Some other properties
    this->operatingSystemInternalName = "android";
    GetAndroidInternalVersion(this->operatingSystemInternalVersion);
    if (AndroidUtilities::IsAndroidTV(androidApp))
    {
        this->hasClipboard = false;
        this->operatingSystemName = "Android TV";
    }
    else
    {
        this->operatingSystemName = "Android";
    }

    //FINJIN_DEBUG_LOG_INFO("Manufacturer: %s, model: %s, OS: %s", this->deviceManufacturer.c_str(), this->deviceModel.c_str(), this->operatingSystemName.c_str());
}

void Application::CreateSystems(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(this->applicationHandle != nullptr);
    auto androidApp = reinterpret_cast<android_app*>(this->applicationHandle);

    //Input--------------------
    this->inputSystemSettings.useSystemBackButton = this->applicationDelegate->GetApplicationSettings().useSystemBackButton;
    if (this->applicationDelegate->GetApplicationSettings().useAccelerometer.value)
    {
        this->inputSystemSettings.useAccelerometer = true;
        this->inputSystemSettings.accelerometerAlooperID = this->applicationGlobals.NewLooperID();
    }
    this->inputSystem.Create(this->inputSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize input system.");
        return;
    }

    //Sound--------------------
    this->soundSystem.Create(this->soundSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize sound system.");
        return;
    }

    //GPU------------------
    this->gpuSystem.Create(gpuSystemSettings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to initialize GPU system.");
        return;
    }

    //Notify delegate
    this->applicationDelegate->OnGpusEnumerated(this->gpuSystem.GetHardwareGpuDescriptions(), this->gpuSystem.GetSoftwareGpuDescriptions());
}

bool Application::MainLoop(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Ensure some data has been set---------------------------------------
    assert(this->applicationDelegate->GetApplicationViewportDescriptionCount() == 1);

    //Prepare callbacks-------------------------------------------------------
    auto androidApp = reinterpret_cast<android_app*>(this->applicationHandle);
    androidApp->userData = this;
    androidApp->onAppCmd = OnAndroidApplicationCommand;
    androidApp->onInputEvent = OnAndroidApplicationInputCommand;

    //Enter main loop---------------------------------------------------------
    int ident;
    int events;
    android_poll_source* source;
    auto done = false;
    while (!done)
    {
        //Handle queued events-----------------
        auto hadFocus = IsAppFocused(androidApp);
        while ((ident = ALooper_pollAll(hadFocus ? 0 : 250, nullptr, &events, (void**)&source)) >= 0)
        {
            //Process event
            if (source != nullptr)
                source->process(androidApp, source);

            if (this->appViewportsController[0]->GetInputContext() != nullptr)
                this->appViewportsController[0]->GetInputContext()->ProcessQueue(ident);

            //Exit main loop if app is no longer running (APP_CMD_DESTROY was received)
            if (!nv_app_status_running(androidApp))
            {
                done = true;
                break;
            }
        }

        //Perform update----------------------
        if (!done)
        {
            if (!error && !this->appViewportsController.empty())
            {
                auto hasFocus = IsAppFocused(androidApp);

                if (hadFocus && !hasFocus)
                    HandleApplicationViewportLostFocus(this->appViewportsController[0].get());
                else if (!hadFocus && hasFocus)
                    HandleApplicationViewportGainedFocus(this->appViewportsController[0].get());

                if (hasFocus)
                {
                    Tick(error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Error processing tick.");
                        ANativeActivity_finish(androidApp->activity);
                    }
                }
            }
            else
            {
                //An error occurred or there are no more windows, time to exit
                ANativeActivity_finish(androidApp->activity);
            }
        }
    }

    return true;
}

void Application::ReportError(const Error& error)
{
    FINJIN_DEBUG_LOG_ERROR("%1%", error.ToString());
}

void Application::ShowTheCursor()
{
    //Do nothing
}

void Application::HideTheCursor()
{
    //Do nothing
}
