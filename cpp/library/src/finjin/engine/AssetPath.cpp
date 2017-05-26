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
#include "AssetPath.hpp"
#include "finjin/common/BitArray.hpp"
#include "finjin/common/DebugLog.hpp"

using namespace Finjin::Engine;


//Local functions---------------------------------------------------------------
static bool GetLanguageAndCountry(const Utf8StringView& componentString, Utf8StringView& language, Utf8StringView& region)
{
    auto underscoreIndex = componentString.find('_');
    if (underscoreIndex != Utf8String::npos)
    {
        language = componentString.substr(0, underscoreIndex);
        region = componentString.substr(underscoreIndex + 1);
    }
    else
        language = componentString;

    return language.length() == 2;
}

static Utf8String GetCpuArchitectureInternalString()
{
    Utf8String s;

#if FINJIN_TARGET_CPU == FINJIN_TARGET_CPU_ARM
    //CPU
    s += "arm";

    //Bits
    #if FINJIN_TARGET_CPU_BITS == 32
        s += "32";
    #elif FINJIN_TARGET_CPU_BITS == 64
        s += "64";
    #else
        #error Invalid bits!
    #endif
#elif FINJIN_TARGET_CPU == FINJIN_TARGET_CPU_INTEL
    //CPU
    #if FINJIN_TARGET_CPU_BITS == 32
        s = "x86";
    #elif FINJIN_TARGET_CPU_BITS == 64
        s = "x64";
    #else
        #error Invalid bits!
    #endif
#else
    #error Invalid CPU!
#endif

    return s;
}

static bool IsLayoutDirection(const Utf8StringView& componentString)
{
    return
        componentString == "ldltr" ||
        componentString == "ldrtl"
        ;
}

static bool IsSize(const Utf8StringView& componentString, const char* prefix, const char* suffix)
{
    auto prefixLength = strlen(prefix);
    if (!componentString.StartsWith(prefix, prefixLength))
        return false;
    auto suffixLength = strlen(suffix);
    if (!componentString.EndsWith(suffix, suffixLength))
        return false;
    for (size_t i = prefixLength; i < componentString.length() - suffixLength; i++)
    {
        if (!isdigit(componentString[i]))
            return false;
    }

    return true;
}

static bool IsSmallestWidth(const Utf8StringView& componentString)
{
    return IsSize(componentString, "sw", "dp");
}

static bool IsWidth(const Utf8StringView& componentString)
{
    return IsSize(componentString, "w", "dp");
}

static bool IsHeight(const Utf8StringView& componentString)
{
    return IsSize(componentString, "h", "dp");
}

static bool IsScreenSize(const Utf8StringView& componentString)
{
    return
        componentString == "small" ||
        componentString == "normal" ||
        componentString == "large" ||
        componentString == "xlarge"
        ;
}

static bool IsScreenOrientation(const Utf8StringView& componentString)
{
    return
        componentString == "land" ||
        componentString == "port"
        ;
}

static bool IsScreenDensity(const Utf8StringView& componentString)
{
    return
        componentString == "ldpi" ||
        componentString == "mdpi" ||
        componentString == "hdpi" ||
        componentString == "xhdpi" ||
        componentString == "nodpi" ||
        componentString == "tvdpi" ||
        componentString == "anydpi"
        ;
}

static bool IsCpuArchitecture(const Utf8StringView& componentString)
{
    return componentString.StartsWith("cpu_");
}

static bool IsCpuByteOrder(const Utf8StringView& componentString)
{
    return
        componentString == "bigendian" ||
        componentString == "littleendian"
        ;
}

static bool IsOperatingSystem(const Utf8StringView& componentString)
{
    return
        componentString == "windows" ||
        componentString == "macos" ||
        componentString == "ios" ||
        componentString == "tvos" ||
        componentString == "carplay" ||
        componentString == "android" ||
        componentString == "linux"
        ;
}

static bool IsOperatingSystemVersion(const Utf8StringView& componentString)
{
    return
        componentString.length() > 2 &&
        componentString[0] == 'v' &&
        componentString[1] == '_'
        ;
}

static bool IsApplicationPlatform(const Utf8StringView& componentString)
{
    return
        componentString == "win32" ||
        componentString == "uwp"
        ;
}

