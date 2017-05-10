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
#include "finjin/engine/GpuContextCommonSettings.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Convert.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
GpuContextCommonSettings::GpuContextCommonSettings(Allocator* allocator) : initialAssetFileSelector(allocator), contextSettingsFileNames(allocator), staticMeshRendererSettingsFileNames(allocator)
{
    this->applicationHandle = nullptr;
    this->osWindow = nullptr;
    this->assetFileReader = nullptr;
    this->availableAssetTextureFormats = GpuAssetTextureFormats::NONE;

    this->tempReadBufferSize = EngineConstants::DEFAULT_GPU_CONTEXT_READ_BUFFER_SIZE;

    this->maxCommandsPerUpdate = EngineConstants::DEFAULT_SUBSYSTEM_COMMANDS_PER_UPDATE;
    this->maxMaterials = 100;
    this->max2DTextures = 100;
    this->maxCubeTextures = 100;
    this->max3DTextures = 100;
    this->maxShaders = 100;
    this->maxMeshes = 100;
    this->maxLights = 100;

    this->frameBufferCount.requested = 2;
    this->frameBufferCount.actual = 0;
    this->jobProcessingPipelineSize = 0;
    this->presentSyncInterval = 1;
    this->maxGpuCommandListsPerStage = 3;
    this->maxDepthValue = 1.0f;
    this->renderingScale = 1;
    this->presentMode = GpuSwapChainPresentMode::ADAPTIVE_VSYNC;
    this->multisampleCount.requested = 1;
    this->multisampleCount.actual = 1;
    this->multisampleQuality.requested = 0;
    this->multisampleQuality.actual = 0;
}

size_t GpuContextCommonSettings::CalculateJobPipelineSize(size_t actualFrameCount)
{
    return actualFrameCount - 1;
}

void GpuContextCommonSettings::ReadSettings(AssetClassFileReader& settingsAssetReader, const AssetReference& settingsFileAssetRef, ByteBuffer& readBuffer, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto result = settingsAssetReader.ReadAsset(readBuffer, settingsFileAssetRef);
    if (result == FileOperationResult::SUCCESS)
    {
        ParseSettings(readBuffer, settingsFileAssetRef.filePath, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to process GPU settings file.");
            return;
        }
    }
    else if (result != FileOperationResult::NOT_FOUND)
    {
        FINJIN_SET_ERROR(error, "Failed to read GPU settings file.");
        return;
    }
}

void GpuContextCommonSettings::ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Utf8StringView section, key, value;

    ConfigDocumentReader reader;
    if (reader.Start(configFileBuffer) != nullptr)
    {
        for (auto line = reader.Current(); line != nullptr; line = reader.Next())
        {
            switch (line->GetType())
            {
                case ConfigDocumentLine::Type::SECTION:
                {
                    line->GetSectionName(section);

                    break;
                }
                case ConfigDocumentLine::Type::KEY_AND_VALUE:
                {
                    line->GetKeyAndValue(key, value);

                    HandleTopLevelSetting(key, value, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR_NO_MESSAGE(error);
                        return;
                    }

                    break;
                }
                default: break;
            }
        }
    }
}

void GpuContextCommonSettings::HandleTopLevelSetting(const Utf8StringView& key, const Utf8StringView& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (key == "texture-asset-formats")
    {
        Split(value, ' ', [this](Utf8StringView& value)
        {
            if (value == "astc")
                this->availableAssetTextureFormats |= GpuAssetTextureFormats::ASTC;
            else if (value == "etc2")
                this->availableAssetTextureFormats |= GpuAssetTextureFormats::ETC2;
            else if (value == "bc")
                this->availableAssetTextureFormats |= GpuAssetTextureFormats::BC;
            return ValueOrError<bool>(true);
        });
    }
    if (key == "temp-read-buffer-size")
        this->tempReadBufferSize = Convert::ToInteger(value, this->tempReadBufferSize);
    else if (key == "max-commands-per-update")
        this->maxCommandsPerUpdate = Convert::ToInteger(value, this->maxCommandsPerUpdate);
    else if (key == "max-materials")
        this->maxMaterials = Convert::ToInteger(value, this->maxMaterials);
    else if (key == "max-2d-textures")
        this->max2DTextures = Convert::ToInteger(value, this->max2DTextures);
    else if (key == "max-cube-textures")
        this->maxCubeTextures = Convert::ToInteger(value, this->maxCubeTextures);
    else if (key == "max-3d-textures")
        this->max3DTextures = Convert::ToInteger(value, this->max3DTextures);
    else if (key == "max-shaders")
        this->maxShaders = Convert::ToInteger(value, this->maxShaders);
    else if (key == "max-meshes")
        this->maxMeshes = Convert::ToInteger(value, this->maxMeshes);
    else if (key == "max-lights")
        this->maxLights = Convert::ToInteger(value, this->maxLights);
}
