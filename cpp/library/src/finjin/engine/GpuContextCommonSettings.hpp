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
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/NumericStruct.hpp"
#include "finjin/common/RequestedValue.hpp"
#include "finjin/engine/AssetClassFileReader.hpp"
#include "finjin/engine/Camera.hpp"
#include "finjin/engine/ContextEventInfo.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/GpuID.hpp"
#include "finjin/engine/GpuRenderTargetSize.hpp"
#include "finjin/engine/PerFrameObjectAllocator.hpp"
#include "finjin/engine/RenderStatus.hpp"
#include "finjin/engine/ResourcePoolDescription.hpp"
#include "finjin/engine/ScreenCapture.hpp"
#include <boost/thread/null_mutex.hpp>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class OSWindow;

    struct GpuEvent
    {
        enum class Type
        {
            NOTIFY,

            CREATE_MESH_FINISHED,
            CREATE_TEXTURE_FINISHED,
            CREATE_MATERIAL_FINISHED,

            SET_CLEAR_COLOR_FINISHED,
            SET_CAMERA_FINISHED,

            RENDER_ENTITY_FINISHED,

            CAPTURE_SCREEN_REQUESTED
        };

        GpuEvent()
        {
            this->type = Type::NOTIFY;
            this->count = 0;
            this->index = 0;
            this->resource = nullptr;
        }

        Type type;

        size_t count;
        size_t index;
        union
        {
            void* resource;
            void* meshResource;
            void* shaderResource;
            void* textureResource;
            void* materialResource;
            void* lightResource;
        };

        ContextEventInfo eventInfo;
    };

    class GpuEvents : public DynamicVector<GpuEvent>
    {
    public:
        using Super = DynamicVector<GpuEvent>;
    };

    struct GpuCommand
    {
        enum class Type
        {
            NOTIFY,

            START_GRAPHICS_COMMANDS,
            FINISH_GRAPHICS_COMMANDS,
            EXECUTE_GRAPHICS_COMMANDS,

            START_RENDER_TARGET,
            FINISH_RENDER_TARGET,

            CREATE_MESH,
            CREATE_TEXTURE,
            CREATE_MATERIAL,

            SET_CLEAR_COLOR,
            SET_CAMERA,

            RENDER_ENTITY,

            CAPTURE_SCREEN
        };

        GpuCommand()
        {
            this->type = Type::NOTIFY;
            this->next = nullptr;
        }

        Type type;
        GpuCommand* next;
        ContextEventInfo eventInfo;
    };

    struct NotifyGpuCommand : public GpuCommand
    {
        NotifyGpuCommand()
        {
            this->type = Type::NOTIFY;
        }
    };

    struct StartGraphicsCommandsGpuCommand : public GpuCommand
    {
        StartGraphicsCommandsGpuCommand()
        {
            this->type = Type::START_GRAPHICS_COMMANDS;
        }
    };

    struct FinishGraphicsCommandsGpuCommand : public GpuCommand
    {
        FinishGraphicsCommandsGpuCommand()
        {
            this->type = Type::FINISH_GRAPHICS_COMMANDS;
        }
    };

    struct ExecuteGraphicsCommandsGpuCommand : public GpuCommand
    {
        ExecuteGraphicsCommandsGpuCommand()
        {
            this->type = Type::EXECUTE_GRAPHICS_COMMANDS;
        }
    };

    struct StartRenderTargetGpuCommand : public GpuCommand
    {
        StartRenderTargetGpuCommand()
        {
            this->type = Type::START_RENDER_TARGET;
        }

        Utf8String renderTargetName;
        StaticVector<Utf8String, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES> dependentRenderTargetNames;
    };

    struct FinishRenderTargetGpuCommand : public GpuCommand
    {
        FinishRenderTargetGpuCommand()
        {
            this->type = Type::FINISH_RENDER_TARGET;
        }
    };

    template <typename T, GpuCommand::Type commandType>
    struct UploadAssetGpuCommand : public GpuCommand
    {
        UploadAssetGpuCommand()
        {
            this->type = commandType;
            this->asset = nullptr;
        }

        T* asset;
    };

    using CreateMeshGpuCommand = UploadAssetGpuCommand<FinjinMesh, GpuCommand::Type::CREATE_MESH>;
    using CreateTextureGpuCommand = UploadAssetGpuCommand<FinjinTexture, GpuCommand::Type::CREATE_TEXTURE>;
    using CreateMaterialGpuCommand = UploadAssetGpuCommand<FinjinMaterial, GpuCommand::Type::CREATE_MATERIAL>;

    struct SetClearColorGpuCommand : public GpuCommand
    {
        SetClearColorGpuCommand()
        {
            this->type = Type::SET_CLEAR_COLOR;
        }

        MathVector4 clearColor;
    };

    struct SetCameraGpuCommand : public GpuCommand
    {
        SetCameraGpuCommand()
        {
            this->type = Type::SET_CAMERA;
        }

        Camera camera;
    };

    struct GpuCommandLight
    {
        GpuCommandLight()
        {
            this->light = nullptr;
            this->state = nullptr;
        }

        GpuCommandLight(FinjinSceneObjectLight* light, LightState* state)
        {
            this->light = light;
            this->state = state;
        }

        FinjinSceneObjectLight* light;
        LightState* state;
    };
    using GpuCommandLights = StaticVector<GpuCommandLight, EngineConstants::MAX_LIGHTS_PER_OBJECT>;

    struct RenderEntityGpuCommand : public GpuCommand
    {
        RenderEntityGpuCommand()
        {
            this->type = Type::RENDER_ENTITY;
            this->entity = nullptr;
            this->ambientLight = MathVector4::Zero();
            this->shaderFeatureFlags = RenderShaderFeatureFlags::GetDefault();
        }

        SceneNodeState sceneNodeState;
        FinjinSceneObjectEntity* entity;
        GpuCommandLights sortedLights;
        MathVector4 ambientLight;
        RenderShaderFeatureFlags shaderFeatureFlags;
    };

    struct CaptureScreenGpuCommand : public GpuCommand
    {
        CaptureScreenGpuCommand()
        {
            this->type = Type::CAPTURE_SCREEN;
        }
    };

    class GpuCommands : public PerFrameObjectAllocator<GpuCommand, boost::null_mutex>
    {
    public:
        using Super = PerFrameObjectAllocator<GpuCommand, boost::null_mutex>;

        using iterator = Super::iterator;
        using const_iterator = Super::const_iterator;

        const_iterator begin() const { return this->commands.begin(); }
        iterator begin() { return this->commands.begin(); }

        const_iterator end() const { return this->commands.end(); }
        iterator end() { return this->commands.end(); }

        bool CreateNotify(const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<NotifyGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool StartGraphicsCommands(const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<StartGraphicsCommandsGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool FinishGraphicsCommands(const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<FinishGraphicsCommandsGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool ExecuteGraphicsCommands(const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<ExecuteGraphicsCommandsGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool StartRenderTarget(const Utf8String* name, StaticVector<Utf8String, EngineConstants::MAX_RENDER_TARGET_DEPENDENCIES>* dependentRenderTargets, const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<StartRenderTargetGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            if (name != nullptr && !name->empty())
            {
                command->renderTargetName.SetAllocator(this);
                command->renderTargetName = *name;
            }

            if (dependentRenderTargets != nullptr)
            {
                command->dependentRenderTargetNames.resize(dependentRenderTargets->size());
                for (size_t dependentRenderTargetIndex = 0; dependentRenderTargetIndex < dependentRenderTargets->size(); dependentRenderTargetIndex++)
                {
                    command->dependentRenderTargetNames[dependentRenderTargetIndex].SetAllocator(this);
                    command->dependentRenderTargetNames[dependentRenderTargetIndex] = (*dependentRenderTargets)[dependentRenderTargetIndex];
                }
            }
            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool FinishRenderTarget(const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<FinishRenderTargetGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool CreateMesh(FinjinMesh* asset, const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<CreateMeshGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->asset = asset;
            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool CreateTexture(FinjinTexture* asset, const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<CreateTextureGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->asset = asset;
            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool CreateMaterial(FinjinMaterial* asset, const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<CreateMaterialGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->asset = asset;
            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool SetClearColor(const MathVector4& color, const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<SetClearColorGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->clearColor = color;
            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool SetCamera(const Camera& camera, const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<SetCameraGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->camera = camera;
            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool RenderEntity(SceneNodeState& sceneNodeState, FinjinSceneObjectEntity* entity, const RenderShaderFeatureFlags& shaderFeatureFlags, const GpuCommandLights& sortedLights, const MathVector4& ambientLight, const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<RenderEntityGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->sceneNodeState = sceneNodeState;
            command->entity = entity;
            command->shaderFeatureFlags = shaderFeatureFlags;
            command->sortedLights = sortedLights;
            command->ambientLight = ambientLight;
            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }

        bool CaptureScreen(const ContextEventInfo& eventInfo = ContextEventInfo::GetEmpty())
        {
            auto command = NewObject<CaptureScreenGpuCommand>(FINJIN_CALLER_ARGUMENTS);
            if (command == nullptr)
                return false;

            command->eventInfo = eventInfo;

            this->commands.push_back(command);

            return true;
        }
    };

    enum class GpuFrameDestination
    {
        SWAP_CHAIN = 1 << 0,
        VR_CONTEXT = 1 << 1
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(GpuFrameDestination)

    enum class GpuSwapChainPresentMode
    {
        FULL_VSYNC,
        ADAPTIVE_VSYNC,
        IMMEDIATE
    };

    enum class GpuIndexFormat
    {
        UINT16_INDEX, //16-bit index
        UINT32_INDEX //32-bit index
    };

    enum class GpuAssetTextureFormats
    {
        NONE = 0,

        ASTC = 1 << 0,
        ETC2 = 1 << 1,
        BC = 1 << 2,

        ALL = ASTC | ETC2 | BC
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(GpuAssetTextureFormats)

    struct GpuContextCommonSettings
    {
        GpuContextCommonSettings(Allocator* initialAllocator);

        static size_t CalculateJobPipelineSize(size_t actualFrameCount);

        void ReadSettings(AssetClassFileReader& settingsAssetReader, const AssetReference& settingsFileAssetRef, ByteBuffer& readBuffer, Error& error);
        virtual void ParseSettings(const ByteBufferReader& configFileBuffer, const Path& assetFilePath, Error& error);

        void HandleTopLevelSetting(const Utf8StringView& key, const Utf8StringView& value, Error& error);

        void* applicationHandle;
        OSWindow* osWindow;
        GpuFrameDestination frameDestination; //The default is sufficient
        void* vrContext; //Must be set if frameDestination has GpuFrameDestination::VR_CONTEXT flag
        GpuRenderTargetSize renderTargetSize;
        AssetFileReader* assetFileReader;
        AssetPathSelector initialAssetFileSelector;
        AssetReferences<EngineConstants::MAX_CONTEXT_SETTINGS_FILES> contextSettingsFileNames; //Usually "gpu-context.cfg"
        AssetReferences<EngineConstants::MAX_CONTEXT_SETTINGS_FILES> staticMeshRendererSettingsFileNames; //Usually "gpu-context-static-mesh-renderer.cfg"
        GpuAssetTextureFormats availableAssetTextureFormats; //Application-provided formats, beyond PNG

        size_t tempReadBufferSize;

        size_t maxCommandsPerUpdate; //Maximum number of high level commands
        size_t maxMaterials;
        size_t max2DTextures;
        size_t maxCubeTextures;
        size_t max3DTextures;
        size_t maxShaders;
        size_t maxMeshes;
        size_t maxLights;

        //RequestedValue types will be set in GpuContext during initialization

        GpuID gpuID;
        RequestedValue<size_t> frameBufferCount;  //Number of frames, including the one displayed. 2 = double buffer, 3 = triple buffer.
        size_t jobProcessingPipelineSize; //Will be set by the GpuContext during initialization based on frameBufferCount. Will usually be frameBufferCount.actual - 1
        size_t presentSyncInterval; //Will be set by the GpuContext during initialization
        size_t maxGpuCommandListsPerStage; //Maximum number of D3D/Vulkan/Metal command lists. The default is sufficient
        float maxDepthValue; //The default is sufficient
        double renderingScale; //The default is sufficient                
        GpuSwapChainPresentMode swapChainPresentMode; //The default is sufficient
        RequestedValue<int> multisampleCount; //Usually 1, 2, 4, 8
        RequestedValue<int> multisampleQuality; //Usually 0
        RequestedValue<ScreenCaptureFrequency> screenCaptureFrequency; //How often to capture the screen/frame buffer, if possible. By default, never
    };

    template <typename Result, typename Descriptions, typename GpuID>
    bool GetGpuDescription(Result& selected, const Descriptions& descriptions, const GpuID& gpuID)
    {
        auto found = descriptions.GetByGpuID(gpuID);
        if (found != nullptr)
        {
            selected = *found;
            return true;
        }
        else
            return false;
    }

} }