static bool IsDeviceModel(const Utf8StringView& componentString)
{
    return componentString.StartsWith("device_");
}

static bool IsGpuManufacturerModel(const Utf8StringView& componentString)
{
    return componentString.StartsWith("gpu_");
}

static bool IsGpuSystem(const Utf8StringView& componentString)
{
    return
        componentString == "d3d12" ||
        componentString == "metal" ||
        componentString == "vulkan"
        ;
}

static bool IsGpuFeatureLevel(const Utf8StringView& componentString)
{
    return componentString.StartsWith("gpufeature_");
}

static bool IsGpuShaderModel(const Utf8StringView& componentString)
{
    return componentString.StartsWith("gpusm_");
}

static bool IsPreferredTextureFormat(const Utf8StringView& componentString)
{
    return
        componentString == "astc" ||
        componentString == "etc2" ||
        componentString == "bc"
        ;
}

static bool IsSoundSystem(const Utf8StringView& componentString)
{
    return
        componentString == "xaudio2" ||
        componentString == "avaudioengine" ||
        componentString == "opensles" ||
        componentString == "openal"
        ;
}

static bool IsInputDevice(const Utf8StringView& componentString)
{
    return
        componentString == "gamecontroller" ||
        componentString == "keyboard" ||
        componentString == "mouse" ||
        componentString == "touchscreen" ||
        componentString == "accelerometer"
        ;
}

static bool IsInputDeviceDescriptor(const Utf8StringView& componentString)
{
    return componentString.StartsWith("inputdevice_");
}

static bool IsInputSystem(const Utf8StringView& componentString)
{
    return
        componentString == "win32input" ||
        componentString == "uwpinput" ||
        componentString == "iosinput" ||
        componentString == "macosinput" ||
        componentString == "linuxinput"
        ;
}

static bool IsInputApi(const Utf8StringView& componentString)
{
    return
        componentString == "xinput" ||
        componentString == "dinput" ||
        componentString == "gcinput"
        ;
}

static bool IsVRSystem(const Utf8StringView& componentString)
{
    return
        componentString == "win32vr" ||
        componentString == "linuxvr"
        ;
}

static bool IsVRApi(const Utf8StringView& componentString)
{
    return componentString == "openvr";
}


//Implementation----------------------------------------------------------------

//AssetPath
AssetPath& AssetPath::operator = (const AssetPath& other)
{
    this->fullName = other.fullName;

    //Copy all the components manually, taking care to update the pointers to reference this->fullName
    for (size_t componentIndex = 0; componentIndex < this->components.size(); componentIndex++)
    {
        auto& component = this->components[componentIndex];
        auto& otherComponent = other.components[componentIndex];

        if (!otherComponent.value.empty())
            component.value.assign(this->fullName.begin() + (otherComponent.value.begin() - other.fullName.begin()), otherComponent.value.length());
        else
            component.clear();
    }

    return *this;
}

bool AssetPath::Create(Allocator* allocator)
{
    return this->fullName.Create(allocator);
}

void AssetPath::Create(AssetClass assetClass, const Path& _fullName, size_t offset, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto prefix = AssetClassUtilities::ToDirectoryName(assetClass);

    for (auto& component : this->components)
        component.clear();

    if (this->fullName.assign(_fullName).HasError())
    {
        FINJIN_SET_ERROR(error, "Failed to assign full name.");
        return;
    }

    if (!prefix.empty())
    {
        if (this->components[AssetPathComponent::ASSET_CLASS_DIRECTORY_NAME].value.assign(this->fullName.begin() + offset, prefix.length()).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to assign asset class directory name component.");
            return;
        }

        offset += prefix.length();

        if (offset < this->fullName.length() && this->fullName[offset] == '-')
            offset++;
    }

    auto lastType = AssetPathComponent::ASSET_CLASS_DIRECTORY_NAME; //Assume the last type was an asset class directory name even if there wasn't one
    Utf8StringView componentString;
    while (offset < this->fullName.length())
    {
        auto hyphenOffset = this->fullName.find("-", offset);

        componentString.assign(this->fullName.begin() + offset, std::min(hyphenOffset - offset, this->fullName.length() - offset));

        ProcessComponent(componentString, lastType);

        offset = hyphenOffset;
        if (offset != Utf8String::npos)
            offset++;
    }
}

