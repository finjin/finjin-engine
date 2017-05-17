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
#include "ApplicationDelegate.hpp"
#include "ApplicationViewport.hpp"
#include "ApplicationViewportDelegate.hpp"
#include "finjin/engine/PlatformCapabilities.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
ApplicationDelegate::ApplicationDelegate(Allocator* allocator) : AllocatedClass(allocator)
{
}

ApplicationDelegate::~ApplicationDelegate()
{
}

size_t ApplicationDelegate::GetMaxFileSystemEntries(ApplicationFileSystem fileSystem, FileSystemEntryType entryTypes) const
{
    //Sensible defaults
    switch (fileSystem)
    {
        case ApplicationFileSystem::READ_APPLICATION_ASSETS: return 100;
        case ApplicationFileSystem::READ_USER_DATA: return 100;
        case ApplicationFileSystem::READ_WRITE_USER_APPLICATION_CACHE_DATA: return 100;
        case ApplicationFileSystem::READ_WRITE_APPLICATION_DATA: return 50;
        case ApplicationFileSystem::READ_WRITE_USER_APPLICATION_DATA: return 50;
        default: break;
    }

    return 0;
}

ReadCommandLineResult ApplicationDelegate::ReadCommandLineSettings(CommandLineArgsProcessor& argsProcessor, Error& error)
{
    return ReadCommandLineResult::SUCCESS;
}

void ApplicationDelegate::GetCommandLineUsage(Utf8String& usage)
{
    usage.clear();
}

size_t ApplicationDelegate::GetBootFileNameCount() const
{
    return 1;
}

const Path& ApplicationDelegate::GetBootFileName(size_t index, bool& required) const
{
    static Path name("finjin-boot.cfg");
    required = false;
    return name;
}

bool ApplicationDelegate::ReadBootFileSection(ConfigDocumentReader& reader, const Utf8StringView& section, Error& error)
{
    return false;
}

bool ApplicationDelegate::ReadBootFileItem(const Utf8StringView& section, const Utf8StringView& key, const Utf8StringView& value, Error& error)
{
    return false;
}

size_t ApplicationDelegate::GetSettingsFileNameCount() const
{
    return 0;
}

const AssetReference& ApplicationDelegate::GetSettingsFileName(size_t index, bool& required) const
{
    required = false;
    return AssetReference::Empty();
}

void ApplicationDelegate::ReadSettings(size_t index, const ByteBuffer& settingsBuffer, Error& error)
{
}

void ApplicationDelegate::OnSoundDevicesEnumerated(const SoundAdapterDescriptions& devices)
{
}

void ApplicationDelegate::OnGpusEnumerated(const HardwareGpuDescriptions& hardwareGpus, const SoftwareGpuDescriptions& softwareGpus)
{
    auto viewportCount = GetApplicationViewportDescriptionCount();
    for (size_t viewportIndex = 0; viewportIndex < viewportCount; viewportIndex++)
    {
        auto& viewportDescription = GetApplicationViewportDescription(viewportIndex);
        if (viewportDescription.desiredGpuID.empty() ||
            !GetGpuDescription(viewportDescription.desiredGpuDescription, hardwareGpus, viewportDescription.desiredGpuID))
        {
            //No desired display adapter or the lookup of the desired display adapter failed

            viewportDescription.desiredGpuDescription = hardwareGpus[0]; //First is best
            viewportDescription.desiredGpuID = viewportDescription.desiredGpuDescription.GetGpuID();

            /*for (const auto& adapter : hardwareGpus)
            {
            int outputIndex = 0;
            for (const auto& output : adapter.outputs)
            {
            std::cout << "Output " << outputIndex++ << "--------------------------------" << std::endl;

            size_t modeIndex = 0;
            for (const auto& displayMode : output.displayModes)
            {
            std::cout << "Mode " << (modeIndex + 1) << ": " << displayMode.Width << " x " << displayMode.Height << " x " << (float)displayMode.RefreshRate.Numerator / displayMode.RefreshRate.Denominator << "hz" << std::endl;
            modeIndex++;
            }
            }
            }*/
        }

        viewportDescription.gpuID = viewportDescription.desiredGpuID;
    }
}

size_t ApplicationDelegate::GetApplicationViewportDescriptionCount() const
{
    return std::min(PlatformCapabilities::GetInstance().maxWindows, GetRequestedApplicationViewportDescriptionCount());
}

ApplicationViewport* ApplicationDelegate::CreateApplicationViewport(Allocator* allocator, size_t index)
{
    return AllocatedClass::New<ApplicationViewport>(allocator, FINJIN_CALLER_ARGUMENTS);
}

ApplicationViewportDelegate* ApplicationDelegate::CreateApplicationViewportDelegate(Allocator* allocator, size_t index)
{
    return AllocatedClass::New<ApplicationViewportDelegate>(allocator, FINJIN_CALLER_ARGUMENTS);
}
