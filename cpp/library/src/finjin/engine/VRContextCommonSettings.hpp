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
#include "finjin/engine/GenericGpuNumericStructs.hpp"
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
        enum class PrimitiveType
        {
            TRIANGLE_LIST
        };

        enum class IndexType
        {
            UINT16,
            UINT32
        };

        enum class TexturePixelFormat
        {
            RGBA8_UNORM
        };

        VRDeviceRenderModelPointer()
        {
            this->vertexData = nullptr;
            this->vertexCount = 0;

            this->vertexElements = nullptr;
            this->vertexElementCount = 0;

            this->indexData = nullptr;
            this->indexType = IndexType::UINT16;

            this->primitiveType = PrimitiveType::TRIANGLE_LIST;
            this->primitiveCount = 0;

            this->textureWidth = this->textureHeight = 0;
            this->textureImage = nullptr;
            this->texturePixelFormat = TexturePixelFormat::RGBA8_UNORM;

            this->textureID = 0;
        }

        const void* vertexData; //Most likely position/normal/uv
        uint32_t vertexCount;

        GpuInputFormatStruct::Element* vertexElements;
        size_t vertexElementCount;

        const void* indexData; //Contains 3 * primitiveCount elements for primitiveType = PrimitiveType::TRIANGLE_LIST
        IndexType indexType;

        PrimitiveType primitiveType;
        uint32_t primitiveCount;

        uint32_t textureWidth;
        uint32_t textureHeight;
        const void* textureImage;
        TexturePixelFormat texturePixelFormat;

        uint32_t textureID;
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