void AssetPath::ProcessComponent(const Utf8StringView& componentString, AssetPathComponent& lastType)
{
    Utf8StringView language, region;
    if (lastType < AssetPathComponent::COUNTRY && GetLanguageAndCountry(componentString, language, region))
    {
        lastType = AssetPathComponent::COUNTRY;
        this->components[AssetPathComponent::LANGUAGE].value = language;
        this->components[AssetPathComponent::COUNTRY].value = region;
    }
    else if (lastType < AssetPathComponent::LAYOUT_DIRECTION && IsLayoutDirection(componentString))
    {
        lastType = AssetPathComponent::LAYOUT_DIRECTION;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::SMALLEST_WIDTH && IsSmallestWidth(componentString))
    {
        lastType = AssetPathComponent::SMALLEST_WIDTH;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::WIDTH && IsWidth(componentString))
    {
        lastType = AssetPathComponent::WIDTH;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::HEIGHT && IsHeight(componentString))
    {
        lastType = AssetPathComponent::HEIGHT;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::SCREEN_SIZE && IsScreenSize(componentString))
    {
        lastType = AssetPathComponent::SCREEN_SIZE;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::SCREEN_ORIENTATION && IsScreenOrientation(componentString))
    {
        lastType = AssetPathComponent::SCREEN_ORIENTATION;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::SCREEN_DENSITY && IsScreenDensity(componentString))
    {
        lastType = AssetPathComponent::SCREEN_DENSITY;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::CPU_ARCHITECTURE && IsCpuArchitecture(componentString))
    {
        lastType = AssetPathComponent::CPU_ARCHITECTURE;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::CPU_BYTE_ORDER && IsCpuByteOrder(componentString))
    {
        lastType = AssetPathComponent::CPU_BYTE_ORDER;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::OPERATING_SYSTEM && IsOperatingSystem(componentString))
    {
        lastType = AssetPathComponent::OPERATING_SYSTEM;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::OPERATING_SYSTEM_VERSION && IsOperatingSystemVersion(componentString))
    {
        lastType = AssetPathComponent::OPERATING_SYSTEM_VERSION;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::APPLICATION_PLATFORM && IsApplicationPlatform(componentString))
    {
        lastType = AssetPathComponent::APPLICATION_PLATFORM;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::GPU_SYSTEM && IsGpuSystem(componentString))
    {
        lastType = AssetPathComponent::GPU_SYSTEM;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::GPU_FEATURE_LEVEL && IsGpuFeatureLevel(componentString))
    {
        lastType = AssetPathComponent::GPU_FEATURE_LEVEL;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::GPU_SHADER_MODEL && IsGpuShaderModel(componentString))
    {
        lastType = AssetPathComponent::GPU_SHADER_MODEL;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT && IsPreferredTextureFormat(componentString))
    {
        lastType = AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::GPU_MODEL && IsGpuManufacturerModel(componentString))
    {
        lastType = AssetPathComponent::GPU_MODEL;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::SOUND_SYSTEM && IsSoundSystem(componentString))
    {
        lastType = AssetPathComponent::SOUND_SYSTEM;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::INPUT_DEVICE_TYPE && IsInputDevice(componentString))
    {
        lastType = AssetPathComponent::INPUT_DEVICE_TYPE;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::INPUT_DEVICE_DESCRIPTOR && IsInputDeviceDescriptor(componentString))
    {
        lastType = AssetPathComponent::INPUT_DEVICE_DESCRIPTOR;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::INPUT_SYSTEM && IsInputSystem(componentString))
    {
        lastType = AssetPathComponent::INPUT_SYSTEM;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::INPUT_API && IsInputApi(componentString))
    {
        lastType = AssetPathComponent::INPUT_API;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::VR_SYSTEM && IsVRSystem(componentString))
    {
        lastType = AssetPathComponent::VR_SYSTEM;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::VR_API && IsVRApi(componentString))
    {
        lastType = AssetPathComponent::VR_API;
        this->components[lastType].value = componentString;
    }
    else if (lastType < AssetPathComponent::DEVICE_MODEL && IsDeviceModel(componentString))
    {
        lastType = AssetPathComponent::DEVICE_MODEL;
        this->components[lastType].value = componentString;
    }
}

