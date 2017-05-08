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
#include "finjin/common/Convert.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/ConfigDocumentWriter.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#include "InputSource.hpp"
#include <ostream>


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    template <typename InputBindings, typename InputContext>
    class InputBindingsSerializer
    {
    public:
        InputBindingsSerializer(InputBindings& _inputBindings, InputContext* _inputContext = nullptr) : inputBindings(_inputBindings), inputContext(_inputContext)
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

            BindingValues bindingValues;
            MouseValues mouseValues;
            GameControllerValues gameControllerValues;

            auto line = reader.Start(text, length);
            while (line != nullptr)
            {
                switch (line->GetType())
                {
                    case ConfigDocumentLine::Type::SECTION:
                    {
                        CommitReadBindingValues(bindingValues);
                        CommitReadMouseValues(mouseValues);
                        CommitReadGameControllerValues(gameControllerValues);

                        line->GetSectionName(sectionName);

                        break;
                    }
                    case ConfigDocumentLine::Type::KEY_AND_VALUE:
                    {
                        line->GetKey(key);
                        line->GetValue(value);
                        if (sectionName == "binding")
                        {
                            if (key == "action")
                                bindingValues.actionName = value;
                            else if (key == "source")
                                bindingValues.sourceName = value;
                            else if (key == "code")
                                bindingValues.code = value;
                            else if (key == "index")
                                bindingValues.index = value;
                            else if (key == "direction")
                                bindingValues.direction = value;
                            else if (key == "touch-count")
                                bindingValues.touchCount = value;
                            else if (key == "device-index")
                                bindingValues.deviceIndex = value;
                            else if (key == "semantic")
                                bindingValues.semantic = value;

                            else if (key == "pressed")
                                bindingValues.pressed = value;
                            else if (key == "holding")
                                bindingValues.holding = value;
                            else if (key == "released")
                                bindingValues.released = value;
                            else if (key == "pov-weak")
                                bindingValues.povWeak = value;
                            else if (key == "pov-strong")
                                bindingValues.povStrong = value;
                            else if (key == "touch-allowed-with-multitouch")
                                bindingValues.touchAllowedWithMultitouch = value;
                            else if (key == "count")
                                bindingValues.count = value;

                            else if (key == "device-instance")
                                bindingValues.instanceDescriptor = value;
                            else if (key == "device-product")
                                bindingValues.productDescriptor = value;
                        }
                        else if (sectionName == "mouse")
                        {
                            if (key == "index")
                                mouseValues.index = value;
                            else if (key == "sensitivity")
                                mouseValues.sensitivity = value;
                        }
                        else if (sectionName == "game-controller")
                        {
                            if (key == "index")
                                gameControllerValues.index = value;
                            else if (key == "sensitivity")
                                gameControllerValues.sensitivity = value;
                            else if (key == "dead-zone")
                                gameControllerValues.deadZone = value;
                        }
                        break;
                    }
                    default: break;
                }

                line = reader.Next();
            }

            CommitReadBindingValues(bindingValues);
            CommitReadMouseValues(mouseValues);
            CommitReadGameControllerValues(gameControllerValues);
        }

        template <typename T> //T will be some type of std::ostream or DocumentWriterOutput
        void Write(T& out, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            ConfigDocumentWriter writer;
            writer.Create(out);

            //WriteSettings(writer);
            WriteBindings(writer);
        }

    private:
        struct BindingValues
        {
            Utf8StringView actionName;
            Utf8StringView sourceName;
            Utf8StringView code;
            Utf8StringView index;
            Utf8StringView direction;
            Utf8StringView touchCount;
            Utf8StringView deviceIndex;
            Utf8StringView semantic;

            Utf8StringView pressed;
            Utf8StringView holding;
            Utf8StringView released;
            Utf8StringView povWeak;
            Utf8StringView povStrong;
            Utf8StringView touchAllowedWithMultitouch;
            Utf8StringView count;

            Utf8StringView instanceDescriptor;
            Utf8StringView productDescriptor;

            void Reset()
            {
                this->actionName.clear();
                this->sourceName.clear();
                this->code.clear();
                this->index.clear();
                this->direction.clear();
                this->touchCount.clear();
                this->deviceIndex.clear();
                this->semantic.clear();

                this->pressed.clear();
                this->holding.clear();
                this->released.clear();
                this->povWeak.clear();
                this->povStrong.clear();
                this->touchAllowedWithMultitouch.clear();
                this->count.clear();

                this->instanceDescriptor.clear();
                this->productDescriptor.clear();
            }
        };

        struct MouseValues
        {
            Utf8StringView index;
            Utf8StringView sensitivity;

            void Reset()
            {
                this->index.clear();
                this->sensitivity.clear();
            }
        };

        struct GameControllerValues
        {
            Utf8StringView index;
            Utf8StringView sensitivity;
            Utf8StringView deadZone;

            void Reset()
            {
                this->index.clear();
                this->sensitivity.clear();
                this->deadZone.clear();
            }
        };

        void CommitReadBindingValues(BindingValues& bindingValues)
        {
            if (!bindingValues.actionName.empty())
            {
                const auto& actionInfo = this->inputBindings.GetActionInfoByName(bindingValues.actionName);
                if (!actionInfo.IsNull() && actionInfo.IsPublic())
                {
                    //Input source
                    InputSource inputSource;
                    ReadInputSource(bindingValues, inputSource);

                    //Trigger flags
                    auto triggerCriteria = ReadTriggerCriteria(bindingValues);

                    //Add the input binding
                    this->inputBindings.AddBinding(inputSource, actionInfo.action, triggerCriteria);
                }
            }

            bindingValues.Reset();
        }

        void CommitReadMouseValues(MouseValues& mouseValues)
        {
            if (!mouseValues.index.empty())
            {
                auto index = Convert::ToInteger(mouseValues.index.ToString(), (size_t)0);
                if (index < MouseConstants::MAX_AXIS_COUNT)
                    this->inputBindings.mouseAxisSensitivity[index] = Convert::ToNumber(mouseValues.sensitivity.ToString(), this->inputBindings.mouseAxisSensitivity[index]);
            }

            mouseValues.Reset();
        }

        void CommitReadGameControllerValues(GameControllerValues& gameControllerValues)
        {
            if (!gameControllerValues.index.empty())
            {
                auto index = Convert::ToInteger(gameControllerValues.index.ToString(), (size_t)0);
                if (index < GameControllerConstants::MAX_AXIS_COUNT)
                {
                    this->inputBindings.gameControllerAxisSensitivity[index] = Convert::ToNumber(gameControllerValues.sensitivity.ToString(), this->inputBindings.gameControllerAxisSensitivity[index]);
                    this->inputBindings.gameControllerAxisDeadZone[index] = Convert::ToNumber(gameControllerValues.deadZone.ToString(), this->inputBindings.gameControllerAxisDeadZone[index]);
                }
            }

            gameControllerValues.Reset();
        }

        enum class DeviceIndexReadResult
        {
            FOUND_INSTANCE_DESCRIPTOR,
            FOUND_PRODUCT_DESCRIPTOR,
            USED_DEVICE_INDEX,
            NOT_FOUND
        };
        DeviceIndexReadResult ReadDeviceIndex(BindingValues& bindingValues, InputSource& inputSource)
        {
            if (this->inputContext != nullptr && (!bindingValues.instanceDescriptor.empty() || !bindingValues.productDescriptor.empty()))
            {
                auto deviceClass = InputDeviceComponentUtilities::GetDeviceClass(inputSource.deviceComponent); //inputSource.deviceComponent has been set at this point, so this is safe
                auto deviceClassCount = this->inputContext->GetDeviceCount(deviceClass);

                if (!bindingValues.instanceDescriptor.empty())
                {
                    //Try to find the matching instance descriptor
                    for (size_t i = 0; i < deviceClassCount; i++)
                    {
                        if (bindingValues.instanceDescriptor == this->inputContext->GetDeviceInstanceDescriptor(deviceClass, i))
                        {
                            inputSource.deviceIndex = i;
                            return DeviceIndexReadResult::FOUND_INSTANCE_DESCRIPTOR;
                        }
                    }
                }

                if (!bindingValues.productDescriptor.empty())
                {
                    //Failed to find instance descriptor, so try to find matching product descriptor
                    for (size_t i = 0; i < deviceClassCount; i++)
                    {
                        if (bindingValues.productDescriptor == this->inputContext->GetDeviceProductDescriptor(deviceClass, i))
                        {
                            inputSource.deviceIndex = i;
                            return DeviceIndexReadResult::FOUND_PRODUCT_DESCRIPTOR;
                        }
                    }
                }
            }

            if (!bindingValues.deviceIndex.empty())
            {
                inputSource.deviceIndex = Convert::ToInteger(bindingValues.deviceIndex.ToString(), inputSource.deviceIndex);
                return DeviceIndexReadResult::USED_DEVICE_INDEX;
            }

            inputSource.deviceIndex = 0;
            return DeviceIndexReadResult::NOT_FOUND;
        }

        void ReadInputSource(BindingValues& bindingValues, InputSource& inputSource)
        {
            inputSource.deviceComponent = InputDeviceComponentUtilities::Parse(bindingValues.sourceName.ToString());

            switch (inputSource.deviceComponent)
            {
                case InputDeviceComponent::NONE:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    break;
                }

                case InputDeviceComponent::KEYBOARD_KEY:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.code = Convert::ToInteger(bindingValues.code.ToString(), inputSource.code);
                    inputSource.index = Convert::ToInteger(bindingValues.index.ToString(), inputSource.index);
                    break;
                }

                case InputDeviceComponent::MOUSE_BUTTON:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.code = Convert::ToInteger(bindingValues.code.ToString(), inputSource.code);
                    inputSource.index = Convert::ToInteger(bindingValues.index.ToString(), inputSource.index);
                    break;
                }
                case InputDeviceComponent::MOUSE_RELATIVE_AXIS:
                case InputDeviceComponent::MOUSE_ABSOLUTE_AXIS:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.code = Convert::ToInteger(bindingValues.code.ToString(), inputSource.code);
                    inputSource.index = Convert::ToInteger(bindingValues.index.ToString(), inputSource.index);
                    inputSource.direction = Convert::ToInteger(bindingValues.direction.ToString(), inputSource.direction);
                    break;
                }

                case InputDeviceComponent::GAME_CONTROLLER_BUTTON:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.code = Convert::ToInteger(bindingValues.code.ToString(), inputSource.code);
                    inputSource.index = Convert::ToInteger(bindingValues.index.ToString(), inputSource.index);
                    break;
                }
                case InputDeviceComponent::GAME_CONTROLLER_AXIS:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.code = Convert::ToInteger(bindingValues.code.ToString(), inputSource.code);
                    inputSource.index = Convert::ToInteger(bindingValues.index.ToString(), inputSource.index);
                    inputSource.direction = Convert::ToInteger(bindingValues.direction.ToString(), inputSource.direction);
                    break;
                }
                case InputDeviceComponent::GAME_CONTROLLER_POV:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.code = Convert::ToInteger(bindingValues.code.ToString(), inputSource.code);
                    inputSource.index = Convert::ToInteger(bindingValues.index.ToString(), inputSource.index);
                    inputSource.povDirection = PovDirectionUtilities::Parse(bindingValues.direction.ToString());
                    break;
                }
                case InputDeviceComponent::GAME_CONTROLLER_LOCATOR:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.code = Convert::ToInteger(bindingValues.code.ToString(), inputSource.code);
                    inputSource.index = Convert::ToInteger(bindingValues.index.ToString(), inputSource.index);
                    break;
                }

                case InputDeviceComponent::TOUCH_COUNT:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.touchCount = Convert::ToInteger(bindingValues.touchCount.ToString(), inputSource.touchCount);
                    break;
                }
                case InputDeviceComponent::TOUCH_RELATIVE_AXIS:
                case InputDeviceComponent::TOUCH_ABSOLUTE_AXIS:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.code = Convert::ToInteger(bindingValues.code.ToString(), inputSource.code);
                    inputSource.index = Convert::ToInteger(bindingValues.index.ToString(), inputSource.index);
                    inputSource.direction = Convert::ToInteger(bindingValues.direction.ToString(), inputSource.direction);
                    break;
                }

                case InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.direction = Convert::ToInteger(bindingValues.direction.ToString(), inputSource.direction);
                    inputSource.touchCount = Convert::ToInteger(bindingValues.touchCount.ToString(), inputSource.touchCount);
                    break;
                }
                case InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS:
                {
                    ReadDeviceIndex(bindingValues, inputSource);
                    inputSource.semantic = InputComponentSemanticUtilities::Parse(bindingValues.semantic.ToString());
                    inputSource.code = Convert::ToInteger(bindingValues.code.ToString(), inputSource.code);
                    inputSource.index = Convert::ToInteger(bindingValues.index.ToString(), inputSource.index);
                    inputSource.direction = Convert::ToInteger(bindingValues.direction.ToString(), inputSource.direction);
                    inputSource.touchCount = Convert::ToInteger(bindingValues.touchCount.ToString(), inputSource.touchCount);
                    break;
                }

                case InputDeviceComponent::ACCELEROMETER_RELATIVE_AXIS:
                {
                    break;
                }
                case InputDeviceComponent::ACCELEROMETER_ABSOLUTE_AXIS:
                {
                    break;
                }

                case InputDeviceComponent::HEADSET_LOCATOR:
                {
                    break;
                }

                default: break;
            }

            inputSource.directionFloat = (float)inputSource.direction;
        }

        static InputTriggerCriteria ReadTriggerCriteria(BindingValues& bindingValues)
        {
            InputTriggerCriteria triggerCriteria(InputTriggerFlag::NONE);
            if (!bindingValues.pressed.empty() && Convert::ToBool(bindingValues.pressed.ToString()))
                triggerCriteria.flags |= InputTriggerFlag::PRESSED;
            if (!bindingValues.holding.empty() && Convert::ToBool(bindingValues.holding.ToString()))
                triggerCriteria.flags |= InputTriggerFlag::HOLDING;
            if (!bindingValues.released.empty() && Convert::ToBool(bindingValues.released.ToString()))
                triggerCriteria.flags |= InputTriggerFlag::RELEASED;
            if (!bindingValues.povWeak.empty() && Convert::ToBool(bindingValues.povWeak.ToString()))
                triggerCriteria.flags |= InputTriggerFlag::POV_WEAK;
            if (!bindingValues.povStrong.empty() && Convert::ToBool(bindingValues.povStrong.ToString()))
                triggerCriteria.flags |= InputTriggerFlag::POV_STRONG;
            if (!bindingValues.touchAllowedWithMultitouch.empty() && Convert::ToBool(bindingValues.touchAllowedWithMultitouch.ToString()))
                triggerCriteria.flags |= InputTriggerFlag::TOUCH_ALLOWED_WITH_MULTITOUCH;
            if (!bindingValues.count.empty())
                triggerCriteria.count = Convert::ToInteger(bindingValues.count.ToString(), triggerCriteria.count);
            return triggerCriteria;
        }

        /*void WriteSettings(ConfigDocumentWriter& writer)
        {
            //Determine which settings have changed from their defaults
            auto modifiedMouseSensitivity = this->inputBindings.ModifiedMouseSensitivity();
            auto modifiedGameControllerAxisDeadZones = this->inputBindings.ModifiedGameControllerAxisDeadZones();
            auto modifiedGameControllerAxisSensitivities = this->inputBindings.ModifiedGameControllerAxisSensitivities();

            //Only save if some settings have changed
            if (modifiedMouseSensitivity || modifiedGameControllerAxisDeadZones || modifiedGameControllerAxisSensitivities)
            {
                //Mouse sensitivities
                if (modifiedMouseSensitivity)
                {
                    writer.WriteScopeStart("mouse");
                    for (size_t index = 0; index < MouseConstants::MAX_AXIS_COUNT; index++)
                    {
                        if (this->inputBindings.mouseAxisSensitivity[index] != this->inputBindings.defaultMouseAxisSensitivity[index])
                        {
                            //Axis index
                            writer.WriteKeyAndValue("index", Convert::ToString(index));

                            //Sensitivity
                            writer.WriteKeyAndValue("sensitivity", Convert::ToString(this->inputBindings.mouseAxisSensitivity[index]));
                        }
                    }
                    writer.WriteScopeEnd();
                    writer.WriteNewline();
                }

                //Game controller axis dead zones and sensitivities
                if (modifiedGameControllerAxisDeadZones || modifiedGameControllerAxisSensitivities)
                {
                    writer.WriteScopeStart("game-controller");

                    auto gameControllerAxisDeadZoneCount = this->inputBindings.gameControllerAxisDeadZone.size();
                    auto gameControllerAxisSensitivityCount = this->inputBindings.gameControllerAxisSensitivity.size();
                    auto maxAxisCount = std::max(gameControllerAxisDeadZoneCount, gameControllerAxisSensitivityCount);
                    for (size_t index = 0; index < maxAxisCount; index++)
                    {
                        if ((index < gameControllerAxisDeadZoneCount && this->inputBindings.gameControllerAxisDeadZone[index] != this->inputBindings.defaultGameControllerAxisDeadZone[index]) ||
                            (index < gameControllerAxisSensitivityCount && this->inputBindings.gameControllerAxisSensitivity[index] != this->inputBindings.defaultGameControllerAxisSensitivity[index]))
                        {
                            //Axis index
                            writer.WriteKeyAndValue("index", Convert::ToString(index));

                            //Sensitivity
                            if (index < gameControllerAxisSensitivityCount && this->inputBindings.gameControllerAxisSensitivity[index] != this->inputBindings.defaultGameControllerAxisSensitivity[index])
                                writer.WriteKeyAndValue("sensitivity", Convert::ToString(this->inputBindings.gameControllerAxisSensitivity[index]));

                            //Dead zone
                            if (index < gameControllerAxisDeadZoneCount && this->inputBindings.gameControllerAxisDeadZone[index] != this->inputBindings.defaultGameControllerAxisDeadZone[index])
                                writer.WriteKeyAndValue("dead-zone", Convert::ToString(this->inputBindings.gameControllerAxisDeadZone[index]));
                        }
                    }

                    writer.WriteScopeEnd();
                    writer.WriteNewline();
                }
            }
        }*/

        void WriteBindings(ConfigDocumentWriter& writer)
        {
            auto bindingCount = this->inputBindings.GetBindingCount();
            if (bindingCount > 0)
            {
                for (size_t bindingIndex = 0; bindingIndex < bindingCount; bindingIndex++)
                {
                    const auto& binding = this->inputBindings.GetBinding(bindingIndex);
                    const auto& actionInfo = this->inputBindings.GetActionInfoByID(binding.action);
                    if (actionInfo.IsPublic())
                    {
                        writer.WriteScopeStart("binding");

                        //Action name
                        writer.WriteKeyAndValue("action", actionInfo.name);

                        //Input source
                        WriteInputSource(writer, binding.inputSource);

                        //Trigger flags
                        WriteTriggerCriteria(writer, binding.triggerCriteria);

                        writer.WriteScopeEnd();
                        writer.WriteNewline();
                    }
                }
            }
        }

        void WriteDeviceIndex(ConfigDocumentWriter& writer, const InputSource& inputSource)
        {
            if (this->inputContext != nullptr)
            {
                auto deviceClass = InputDeviceComponentUtilities::GetDeviceClass(inputSource.deviceComponent);
                if (inputSource.deviceIndex < this->inputContext->GetDeviceCount(deviceClass))
                {
                    auto instanceDescriptor = this->inputContext->GetDeviceInstanceDescriptor(deviceClass, inputSource.deviceIndex);
                    auto productDescriptor = this->inputContext->GetDeviceProductDescriptor(deviceClass, inputSource.deviceIndex);

                    writer.WriteKeyAndValue("device-instance", instanceDescriptor);
                    writer.WriteKeyAndValue("device-product", productDescriptor);

                    //Done
                    return;
                }
            }

            writer.WriteKeyAndValue("device-index", Convert::ToString(inputSource.deviceIndex));
        }

        void WriteInputSource(ConfigDocumentWriter& writer, const InputSource& inputSource)
        {
            writer.WriteKeyAndValue("source", inputSource.GetDeviceComponentString());
            switch (inputSource.deviceComponent)
            {
                case InputDeviceComponent::KEYBOARD_KEY:
                {
                    WriteDeviceIndex(writer, inputSource);

                    if (inputSource.semantic != InputComponentSemantic::NONE)
                        WriteSemantic(writer, inputSource.semantic);
                    else if (inputSource.code != InputSource::NO_COMPONENT_CODE)
                        writer.WriteKeyAndValue("code", Convert::ToString(inputSource.code));
                    else if (inputSource.index != (size_t)-1)
                        writer.WriteKeyAndValue("index", Convert::ToString(inputSource.index));

                    break;
                }

                case InputDeviceComponent::MOUSE_BUTTON:
                {
                    WriteDeviceIndex(writer, inputSource);

                    if (inputSource.semantic != InputComponentSemantic::NONE)
                        WriteSemantic(writer, inputSource.semantic);
                    else if (inputSource.code != InputSource::NO_COMPONENT_CODE)
                        writer.WriteKeyAndValue("code", Convert::ToString(inputSource.code));
                    else if (inputSource.index != (size_t)-1)
                        writer.WriteKeyAndValue("index", Convert::ToString(inputSource.index));

                    break;
                }
                case InputDeviceComponent::MOUSE_RELATIVE_AXIS:
                case InputDeviceComponent::MOUSE_ABSOLUTE_AXIS:
                {
                    WriteDeviceIndex(writer, inputSource);

                    if (inputSource.semantic != InputComponentSemantic::NONE)
                        WriteSemantic(writer, inputSource.semantic);
                    else if (inputSource.code != InputSource::NO_COMPONENT_CODE)
                        writer.WriteKeyAndValue("code", Convert::ToString(inputSource.code));
                    else if (inputSource.index != (size_t)-1)
                        writer.WriteKeyAndValue("index", Convert::ToString(inputSource.index));

                    if (inputSource.direction != 0)
                        writer.WriteKeyAndValue("direction", Convert::ToString(inputSource.direction));

                    break;
                }

                case InputDeviceComponent::GAME_CONTROLLER_BUTTON:
                {
                    WriteDeviceIndex(writer, inputSource);

                    if (inputSource.semantic != InputComponentSemantic::NONE)
                        WriteSemantic(writer, inputSource.semantic);
                    else if (inputSource.code != InputSource::NO_COMPONENT_CODE)
                        writer.WriteKeyAndValue("code", Convert::ToString(inputSource.code));
                    else if (inputSource.index != (size_t)-1)
                        writer.WriteKeyAndValue("index", Convert::ToString(inputSource.index));

                    break;
                }
                case InputDeviceComponent::GAME_CONTROLLER_AXIS:
                {
                    WriteDeviceIndex(writer, inputSource);

                    if (inputSource.semantic != InputComponentSemantic::NONE)
                        WriteSemantic(writer, inputSource.semantic);
                    else if (inputSource.code != InputSource::NO_COMPONENT_CODE)
                        writer.WriteKeyAndValue("code", Convert::ToString(inputSource.code));
                    else if (inputSource.index != (size_t)-1)
                        writer.WriteKeyAndValue("index", Convert::ToString(inputSource.index));

                    if (inputSource.direction != 0)
                        writer.WriteKeyAndValue("direction", Convert::ToString(inputSource.direction));

                    break;
                }
                case InputDeviceComponent::GAME_CONTROLLER_POV:
                {
                    WriteDeviceIndex(writer, inputSource);

                    if (inputSource.semantic != InputComponentSemantic::NONE)
                        WriteSemantic(writer, inputSource.semantic);
                    else if (inputSource.code != InputSource::NO_COMPONENT_CODE)
                        writer.WriteKeyAndValue("code", Convert::ToString(inputSource.code));
                    else if (inputSource.index != (size_t)-1)
                        writer.WriteKeyAndValue("index", Convert::ToString(inputSource.index));

                    if (inputSource.direction != 0)
                        writer.WriteKeyAndValue("direction", PovDirectionUtilities::ToString(inputSource.povDirection));

                    break;
                }
                case InputDeviceComponent::GAME_CONTROLLER_LOCATOR:
                {
                    WriteDeviceIndex(writer, inputSource);

                    if (inputSource.semantic != InputComponentSemantic::NONE)
                        WriteSemantic(writer, inputSource.semantic);
                    else if (inputSource.code != InputSource::NO_COMPONENT_CODE)
                        writer.WriteKeyAndValue("code", Convert::ToString(inputSource.code));
                    else if (inputSource.index != (size_t)-1)
                        writer.WriteKeyAndValue("index", Convert::ToString(inputSource.index));

                    break;
                }

                case InputDeviceComponent::TOUCH_COUNT:
                {
                    WriteDeviceIndex(writer, inputSource);

                    WriteSemantic(writer, inputSource.semantic);

                    if (inputSource.touchCount != 0)
                        writer.WriteKeyAndValue("touch-count", Convert::ToString(inputSource.touchCount));

                    break;
                }
                case InputDeviceComponent::TOUCH_RELATIVE_AXIS:
                case InputDeviceComponent::TOUCH_ABSOLUTE_AXIS:
                {
                    WriteDeviceIndex(writer, inputSource);

                    if (inputSource.semantic != InputComponentSemantic::NONE)
                        WriteSemantic(writer, inputSource.semantic);
                    else if (inputSource.code != InputSource::NO_COMPONENT_CODE)
                        writer.WriteKeyAndValue("code", Convert::ToString(inputSource.code));
                    else if (inputSource.index != (size_t)-1)
                        writer.WriteKeyAndValue("index", Convert::ToString(inputSource.index));

                    if (inputSource.direction != 0)
                        writer.WriteKeyAndValue("direction", Convert::ToString(inputSource.direction));

                    break;
                }

                case InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS:
                {
                    WriteDeviceIndex(writer, inputSource);

                    WriteSemantic(writer, inputSource.semantic);

                    if (inputSource.direction != 0)
                        writer.WriteKeyAndValue("direction", Convert::ToString(inputSource.direction));

                    if (inputSource.touchCount != 0)
                        writer.WriteKeyAndValue("touch-count", Convert::ToString(inputSource.touchCount));

                    break;
                }
                case InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS:
                {
                    WriteDeviceIndex(writer, inputSource);

                    if (inputSource.semantic != InputComponentSemantic::NONE)
                        WriteSemantic(writer, inputSource.semantic);
                    else if (inputSource.code != InputSource::NO_COMPONENT_CODE)
                        writer.WriteKeyAndValue("code", Convert::ToString(inputSource.code));
                    else if (inputSource.index != (size_t)-1)
                        writer.WriteKeyAndValue("index", Convert::ToString(inputSource.index));

                    if (inputSource.direction != 0)
                        writer.WriteKeyAndValue("direction", Convert::ToString(inputSource.direction));

                    if (inputSource.touchCount != 0)
                        writer.WriteKeyAndValue("touch-count", Convert::ToString(inputSource.touchCount));

                    break;
                }

                case InputDeviceComponent::ACCELEROMETER_RELATIVE_AXIS:
                {
                    break;
                }
                case InputDeviceComponent::ACCELEROMETER_ABSOLUTE_AXIS:
                {
                    break;
                }

                case InputDeviceComponent::HEADSET_LOCATOR:
                {
                    WriteDeviceIndex(writer, inputSource);

                    if (inputSource.semantic != InputComponentSemantic::NONE)
                        WriteSemantic(writer, inputSource.semantic);
                    else if (inputSource.code != InputSource::NO_COMPONENT_CODE)
                        writer.WriteKeyAndValue("code", Convert::ToString(inputSource.code));
                    else if (inputSource.index != (size_t)-1)
                        writer.WriteKeyAndValue("index", Convert::ToString(inputSource.index));

                    break;
                }

                default: break;
            }
        }

        static void WriteTriggerCriteria(ConfigDocumentWriter& writer, InputTriggerCriteria triggerCriteria)
        {
            if (AnySet(triggerCriteria.flags & InputTriggerFlag::PRESSED))
                writer.WriteKeyAndValue("pressed", "true");
            if (AnySet(triggerCriteria.flags & InputTriggerFlag::HOLDING))
                writer.WriteKeyAndValue("holding", "true");
            if (AnySet(triggerCriteria.flags & InputTriggerFlag::RELEASED))
                writer.WriteKeyAndValue("released", "true");
            if (AnySet(triggerCriteria.flags & InputTriggerFlag::POV_WEAK))
                writer.WriteKeyAndValue("pov-weak", "true");
            if (AnySet(triggerCriteria.flags & InputTriggerFlag::POV_STRONG))
                writer.WriteKeyAndValue("pov-strong", "true");
            if (AnySet(triggerCriteria.flags & InputTriggerFlag::TOUCH_ALLOWED_WITH_MULTITOUCH))
                writer.WriteKeyAndValue("touch-allowed-with-multitouch", "true");
            if (triggerCriteria.count > 1)
                writer.WriteKeyAndValue("count", Convert::ToString(triggerCriteria.count));
        }

        static void WriteSemantic(ConfigDocumentWriter& writer, InputComponentSemantic semantic)
        {
            if (semantic != InputComponentSemantic::NONE)
                writer.WriteKeyAndValue("semantic", InputComponentSemanticUtilities::ToString(semantic));
        }

    private:
        InputBindings& inputBindings;
        InputContext* inputContext;
    };

} }
