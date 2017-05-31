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
#include "finjin/common/CommandLineArgsProcessor.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/StandardPaths.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/ApplicationFileSystem.hpp"
#include "finjin/engine/ApplicationNameFormat.hpp"
#include "finjin/engine/ApplicationSettings.hpp"
#include "finjin/engine/ApplicationViewportDescription.hpp"
#include "finjin/engine/DisplayInfo.hpp"
#include "finjin/engine/GpuDescription.hpp"
#include "finjin/engine/GpuSystem.hpp"
#include "finjin/engine/MemorySettings.hpp"
#include "finjin/engine/SoundAdapterDescription.hpp"
#include "finjin/engine/SoundSourceCommon.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class ApplicationViewport;
    class ApplicationViewportsController;
    class ApplicationViewportDelegate;

    using namespace Finjin::Common;

    /**
     * General order of operations during application initialization:
     * During initialization:
     *  1)ReadCommandLineSettings() - Reads/parses the specified command line arguments.
     *  2)GetMaxFileSystemEntries() - Called to get information about the file system in preparation for reading files.
     *  3)GetBootFileNameCount() - Gets the name of the boot file. If none is specified, none will be loaded.
     *    3a)GetBootFileName() - Gets the name of the boot file. If none is specified, none will be loaded.
     *  4)ReadBootFileItem() - Reads/parses one item that was in the boot file.
     *  5)GetSettingsFileNameCount() - Gets the number of application-specific settings files to read.
     *    5a)GetSettingsFileName() - Gets the asset reference of the file to read.
     *    5b)ReadSettings() - Called with the data that was read at the asset reference returned by GetSettingsFileName().
     *  6)OnSoundDevicesEnumerated() - Depending on platform this may not be called
     *  7)OnGpusEnumerated() - Depending on platform this may not be called
     *  8)GetApplicationViewportDescriptionCount()
     *    8a)GetApplicationViewportDescription() - Called to get an application viewport description.
     *    8b)CreateApplicationViewport() - Called to create the application viewport.
     *  9)OnInitializedApplicationViewportsController() - Called when all the application windows have been fully initialized.
     *    On all platforms except Android this will be called after all the viewports have been created in previous step.
     *
     * During runtime:
     *  -OnTickApplicationViewports() - Called to call Tick() on all the application viewports. 
     *   If overridden and the override performs the Tick() on the viewports, this should return true.
     */
    class ApplicationDelegate : public AllocatedClass
    {
    public:
        ApplicationDelegate(Allocator* allocator);
        ~ApplicationDelegate();

        virtual size_t GetMaxFileSystemEntries(ApplicationFileSystem fileSystem, FileSystemEntryType findTypes) const;

        virtual const Utf8String& GetName(ApplicationNameFormat format) const = 0;

        virtual ReadCommandLineResult ReadCommandLineSettings(CommandLineArgsProcessor& argsProcessor, Error& error);
        virtual void GetCommandLineUsage(Utf8String& usage);

        virtual size_t GetBootFileNameCount() const;
        virtual const Path& GetBootFileName(size_t index, bool& required) const;
        virtual bool ReadBootFileSection(ConfigDocumentReader& reader, const Utf8StringView& section, Error& error);
        virtual bool ReadBootFileItem(const Utf8StringView& section, const Utf8StringView& key, const Utf8StringView& value, Error& error);

        //virtual void OnInitializedMemory(Allocator* allocator, MemorySettings& memoryConfigurationSettings) {}

        virtual size_t GetSettingsFileNameCount() const;
        virtual const AssetReference& GetSettingsFileName(size_t index, bool& required) const;
        virtual void ReadSettings(size_t index, const ByteBuffer& settingsBuffer, Error& error);

        virtual void OnSoundDevicesEnumerated(const SoundAdapterDescriptions& devices);

        virtual void OnGpusEnumerated(const HardwareGpuDescriptions& hardwareGpus, const SoftwareGpuDescriptions& softwareGpus);

        virtual const ApplicationSettings& GetApplicationSettings() const = 0;

        virtual void OnDestroyStart() = 0;
        virtual void OnDestroyFinish() = 0;

        size_t GetApplicationViewportDescriptionCount() const;
        virtual size_t GetRequestedApplicationViewportDescriptionCount() const = 0;
        virtual void SetActualApplicationViewportDescriptionCount(size_t count) {}
        virtual ApplicationViewportDescription& GetApplicationViewportDescription(size_t index) = 0;

        virtual ApplicationViewport* CreateApplicationViewport(Allocator* allocator, size_t index);
        virtual ApplicationViewportDelegate* CreateApplicationViewportDelegate(Allocator* allocator, size_t index);

        virtual void OnInitializedApplicationViewportsController(ApplicationViewportsController& appViewportsController) {}
        
        virtual bool OnTickApplicationViewports(ApplicationViewportsController& appViewportsController, Error& error);
    };

} }