const Path& AssetPath::GetFullName() const
{
    return this->fullName;
}

Utf8String AssetPath::ToString() const
{
    Utf8String result;

    for (auto& component : this->components)
    {
        if (!component.empty())
        {
            if (!result.empty())
                result += "-";
            result += component.value;
        }
    }

    return result;
}

//AssetPathSelector
AssetPathSelector::AssetPathSelector(Allocator* allocator)
{
    if (allocator != nullptr)
        Create(allocator);
}

bool AssetPathSelector::Create(Allocator* allocator)
{
    auto result = true;

    for (auto& component : this->components)
        result &= component.value.Create(allocator);

    return result;
}

const Utf8String& AssetPathSelector::Get(AssetPathComponent type) const
{
    auto& component = this->components[type];
    return component.value;
}

void AssetPathSelector::Set(AssetPathComponent type, const Utf8String& value)
{
    switch (type)
    {
        case AssetPathComponent::ASSET_CLASS_DIRECTORY_NAME: SetPreformatted(type, value); break;
        case AssetPathComponent::LANGUAGE: SetSafeLowerAscii(type, "", value); break;
        case AssetPathComponent::COUNTRY: SetSafeLowerAscii(type, "", value); break;
        case AssetPathComponent::LAYOUT_DIRECTION: SetPreformatted(type, value); break;
        case AssetPathComponent::SMALLEST_WIDTH: FormatAndSetSize(type, "sw", value); break;
        case AssetPathComponent::WIDTH: FormatAndSetSize(type, "w", value); break;
        case AssetPathComponent::HEIGHT: FormatAndSetSize(type, "h", value); break;
        case AssetPathComponent::SCREEN_SIZE: SetPreformatted(type, value); break;
        case AssetPathComponent::SCREEN_ORIENTATION: SetPreformatted(type, value); break;
        case AssetPathComponent::SCREEN_DENSITY: SetPreformatted(type, value); break;
        case AssetPathComponent::CPU_ARCHITECTURE: SetSafeLowerAscii(type, "cpu_", value); break;
        case AssetPathComponent::CPU_BYTE_ORDER: SetPreformatted(type, value); break;
        case AssetPathComponent::OPERATING_SYSTEM: SetPreformatted(type, value); break;
        case AssetPathComponent::OPERATING_SYSTEM_VERSION: SetSafeLowerAscii(type, "v_", value); break;
        case AssetPathComponent::APPLICATION_PLATFORM: SetPreformatted(type, value); break;
        case AssetPathComponent::GPU_SYSTEM: SetPreformatted(type, value); break;
        case AssetPathComponent::GPU_FEATURE_LEVEL: SetSafeLowerAscii(type, "gpufeature_", value); break;
        case AssetPathComponent::GPU_SHADER_MODEL: SetSafeLowerAscii(type, "gpusm_", value); break;
        case AssetPathComponent::GPU_PREFERRED_TEXTURE_FORMAT: SetPreformatted(type, value); break; //SetSafeLowerAscii(type, "gputex_", value); break;
        case AssetPathComponent::GPU_MODEL: SetSafeLowerAscii(type, "gpu_", value); break;
        case AssetPathComponent::SOUND_SYSTEM: SetPreformatted(type, value); break;
        case AssetPathComponent::INPUT_DEVICE_TYPE: SetSafeLowerAlphaNumeric(type, "", value); break;
        case AssetPathComponent::INPUT_DEVICE_DESCRIPTOR: SetSafeLowerAscii(type, "inputdevice_", value); break;
        case AssetPathComponent::INPUT_SYSTEM: SetPreformatted(type, value); break;
        case AssetPathComponent::INPUT_API: SetPreformatted(type, value); break;
        case AssetPathComponent::VR_SYSTEM: SetPreformatted(type, value); break;
        case AssetPathComponent::VR_API: SetPreformatted(type, value); break;
        case AssetPathComponent::DEVICE_MODEL: SetSafeLowerAlphaNumeric(type, "device_", value); break;
        default: break;
    }
}

