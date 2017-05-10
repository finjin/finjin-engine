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
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/ConfigDocumentWriter.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/engine/AssetClassFileReader.hpp"
#include "finjin/engine/InputComponents.hpp"
#include "finjin/engine/InputSource.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    template <typename InputDevice>
    class InputDeviceSerializer
    {
    public:
        InputDeviceSerializer(InputDevice& _inputDevice) : inputDevice(_inputDevice)
        {
        }

        void Read(const ByteBuffer& byteBuffer, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            Read(reinterpret_cast<const char*>(byteBuffer.data()), byteBuffer.size(), error);
            if (error)
                FINJIN_SET_ERROR_NO_MESSAGE(error);
        }

        void Read(const char* text, size_t length, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            ConfigDocumentReader reader;

            Utf8StringView sectionName, key, value;

            ConfigValues configValues;

            auto line = reader.Start(text, length);
            while (line != nullptr)
            {
                switch (line->GetType())
                {
                    case ConfigDocumentLine::Type::SECTION:
                    {
                        CommitValues(sectionName, configValues);

                        line->GetSectionName(sectionName);

                        break;
                    }
                    case ConfigDocumentLine::Type::KEY_AND_VALUE:
                    {
                        line->GetKey(key);
                        line->GetValue(value);
                        if (key == "index")
                            configValues.index = value;
                        else if (key == "code")
                            configValues.code = value;
                        else if (key == "enabled")
                            configValues.enabled = value;
                        else if (key == "display-name")
                            configValues.displayName = value;
                        else if (key == "rest-value")
                            configValues.restValue = value;
                        else if (key == "semantic")
                            configValues.semantic = value;
                        else if (key == "magnitude")
                            configValues.magnitude = value;
                        else if (key == "direction")
                            configValues.direction = value;
                        break;
                    }
                    default: break;
                }

                line = reader.Next();
            }

            CommitValues(sectionName, configValues);
        }

    private:
        struct ConfigValues
        {
            //All the values that can be encountered within a configuration section

            Utf8StringView index;
            Utf8StringView code;
            Utf8StringView displayName;
            Utf8StringView enabled;
            Utf8StringView restValue;
            Utf8StringView semantic;
            Utf8StringView magnitude;
            Utf8StringView direction;

            void Reset()
            {
                this->index.clear();
                this->code.clear();
                this->displayName.clear();
                this->enabled.clear();
                this->restValue.clear();
                this->semantic.clear();
                this->magnitude.clear();
                this->direction.clear();
            }
        };

        void CommitValues(Utf8StringView& sectionName, ConfigValues& configValues)
        {
            bool handled = false;

            if (sectionName.empty())
            {
                //Try to configure device
                if (!handled)
                {
                    if (!configValues.displayName.empty())
                        this->inputDevice.SetDisplayName(configValues.displayName);
                    if (!configValues.semantic.empty())
                        this->inputDevice.SetSemantic(InputDeviceSemanticUtilities::Parse(configValues.semantic));

                    handled = true;
                }
            }
            else
            {
                //Try to configure a component by code
                if (!handled && !configValues.code.empty())
                {
                    auto code = Convert::ToInteger(configValues.code, (int)0);

                    if (sectionName == "key" || sectionName == "button")
                    {
                        auto button = GetButtonByCode(&this->inputDevice, code);
                        if (button != nullptr)
                        {
                            CommitButtonValues(button, configValues);
                            handled = true;
                        }
                    }
                    else if (sectionName == "axis")
                    {
                        auto axis = GetAxisByCode(&this->inputDevice, code);
                        if (axis != nullptr)
                        {
                            CommitAxisValues(axis, configValues);
                            handled = true;
                        }
                    }
                    else if (sectionName == "pov")
                    {
                        auto pov = GetPovByCode(&this->inputDevice, code);
                        if (pov != nullptr)
                        {
                            CommitPovValues(pov, configValues);
                            handled = true;
                        }
                    }
                    else if (sectionName == "locator")
                    {
                        auto locator = GetLocatorByCode(&this->inputDevice, code);
                        if (locator != nullptr)
                        {
                            CommitLocatorValues(locator, configValues);
                            handled = true;
                        }
                    }
                }

                //Try to configure a component by index
                if (!handled && !configValues.index.empty())
                {
                    auto index = Convert::ToInteger(configValues.index, (size_t)0);

                    if (sectionName == "key" || sectionName == "button")
                    {
                        if (index < this->inputDevice.GetButtonCount())
                        {
                            CommitButtonValues(this->inputDevice.GetButton(index), configValues);
                            handled = true;
                        }
                    }
                    else if (sectionName == "axis")
                    {
                        if (index < this->inputDevice.GetAxisCount())
                        {
                            CommitAxisValues(this->inputDevice.GetAxis(index), configValues);
                            handled = true;
                        }
                    }
                    else if (sectionName == "pov")
                    {
                        if (index < this->inputDevice.GetPovCount())
                        {
                            CommitPovValues(this->inputDevice.GetPov(index), configValues);
                            handled = true;
                        }
                    }
                    else if (sectionName == "locator")
                    {
                        if (index < this->inputDevice.GetLocatorCount())
                        {
                            CommitLocatorValues(this->inputDevice.GetLocator(index), configValues);
                            handled = true;
                        }
                    }
                }
            }

            sectionName.clear();
            configValues.Reset();
        }

        template <typename ButtonType>
        void CommitButtonValues(ButtonType* button, ConfigValues& configValues)
        {
            if (!configValues.displayName.empty())
                button->SetDisplayName(configValues.displayName);
            if (!configValues.semantic.empty())
                button->SetSemantic(InputComponentSemanticUtilities::Parse(configValues.semantic));
            if (!configValues.enabled.empty())
                button->Enable(Convert::ToBool(configValues.enabled));
        }

        template <typename AxisType>
        void CommitAxisValues(AxisType* axis, ConfigValues& configValues)
        {
            if (!configValues.displayName.empty())
                axis->SetDisplayName(configValues.displayName);
            if (!configValues.restValue.empty())
                axis->SetRestValue(Convert::ToNumber(configValues.restValue, 0.0f));
            if (!configValues.semantic.empty())
                axis->SetSemantic(InputComponentSemanticUtilities::Parse(configValues.semantic));
            if (!configValues.magnitude.empty())
            {
                auto magnitude = Convert::ToNumber(configValues.magnitude, 0.0f);
                axis->SetMinMax(-magnitude, magnitude);
            }
            if (!configValues.direction.empty())
                axis->SetDirection(Convert::ToNumber(configValues.direction, 1.0f));
            if (!configValues.enabled.empty())
                axis->Enable(Convert::ToBool(configValues.enabled));
        }

        template <typename PovType>
        void CommitPovValues(PovType* pov, ConfigValues& configValues)
        {
            if (!configValues.displayName.empty())
                pov->SetDisplayName(configValues.displayName);
            if (!configValues.semantic.empty())
                pov->SetSemantic(InputComponentSemanticUtilities::Parse(configValues.semantic));
            if (!configValues.enabled.empty())
                pov->Enable(Convert::ToBool(configValues.enabled));
        }

        template <typename LocatorType>
        void CommitLocatorValues(LocatorType* locator, ConfigValues& configValues)
        {
            if (!configValues.displayName.empty())
                locator->SetDisplayName(configValues.displayName);
            if (!configValues.semantic.empty())
                locator->SetSemantic(InputComponentSemanticUtilities::Parse(configValues.semantic));
            if (!configValues.enabled.empty())
                locator->Enable(Convert::ToBool(configValues.enabled));
        }

    private:
        InputDevice& inputDevice;
    };

    enum class ConfigureInputDeviceResult
    {
        SUCCESS,
        CONFIG_FILE_READ_FAILED,
        CONFIG_FILE_PARSE_FAILED
    };

} }


//Functions---------------------------------------------------------------------
namespace Finjin { namespace Engine {

    template <typename Device>
    ConfigureInputDeviceResult ConfigureInputDevice
        (
        Device& device,
        const Utf8String& productDescriptor,
        AssetClassFileReader& assetReader,
        ByteBuffer& configFileBuffer
        )
    {
        //Compose asset reference
        auto configFileName = productDescriptor;
        configFileName += ".cfg";
        AssetReference configFileAssetRef(FINJIN_ALLOCATOR_NULL);
        configFileAssetRef.ForLocalFile(configFileName);

        //Attempt to read the file
        if (assetReader.ReadAsset(configFileBuffer, configFileAssetRef) != FileOperationResult::SUCCESS)
            return ConfigureInputDeviceResult::CONFIG_FILE_READ_FAILED;

        //Attempt to deserialize the file into the device
        InputDeviceSerializer<Device> serializer(device);

        FINJIN_DECLARE_ERROR(error);
        serializer.Read(configFileBuffer, error);
        if (error)
            return ConfigureInputDeviceResult::CONFIG_FILE_PARSE_FAILED;

        return ConfigureInputDeviceResult::SUCCESS;
    }

} }
