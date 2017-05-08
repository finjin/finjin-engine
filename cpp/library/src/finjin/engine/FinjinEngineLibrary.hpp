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
#include "finjin/common/FinjinCommonLibrary.hpp"


//Macros------------------------------------------------------------------------

//Engine name
#define FINJIN_ENGINE_NAME "Finjin Engine"

//Engine version
#define FINJIN_ENGINE_VERSION_MAJOR 0
#define FINJIN_ENGINE_VERSION_MINOR 3
#define FINJIN_ENGINE_VERSION_PATCH 5

//GPU system
#define FINJIN_TARGET_GPU_SYSTEM_D3D12 1
#define FINJIN_TARGET_GPU_SYSTEM_VULKAN 2
#define FINJIN_TARGET_GPU_SYSTEM_METAL 3

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #if _WIN32_WINNT == 0x0A00 && !FINJIN_TARGET_GPU_SYSTEM_VULKAN_FORCED
        //Windows 10 target
        #define FINJIN_TARGET_GPU_SYSTEM FINJIN_TARGET_GPU_SYSTEM_D3D12
    #elif _WIN32_WINNT == 0x0603 || _WIN32_WINNT == 0x0602 || _WIN32_WINNT == 0x0601 || FINJIN_TARGET_GPU_SYSTEM_VULKAN_FORCED
        //Windows 8.1/8/7 target
        #define FINJIN_TARGET_GPU_SYSTEM FINJIN_TARGET_GPU_SYSTEM_VULKAN
    #else
        #error Unexpected Windows target!
    #endif
#elif FINJIN_TARGET_PLATFORM_IS_APPLE
    #define FINJIN_TARGET_GPU_SYSTEM FINJIN_TARGET_GPU_SYSTEM_METAL
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    #define FINJIN_TARGET_GPU_SYSTEM FINJIN_TARGET_GPU_SYSTEM_VULKAN
#else
    #error No GPU system!
#endif

//Sound system
#define FINJIN_TARGET_SOUND_SYSTEM_XAUDIO2 1
#define FINJIN_TARGET_SOUND_SYSTEM_OPENAL 2
#define FINJIN_TARGET_SOUND_SYSTEM_AVAUDIOENGINE 3
#define FINJIN_TARGET_SOUND_SYSTEM_OPENSL 4

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS
    #define FINJIN_TARGET_SOUND_SYSTEM FINJIN_TARGET_SOUND_SYSTEM_XAUDIO2
#elif FINJIN_TARGET_PLATFORM_IS_APPLE
    #define FINJIN_TARGET_SOUND_SYSTEM FINJIN_TARGET_SOUND_SYSTEM_AVAUDIOENGINE
#elif FINJIN_TARGET_PLATFORM_IS_ANDROID
    #define FINJIN_TARGET_SOUND_SYSTEM FINJIN_TARGET_SOUND_SYSTEM_OPENSL
#elif FINJIN_TARGET_PLATFORM_IS_LINUX
    #define FINJIN_TARGET_SOUND_SYSTEM FINJIN_TARGET_SOUND_SYSTEM_OPENAL
#else
    #error No sound system!
#endif

//Virtual reality system
#define FINJIN_TARGET_VR_SYSTEM_NONE 0
#define FINJIN_TARGET_VR_SYSTEM_OPENVR 1

#if FINJIN_TARGET_PLATFORM_IS_WINDOWS && !FINJIN_TARGET_PLATFORM_IS_WINDOWS_UWP
    #define FINJIN_TARGET_VR_SYSTEM FINJIN_TARGET_VR_SYSTEM_NONE //FINJIN_TARGET_VR_SYSTEM_OPENVR
#else
    #define FINJIN_TARGET_VR_SYSTEM FINJIN_TARGET_VR_SYSTEM_NONE
#endif

//Utilities
#define FINJIN_USE_ENGINE_NAMESPACES \
    using namespace Finjin::Common; \
    using namespace Finjin::Engine;


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    struct EngineConstants
    {
        enum { MAX_WINDOWS = 16 };
        enum { MAX_DISPLAYS = MAX_WINDOWS };
        enum { MAX_GPU_ADAPTERS = 5 };
        enum { MAX_SOUND_ADAPTERS = 5 };

        enum { MIN_FRAME_BUFFERS = 2 };
        enum { MAX_FRAME_BUFFERS = 3 };

        enum { MAX_FRAME_STAGES = MAX_FRAME_BUFFERS - 1 };

        enum { MAX_WINDOW_LISTENERS = 5 }; //5 is usually enough (at most 1 for each of the application, gpu, input, sound, and VR systems)

        enum { MAX_FILE_SYSTEMS = 5 }; //Different file systems within the application. Must be at least ApplicationFileSystem::COUNT
        enum { MAX_ASSET_ROOT_DIRECTORIES = 4 }; //The maximum number of root directories used by the application. Typically "engine" and "app"
        enum { MAX_ASSET_CLASS_DIRECTORIES = 8 }; //The maximum number of matching directories for a particular asset class

        enum { MAX_CONTEXT_SETTINGS_FILES = 8 }; //The maximum number of settings files that can be configured for a subsystem (gpu, input, sound, VR) context

        enum { DEFAULT_CONFIGURATION_BUFFER_SIZE = 50000 }; //Buffer size for reading configuration files
        enum { DEFAULT_ASSET_BUFFER_SIZE = 10000000 }; //Buffer size for reading asset files
        enum { DEFAULT_SOUND_CONTEXT_READ_BUFFER_SIZE = 10000000 }; //Buffer size for reading sound context resources (config files, sound files, etc)
        enum { DEFAULT_GPU_CONTEXT_READ_BUFFER_SIZE = 10000000 }; //Buffer size for reading gpu context resources (config files, textures, meshes, etc)

        enum { MAX_MATCHING_ASSET_NAMES = 128 }; //Maximum number of files or subdirectories that can potentially match an AssetPathSelector

        enum { MAX_EXTERNAL_ASSET_FILE_EXTENSIONS = 4 }; //"External" asset file extensions, such as "wav", "dds", "png", etc.

        enum { DEFAULT_SUBSYSTEM_COMMANDS_PER_UPDATE = 1000 };

        enum { MAX_RENDER_TARGET_VIEWPORTS = 4 }; //Maximum number of viewports and scissor rectangles within a render target
        enum { MAX_RENDER_TARGET_OUTPUTS = 8 }; //Maximum number of outputs a render target can have. The actual "true" maximum may depend on the 3D subsystem
        enum { MAX_RENDER_TARGET_DEPENDENCIES = 4 }; //Maximum number of other render targets a render target can depend on
        enum { MAX_RENDER_TARGET_BARRIERS = MAX_RENDER_TARGET_OUTPUTS + MAX_RENDER_TARGET_DEPENDENCIES * MAX_RENDER_TARGET_OUTPUTS };

        enum { MAX_LIGHTS_PER_OBJECT = 4 }; //A limit of 4 allows 4 indices to be packed into a shader uint4
    };

} }