void AssetPathSelector::Set(AssetPathComponent type, ByteOrder value)
{
    switch (type)
    {
        case AssetPathComponent::CPU_BYTE_ORDER: SetPreformatted(type, value == ByteOrder::LITTLE ? "littleendian" : "bigendian"); break;
        default: break;
    }
}

void AssetPathSelector::Set(AssetPathComponent type, LayoutDirection value)
{
    switch (type)
    {
        case AssetPathComponent::LAYOUT_DIRECTION: SetPreformatted(type, value == LayoutDirection::LEFT_TO_RIGHT ? "ldltr" : "ldrtl"); break;
        default: break;
    }
}

void AssetPathSelector::SetAssetClassDirectoryName(AssetClass assetClass)
{
    auto& component = this->components[AssetPathComponent::ASSET_CLASS_DIRECTORY_NAME];
    AssetClassUtilities::ToDirectoryName(component.value, assetClass);
}

void AssetPathSelector::SetCpuArchitecture()
{
    Set(AssetPathComponent::CPU_ARCHITECTURE, GetCpuArchitectureInternalString());
}

void AssetPathSelector::SetCpuByteOrder()
{
    Set(AssetPathComponent::CPU_BYTE_ORDER, GetByteOrder());
}

void AssetPathSelector::SetPreformatted(AssetPathComponent type, const Utf8String& value)
{
    if (type == AssetPathComponent::NONE)
        return;

    auto& component = this->components[type];
    component.value = value;

    //FINJIN_DEBUG_LOG_INFO("preformatted: %1%", component.value);
}

void AssetPathSelector::FormatAndSetSize(AssetPathComponent type, const char* prefix, const Utf8String& value)
{
    if (type == AssetPathComponent::NONE)
        return;

    auto& component = this->components[type];
    if (value.empty())
        component.value.clear();
    else
    {
        component.value = prefix;
        component.value += value;
        if (!value.EndsWith("dp"))
            component.value += "dp";
    }
}

void AssetPathSelector::SetSafeLowerAscii(AssetPathComponent type, const char* prefix, const Utf8String& value)
{
    if (type == AssetPathComponent::NONE)
        return;

    auto& component = this->components[type];
    if (value.empty())
        component.value.clear();
    else
    {
        component.value = prefix;
        component.value += value;
        component.value.ToLowerAscii();
        component.value.RemoveWhitespace();
        component.value.ReplaceAll('.', '_');
        component.value.ReplaceAll('-', '_');
        component.value.ReplaceAll("(r)", "");
        component.value.ReplaceAll("(tm)", "");
    }

    //FINJIN_DEBUG_LOG_INFO("safe lower ascii: %1%", component.value);
}

void AssetPathSelector::SetSafeLowerAlphaNumeric(AssetPathComponent type, const char* prefix, const Utf8String& value)
{
    if (type == AssetPathComponent::NONE)
        return;

    auto& component = this->components[type];
    if (value.empty())
        component.value.clear();
    else
    {
        component.value = prefix;
        component.value += value;
        component.value.ReplaceAll("(r)", "");
        component.value.ReplaceAll("(R)", "");
        component.value.ToLowerAlphaNumeric();

        //FINJIN_DEBUG_LOG_INFO("safe lower alnum: %1%", component.value);
    }
}

Utf8String AssetPathSelector::ToString() const
{
    Utf8String result;
    ToString(result);
    return result;
}

ValueOrError<void> AssetPathSelector::ToString(Utf8String& result) const
{
    result.clear();

    for (auto& component : this->components)
    {
        if (!component.empty())
        {
            if (!result.empty())
            {
                if (result.append("-").HasError())
                    return ValueOrError<void>::CreateError();
            }
            if (result.append(component.value).HasError())
                return ValueOrError<void>::CreateError();
        }
    }

    return ValueOrError<void>();
}

bool AssetPathSelector::Accepts(const AssetPath& name) const
{
    return SelectBest(&name, 1) != nullptr;
}

const AssetPath* AssetPathSelector::SelectBest(const AssetPath* names, size_t count) const
{
    BitArray<EngineConstants::MAX_MATCHING_ASSET_NAMES> invalidNames;

    size_t invalidCount = 0;
    return SelectBest(names, count, invalidNames, invalidCount);
}
