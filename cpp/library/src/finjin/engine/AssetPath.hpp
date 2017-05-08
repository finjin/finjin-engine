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
#include "finjin/common/ByteOrder.hpp"
#include "finjin/common/DebugLog.hpp"
#include "finjin/common/EnumArray.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/LayoutDirection.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/engine/AssetClass.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class AssetPathComponent
    {
        NONE,

        ASSET_CLASS_DIRECTORY_NAME, //Asset class plural/directory name. Will usually be something like "shaders", "sounds", etc...
        LANGUAGE, //en, fr, etc...
        COUNTRY, //country/region. Example: us, gb, fr, ca, etc.... When parsing, language and region are specified together as: language_country. Example: en_us, en_gb, etc...
        LAYOUT_DIRECTION, //ldltr, ldrtl
        SMALLEST_WIDTH, //sw<width>dp
        WIDTH, //w<width>dp
        HEIGHT, //h<height>dp
        SCREEN_SIZE, //small, normal, large, xlarge
        SCREEN_ORIENTATION, //port, land
        SCREEN_DENSITY, //ldpi, mdpi, hdpi, xhdpi, nodpi, tvdpi, anydpi
        CPU_ARCHITECTURE, //cpu_<arch>. Example: cpu_x86, cpu_x64, cpu_arm32, cpu_arm64, etc...
        CPU_BYTE_ORDER, //bigendian, littleendian
        OPERATING_SYSTEM, //windows, macos, ios, tvos, carplay, linux, android ...
        OPERATING_SYSTEM_VERSION, //v_1_0, v_14, etc...
        APPLICATION_PLATFORM, //win32, uwp
        GPU_SYSTEM, //d3d12, vulkan, metal
        GPU_FEATURE_LEVEL, //gpufeature_<level>. Example: gpufeature_10_1, etc...
        GPU_SHADER_MODEL, //gpusm_<version>. Example: gpusm_5_1, gpusm_nv_5_1, etc...
        GPU_PREFERRED_TEXTURE_FORMAT, //astc, etc2, bc
        GPU_MODEL, //gpu_<model>. Example: gpu_nvidiagtx980, etc...
        SOUND_SYSTEM, //xaudio2, avaudioengine, opensles, openal
        INPUT_DEVICE_TYPE, //gamecontroller, keyboard, mouse, touchscreen, accelerometer
        INPUT_SYSTEM, //win32input, uwpinput, iosinput, macosinput, linuxinput
        INPUT_API, //xinput, dinput, gcinput
        INPUT_DEVICE_DESCRIPTOR, //inputdevice_<product descriptor>. Example: inputdevice_4c05c405_0000_0000_0000_504944564944, inputdevice_vid2341_pid1000
        VR_SYSTEM, //win32vr, linuxvr
        VR_API, //openvr
        DEVICE_MODEL, //The computer on which this application is running. device_<model>. Example: device_nexus6, device_ipad, etc...

        COUNT
    };

    class AssetPath
    {
        friend class AssetPathSelector;

    public:
        AssetPath& operator = (const AssetPath& other);

        bool Create(Allocator* allocator);

        void Create
            (
            AssetClass assetClass,
            const Path& fullName, //Can be a simple name like "shaders" or a file path like "app/shaders-d3d12"
            size_t nameOffset, //Offset to the part that contains the actual name of interest. In the case of fullName="app/shaders-d3d12", offset would be 4
            Error& error
            );

        const Path& GetFullName() const;

        Utf8String ToString() const;

    private:
        void ProcessComponent(const Utf8StringView& componentString, AssetPathComponent& lastType);

    private:
        Path fullName;

        struct Component
        {
            Utf8StringView value;

            bool empty() const { return this->value.empty(); }
            void clear() { this->value.clear(); }
        };
        EnumArray<AssetPathComponent, AssetPathComponent::COUNT, Component> components;
    };

    class AssetPathSelector
    {
    public:
        AssetPathSelector(Allocator* allocator = nullptr);

        bool Create(Allocator* allocator);

        const Utf8String& Get(AssetPathComponent type) const;
        void Set(AssetPathComponent type, const Utf8String& value);
        void Set(AssetPathComponent type, ByteOrder value);
        void Set(AssetPathComponent type, LayoutDirection value);
        void SetAssetClassDirectoryName(AssetClass assetClass);
        void SetCpuArchitecture();

        template <typename... Args>
        void Set(const AssetPathSelector& other, Args... args)
        {
            for (auto arg : { args... })
                this->components[arg].value = other.components[arg].value;
        }

        Utf8String ToString() const;
        ValueOrError<void> ToString(Utf8String& result) const;

        bool Accepts(const AssetPath& name) const;

        const AssetPath* SelectBest(const AssetPath* names, size_t count) const;

        template <typename Bits>
        const AssetPath* SelectBest(const AssetPath* names, size_t count, Bits invalidNames, size_t invalidNameCount) const
        {
            assert(count <= invalidNames.size());
            count = std::min(invalidNames.size(), count);

            auto validNameCount = count - invalidNameCount;

            //Disqualify all the names that don't have matching selector components
            for (size_t componentIndex = 0; componentIndex < this->components.size(); componentIndex++)
            {
                auto& component = this->components[componentIndex];
                if (!component.empty())
                {
                    //Selector specified the component

                    for (size_t otherNameIndex = 0; otherNameIndex < count; otherNameIndex++)
                    {
                        auto& otherComponent = names[otherNameIndex].components[componentIndex];
                        if (!otherComponent.empty() && component != otherComponent)
                        {
                            //Both the selector and the name have values for the component and they do not match

                            //Mark the name as invalid if necessary
                            if (!invalidNames.IsBitSet(otherNameIndex))
                            {
                                //FINJIN_DEBUG_LOG_INFO("Marking name as invalid: %1%", names[otherNameIndex].GetFullName());

                                invalidNames.SetBit(otherNameIndex);
                                validNameCount--;
                            }
                        }
                    }
                }
            }

            //For each component in the selector, if that component is specified in a name, eliminate all other names that don't have that component
            for (size_t componentIndex = 0; componentIndex < this->components.size() && validNameCount > 0; componentIndex++)
            {
                auto& component = this->components[componentIndex];
                if (!component.empty())
                {
                    //Selector specified the component

                    //FINJIN_DEBUG_LOG_INFO("Iterating on component selector: %1%", component.value);

                    //Determine whether any other valid name contains the component
                    bool anyNameHasComponent = false;
                    for (size_t otherNameIndex = 0; otherNameIndex < count; otherNameIndex++)
                    {
                        if (!invalidNames.IsBitSet(otherNameIndex))
                        {
                            auto& otherComponent = names[otherNameIndex].components[componentIndex];
                            if (!otherComponent.empty() && component == otherComponent)
                            {
                                //FINJIN_DEBUG_LOG_INFO("Some name component has selector: %1%", component.value);

                                anyNameHasComponent = true;
                                break;
                            }
                        }
                    }

                    //If any name has the component, eliminate the ones that do not
                    if (anyNameHasComponent)
                    {
                        for (size_t otherNameIndex = 0; otherNameIndex < count; otherNameIndex++)
                        {
                            if (!invalidNames.IsBitSet(otherNameIndex))
                            {
                                auto& otherComponent = names[otherNameIndex].components[componentIndex];
                                if (component != otherComponent)
                                {
                                    //FINJIN_DEBUG_LOG_INFO("%1% != %2%", component.value, otherComponent.value.ToString());

                                    invalidNames.SetBit(otherNameIndex);
                                    validNameCount--;
                                }
                                else
                                {
                                    //FINJIN_DEBUG_LOG_INFO("%1% == %2%", component.value, otherComponent.value.ToString());
                                }
                            }
                        }
                    }
                }
            }

            //FINJIN_DEBUG_LOG_INFO("Valid names remaining: %1%", validNameCount);

            //Ideally at this point validNameCount == 1 will occur, meaning there was one best name
            //However, validNameCount > 1 can occur if the selector was underspecified, or there were duplicates in 'names'
            if (validNameCount > 0)
            {
                //Return the first name that isn't invalid
                for (size_t otherNameIndex = 0; otherNameIndex < count; otherNameIndex++)
                {
                    if (!invalidNames.IsBitSet(otherNameIndex))
                        return &names[otherNameIndex];
                }

                //This should not happen
                assert(0 && "Unexpectedly failed to find a valid name.");
            }

            return nullptr;
        }

    private:
        void SetPreformatted(AssetPathComponent type, const Utf8String& value);
        void FormatAndSetSize(AssetPathComponent type, const char* prefix, const Utf8String& value);

        void SetSafeLowerAscii(AssetPathComponent type, const char* prefix, const Utf8String& value);
        void SetSafeLowerAlphaNumeric(AssetPathComponent type, const char* prefix, const Utf8String& value);

    private:
        struct Component
        {
            Utf8String value;

            bool operator == (const AssetPath::Component& other) const { return other.value == this->value; }
            bool operator != (const AssetPath::Component& other) const { return other.value != this->value; }
            bool empty() const { return this->value.empty(); }
        };
        EnumArray<AssetPathComponent, AssetPathComponent::COUNT, Component> components;
    };

} }
