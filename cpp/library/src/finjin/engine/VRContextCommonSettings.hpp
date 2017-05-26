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
#include "finjin/engine/AssetFileReader.hpp"
#include "finjin/engine/ContextEventInfo.hpp"
#include "finjin/engine/PerFrameObjectAllocator.hpp"
#include <boost/thread/null_mutex.hpp>


//Macros------------------------------------------------------------------------
#define FINJIN_VR_CONTEXT_DEVICE_CALLBACKS(CONTEXT_TYPE) \
    std::function<void(CONTEXT_TYPE* vrContext, size_t vrDeviceIndex)> addDeviceHandler;\
    std::function<void(CONTEXT_TYPE* vrContext, size_t vrDeviceIndex)> removeDeviceHandler;\
    std::function<void(CONTEXT_TYPE* vrContext, size_t vrDeviceIndex)> deviceRenderModelLoadedHandler;


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    struct VREvent
    {
        enum class Type
        {
            NONE
        };

        VREvent()
        {
            this->type = Type::NONE;
            this->count = 0;
        }

        Type type;

        union
        {
            size_t count;
            void* soundBuffer;
            void* soundSource;
        };

        ContextEventInfo eventInfo;
    };

    class VREvents : public DynamicVector<VREvent>
    {
    public:
        using Super = DynamicVector<VREvent>;
    };

    struct VRCommand
    {
        enum class Type
        {
            NOTIFY,

            APPLY_HAPTIC_FEEDBACK
        };

        VRCommand()
        {
            this->type = Type::NOTIFY;
            this->next = nullptr;
            this->count = 0;
        }

        Type type;
        VRCommand* next;

        union
        {
            size_t count;
        };

        ContextEventInfo eventInfo;
    };

    class VRCommands : public PerFrameObjectAllocator<VRCommand, boost::null_mutex>
    {
    public:
        using Super = PerFrameObjectAllocator<VRCommand, boost::null_mutex>;

        using iterator = Super::iterator;
        using const_iterator = Super::const_iterator;

        const_iterator begin() const { return this->commands.begin(); }
        iterator begin() { return this->commands.begin(); }

        const_iterator end() const { return this->commands.end(); }
        iterator end() { return this->commands.end(); }
    };

    struct VRDeviceRenderModelPointer
    {
        VRDeviceRenderModelPointer()
        {
            this->vertex_positionNormalTex = nullptr;
            this->vertexCount = 0;

            this->indexData = nullptr;
            this->triangleCount = 0;

            this->textureWidth = this->textureHeight = 0;
            this->rgbaTextureData = nullptr;
        }

        const void* vertex_positionNormalTex;
        uint32_t vertexCount;

        const uint16_t* indexData;
        uint32_t triangleCount; //Index count is 3 * TriangleCount

        uint16_t textureWidth, textureHeight;
        const uint8_t* rgbaTextureData;
    };

    struct VRContextCommonSettings
    {
        VRContextCommonSettings(Allocator* initialAllocator);

        void HandleTopLevelSetting(const Utf8StringView& key, const Utf8StringView& value, Error& error);

        AssetFileReader* assetReader;
        AssetPathSelector initialAssetSelector;
        AssetReferences<EngineConstants::MAX_CONTEXT_SETTINGS_FILES> settingsFileNames; //Usually "vr-context.cfg"

        bool loadStandardRenderModels; //Indicates whether the standard 3D models should be loaded (if such a thing is supported)

        size_t maxCommandsPerUpdate;
    };

} }
