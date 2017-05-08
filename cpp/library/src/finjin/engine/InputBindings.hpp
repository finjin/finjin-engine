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
#include "finjin/common/BitArray.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Distance.hpp"
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/VirtualFileSystem.hpp"
#include "finjin/engine/Asset.hpp"
#include "finjin/engine/AssetReference.hpp"
#include "finjin/engine/ContextEventInfo.hpp"
#include "finjin/engine/InputBindingsSerializer.hpp"
#include "finjin/engine/InputSource.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    //InputBindingsDefaults
    struct InputBindingsDefaults
    {
        float mouseAxisSensitivity;
        float gameControllerAxisDeadZone;
        float gameControllerAxisSensitivity;

        InputBindingsDefaults()
        {
            this->mouseAxisSensitivity = 1;
            this->gameControllerAxisDeadZone = 0; //0 = Rely on the device/axis's own dead zone
            this->gameControllerAxisSensitivity = 1;
        }
    };

    //InputActionInfoFlag
    enum class InputActionInfoFlag : uint32_t
    {
        NONE = 0,

        /** Indicates that the action is private and should not be loaded or saved */
        PRIVATE = 1 << 0
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(InputActionInfoFlag)

    enum class InputBindingsConfigurationFlag : uint32_t
    {
        NONE = 0,
        CONNECTED_ONLY = 1 << 0
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(InputBindingsConfigurationFlag)

    struct InputBindingsConfigurationSearchCriteria
    {
        InputBindingsConfigurationSearchCriteria(InputDeviceClass deviceClass, size_t index = (size_t)-1, InputBindingsConfigurationFlag flag = InputBindingsConfigurationFlag::NONE, InputDeviceSemantic semantic = InputDeviceSemantic::NONE)
        {
            this->deviceClass = deviceClass;
            this->deviceSemantic = semantic;
            this->deviceIndex = index;
            this->flags = flags;
        }

        InputBindingsConfigurationSearchCriteria(InputDeviceClass deviceClass, const Utf8String& productDescriptor, size_t index = (size_t)-1, InputBindingsConfigurationFlag flags = InputBindingsConfigurationFlag::NONE, InputDeviceSemantic semantic = InputDeviceSemantic::NONE)
        {
            this->deviceClass = deviceClass;
            this->deviceSemantic = semantic;
            this->productDescriptor = productDescriptor;
            this->deviceIndex = index;
            this->flags = flags;
        }

        InputBindingsConfigurationSearchCriteria(InputDeviceClass deviceClass, const Utf8String& productDescriptor, const Utf8String& instanceDescriptor, InputBindingsConfigurationFlag flags = InputBindingsConfigurationFlag::NONE, InputDeviceSemantic semantic = InputDeviceSemantic::NONE)
        {
            this->deviceClass = deviceClass;
            this->deviceSemantic = semantic;
            this->productDescriptor = productDescriptor;
            this->instanceDescriptor = instanceDescriptor;
            this->deviceIndex = (size_t)-1;
            this->flags = flags;
        }

        bool IsMatchingDeviceIndex(size_t index) const
        {
            return this->deviceIndex == (size_t)-1 || this->deviceIndex == index;
        }

        bool IsMatchingDeviceSemantic(InputDeviceSemantic deviceSemantic) const
        {
            return this->deviceSemantic == InputDeviceSemantic::NONE || this->deviceSemantic == deviceSemantic;
        }

        InputDeviceClass deviceClass;
        InputDeviceSemantic deviceSemantic;
        size_t deviceIndex;
        InputBindingsConfigurationFlag flags;
        Utf8String productDescriptor;
        Utf8String instanceDescriptor;
    };

    //InputBindingsConfigurationResult
    class InputBindingsConfigurationResult
    {
    public:
        enum class Type
        {
            SUCCESS,
            NO_DEVICES, //No devices exist for specified device class
            BAD_SEARCH_CRITERIA, //Criteria used to search for criteria is invalid
            NO_CONFIGURATION, //Device exists but it doesn't have a configuration
            NOT_CONNECTED, //Device exists but it wasn't connected (and the caller wanted only connected devices)
            NOT_FOUND //Not device matching criteria could be found
        };

        Type type;
        Utf8String productDescriptor;
        Utf8String instanceDescriptor;
        size_t deviceIndex;

        InputBindingsConfigurationResult()
        {
            this->type = Type::SUCCESS;
            this->deviceIndex = (size_t)-1;
        }

        InputBindingsConfigurationResult(Type type)
        {
            this->type = type;
        }

        InputBindingsConfigurationResult(Type type, const Utf8String& productDescriptor, const Utf8String& instanceDescriptor, size_t index)
        {
            this->type = type;
            this->productDescriptor = productDescriptor;
            this->instanceDescriptor = instanceDescriptor;
            this->deviceIndex = index;
        }

        bool IsSuccess() const
        {
            return this->type == Type::SUCCESS;
        }
    };

    /**
     * A value generated during the processing of input actions, which are passed to ProcessInputAction().
     */
    struct InputBindingValue
    {
        enum class Type
        {
            SCALAR, //This is used for nearly everything
            VECTOR_3,
            MATRIX_33,
            LOCATOR //Used by inputs that generate a locator, like a VR headset or a VR motion controller
        };

        InputBindingValue()
        {
            this->scalar = 0;
            this->type = Type::SCALAR;
        }

        InputBindingValue(float value)
        {
            this->scalar = value;
            this->type = Type::SCALAR;
        }

        InputBindingValue(const float* value, Type type)
        {
            assert(value != nullptr);

            switch (type)
            {
                case Type::SCALAR: this->scalar = *value; break;
                case Type::VECTOR_3: FINJIN_COPY_MEMORY(this->vect3, value, sizeof(float) * 3); break;
                case Type::MATRIX_33: FINJIN_COPY_MEMORY(this->matrix33, value, sizeof(float) * 3 * 3); break;
                default: break;
            }

            this->type = type;
        }

        struct Locator
        {
            float orientation[3 * 3];
            float position[3];
            float velocity[3];
        };

        union
        {
            float scalar;
            float vect3[3];
            float matrix33[3 * 3];
            Locator locator;
        };

        Type type = Type::SCALAR;
    };

    //InputBindingsUnitConverter
    //int actionID, InputDeviceComponent deviceComponent, InputBindingValue& amount, SimpleTimeDelta elapsedTimeSeconds
    using InputBindingsUnitConverter = std::function<void(int, InputDeviceComponent, InputBindingValue&, SimpleTimeDelta)>;

    /**
     * Template input binding base class.
     *
     * This class operates by translating actual user inputs (from a keyboard, mouse, etc) to higher level application-defined actions.
     * The translation occurs in two stages:
     *  1)The first stage turns an input into an InputActionID.
     *  2)In the second stage, the InputActionID is then turned into an EventID, which is added to a collection of Actions.
     *
     * For example:
     *  -Some InputActionID values might be: MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT.
     *  -Those might be translated to a single MOVE EventID, which may also have additional data for tracking the direction of the move.
     *  -In the process of performing the translation, depending on the input source type, values may also be generated as well. This is the case for mouse and game controller inputs
     *
     * This class has one virtual method that must be overridden in a derived class:
     *   virtual void AddAction(Actions& actions, InputActionID action, InputDeviceComponent deviceType, TriggerFlags trigger, InputBindingValue& amount, SimpleTimeDelta elapsedTimeSeconds);
     *
     * The Actions type is never used directly. It can be whatever you use in your AddAction()
     * implementation. It will typically provide some facility for retaining the event information
     * that is passed to AddAction().
     */
    template <typename Actions, size_t maxBindings = 100>
    FINJIN_ASSET_CLASS class InputBindings
    {
    public:
        using InputActionID = int;

        enum { MAX_BINDINGS = maxBindings };

        //Structs--------------------------------------------------------------------------------
        /** Maps an input source to an action. */
        struct InputToAction
        {
            InputToAction()
            {
                this->deviceClass = InputDeviceClass::NONE;
                this->deviceClassIndex = 0;
            }

            void FinishInitialization()
            {
                this->deviceClass = InputDeviceComponentUtilities::GetDeviceClass(this->inputSource.deviceComponent);
                this->deviceClassIndex = InputDeviceClassUtilities::ToIndex(this->deviceClass);
            }

            /** The source input device for the action */
            InputSource inputSource;

            /** The action that results */
            InputActionID action;

            /** Flags indicating when action is triggered */
            InputTriggerCriteria triggerCriteria;

            //Derived values
            InputDeviceClass deviceClass;
            size_t deviceClassIndex;
        };

        /* Descriptor for a single action. */
        class InputActionInfo
        {
        public:
            /** Constructor used to initialize the null action info */
            InputActionInfo()
            {
                this->name = nullptr;
                this->flags = InputActionInfoFlag::NONE;
            }

            /** The most commonly used constructor */
            InputActionInfo(const char* actionName, InputActionID action, InputActionInfoFlag flags = InputActionInfoFlag::NONE) : name(actionName)
            {
                this->action = action;
                this->flags = flags;
            }

            /** Determines if the action is public */
            bool IsPublic() const
            {
                return this->name != nullptr && this->name[0] != 0 && NoneSet(this->flags & InputActionInfoFlag::PRIVATE);
            }

            bool IsNull() const
            {
                return this->name == nullptr || this->name == nullptr;
            }

        public:
            /**
            * The name of the action.
            * This will be used when loading and saving bindings, so it's best if
            * the name doesn't contain whitespace
            */
            const char* name;

            /** The action identifier */
            InputActionID action;

            /** Indicates whether the info represents the 'null' action */
            bool isNull;

        private:
            /** Flags */
            InputActionInfoFlag flags;
        };


        //Methods--------------------------------------------------------------------------------
        InputBindings(const InputBindingsDefaults& defaults = InputBindingsDefaults())
        {
            this->unitConverter = nullptr;

            for (size_t i = 0; i < this->defaultMouseAxisSensitivity.size(); i++)
                this->defaultMouseAxisSensitivity[i] = defaults.mouseAxisSensitivity;

            for (size_t i = 0; i < this->defaultGameControllerAxisDeadZone.size(); i++)
            {
                this->defaultGameControllerAxisDeadZone[i] = defaults.gameControllerAxisDeadZone;

                this->defaultGameControllerAxisSensitivity[i] = defaults.gameControllerAxisSensitivity;
            }

            Reset();
        }

        virtual ~InputBindings()
        {
        }

        void Reset()
        {
            this->bindings.clear();

            for (size_t i = 0; i < this->defaultMouseAxisSensitivity.size(); i++)
                this->mouseAxisSensitivity[i] = this->defaultMouseAxisSensitivity[i];

            for (size_t i = 0; i < this->gameControllerAxisSensitivity.size(); i++)
            {
                this->gameControllerAxisDeadZone[i] = this->defaultGameControllerAxisDeadZone[i];

                this->gameControllerAxisSensitivity[i] = this->defaultGameControllerAxisSensitivity[i];
            }
        }

        InputBindingsUnitConverter GetUnitConverter()
        {
            return this->unitConverter;
        }

        void SetUnitConverter(InputBindingsUnitConverter unitConverter)
        {
            this->unitConverter = unitConverter;
        }

        size_t GetBindingCount() const
        {
            return this->bindings.size();
        }

        const InputToAction& GetBinding(size_t index) const
        {
            return this->bindings[index];
        }

        InputToAction& GetBinding(size_t index)
        {
            return this->bindings[index];
        }

        template <typename InputCommands>
        void AddHapticFeedback(InputCommands& inputCommands, const HapticFeedbackSettings& force, InputDeviceClass deviceClass, const ContextEventInfo& eventInfo = ContextEventInfo::Empty())
        {
            std::array<BitArray<InputDeviceConstants::MAX_DEVICES>, InputDeviceClassUtilities::COUNT> triggeredDevices;

            auto bindingCount = GetBindingCount();
            for (size_t index = 0; index < bindingCount; index++)
            {
                auto& binding = GetBinding(index);
                if (AnySet(deviceClass & binding.deviceClass) && !triggeredDevices[binding.deviceClassIndex].TestAndSetBit(binding.inputSource.deviceIndex))
                    inputCommands.AddHapticFeedback(binding.deviceClass, binding.inputSource.deviceIndex, force, eventInfo);
            }
        }

        template <typename InputCommands>
        void StopHapticFeedback(InputCommands& inputCommands, InputDeviceClass deviceClass, const ContextEventInfo& eventInfo = ContextEventInfo::Empty())
        {
            std::array<BitArray<InputDeviceConstants::MAX_DEVICES>, InputDeviceClassUtilities::COUNT> triggeredDevices;

            auto bindingCount = GetBindingCount();
            for (size_t index = 0; index < bindingCount; index++)
            {
                auto& binding = GetBinding(index);
                if (AnySet(deviceClass & binding.deviceClass) && !triggeredDevices[binding.deviceClassIndex].TestAndSetBit(binding.inputSource.deviceIndex))
                    inputCommands.StopHapticFeedback(binding.deviceClass, binding.inputSource.deviceIndex, eventInfo);
            }
        }

        /**
         * Checks the input binding item for the specified action and input method.
         * @param deviceType [in] - The device type. This is needed since there may be multiple bindings
         * for the same action, each with a different device type.
         * Should be one of the following: InputDeviceComponent::KEYBOARD_KEY, InputDeviceComponent::MOUSE_BUTTON,
         * InputSource::MOUSE_AXIS, InputDeviceComponent::GAME_CONTROLLER_BUTTON, InputDeviceComponent::GAME_CONTROLLER_AXIS, InputDeviceComponent::GAME_CONTROLLER_POV
         * @param action [in] - The action identifier
         * @return If the specified input binding is found, true is returned. Otherwise, false is returned
         */
        bool HasInputBinding(InputDeviceComponent deviceComponent, InputActionID action) const
        {
            auto bindingCount = GetBindingCount();
            for (size_t index = 0; index < bindingCount; index++)
            {
                auto& binding = GetBinding(index);
                if (binding.action == action && binding.inputSource.deviceComponent == deviceComponent)
                    return true;
            }

            return false;
        }

        /**
         * Gets the input binding item for the specified action and input method.
         * @param deviceType [in] - The device type. This is needed since there may be multiple bindings
         * for the same action, each with a different device type.
         * Should be one of the following: InputDeviceComponent::KEYBOARD_KEY, InputDeviceComponent::MOUSE_BUTTON,
         * InputSource::MOUSE_AXIS, InputDeviceComponent::GAME_CONTROLLER_BUTTON, InputDeviceComponent::GAME_CONTROLLER_AXIS, InputDeviceComponent::GAME_CONTROLLER_POV
         * @param action [in] - The action identifier
         * @param inputBinding [out] - The found input binding
         * @return If the specified input binding is found, true is returned. Otherwise, false is returned
         */
        bool GetInputBinding(InputDeviceComponent deviceComponent, InputActionID action, InputToAction& inputBinding) const
        {
            auto bindingCount = GetBindingCount();
            for (size_t index = 0; index < bindingCount; index++)
            {
                auto& binding = GetBinding(index);
                if (binding.inputSource.deviceComponent == deviceComponent && binding.action == action)
                {
                    inputBinding = binding;
                    return true;
                }
            }

            return false;
        }

        /**
         * Adds a new input binding or modifies an existing one.
         */
        ValueOrError<bool> SetInputBinding(const InputToAction& inputBinding)
        {
            //Try to set the binding into an existing location
            auto bindingCount = GetBindingCount();
            for (size_t index = 0; index < bindingCount; index++)
            {
                auto& binding = GetBinding(index);
                if (binding.inputSource.deviceComponent == inputBinding.inputSource.deviceComponent && binding.action == inputBinding.action)
                {
                    binding = inputBinding;
                    return ValueOrError<bool>();
                }
            }

            //If we're here, no such binding exists. Add it
            return AddBinding(inputBinding.inputSource, inputBinding.action, inputBinding.triggerCriteria);
        }

        //Gets/sets the mouse sensitivity
        float GetMouseAxisSensitivity(size_t axis) const
        {
            return this->mouseAxisSensitivity[axis];
        }
        void SetMouseAxisSensitivity(size_t axis, float sensitivity)
        {
            this->mouseAxisSensitivity[axis] = sensitivity;
        }

        //Gets/sets the game controller dead zone
        int GetGameControllerAxisDeadZone(size_t axis) const
        {
            return this->gameControllerAxisDeadZone[axis];
        }
        void SetGameControllerAxisDeadZone(size_t axis, float deadZone)
        {
            this->gameControllerAxisDeadZone[axis] = deadZone;
        }
        void SetGameControllerAxisDeadZone(float deadZone)
        {
            for (size_t i = 0; i < this->gameControllerAxisDeadZone.size(); i++)
                this->gameControllerAxisDeadZone[i] = deadZone;
        }

        //Gets/sets the sensitivity for a game controller axis
        float GetGameControllerAxisSensitivity(size_t axis)
        {
            return this->gameControllerAxisSensitivity[axis];
        }
        void SetGameControllerAxisSensitivity(size_t axis, float sensitivity)
        {
            this->gameControllerAxisSensitivity[axis] = sensitivity;
        }

        /**
         * Translates an input to an action
         * @param actions [out] - The container for the generated action
         * @param action [in] - The action identifier
         * @param deviceType [in] - The type of device that triggered the action to occur.
         * This is useful in rare situations where logic is needed for a particular device type
         * @param trigger [in] - The type of trigger that caused the event to occur. Will be
         * InputTriggerFlag::HOLDING, InputTriggerFlag::PRESSED, or InputTriggerFlag::RELEASED
         * @param amount [in] - This will vary depending on the input device mapped to
         * the action. Digital devices will generate 0 or 1, analog devices will generate
         * a value in [-1, 1]
         */
        virtual void ProcessInputAction
            (
            Actions& actions,
            InputActionID action,
            InputDeviceComponent deviceComponent,
            InputTriggerFlag trigger,
            InputBindingValue& amount,
            SimpleTimeDelta elapsedTimeSeconds
            ) = 0;

        /**
         * Adds an input binding
         * @param inputSource [in] - The input source that generates the action
         * @param action [in] - The action identifier
         * @param triggerCriteria [in] - Flags that affect the triggering of the action.
         */
        ValueOrError<bool> AddBinding(InputSource inputSource, InputActionID action, InputTriggerFlag triggerFlags = InputTriggerFlag::PRESSED | InputTriggerFlag::HOLDING | InputTriggerFlag::RELEASED)
        {
            return AddBinding(inputSource, action, InputTriggerCriteria(triggerFlags));
        }

        ValueOrError<bool> AddBinding(InputSource inputSource, InputActionID action, InputTriggerCriteria triggerCriteria)
        {
            InputToAction binding;

            binding.inputSource = inputSource;

            binding.action = action;

            binding.triggerCriteria = triggerCriteria;

            binding.FinishInitialization();

            return this->bindings.push_back(binding);
        }

        /**
         * Assigns the bindings of the specified class to a different device.
         */
        void SetInputBindingsDeviceIndex(size_t deviceIndex, InputDeviceClass deviceClass = InputDeviceClass::ALL)
        {
            for (size_t i = 0; i < this->bindings.size(); i++)
            {
                if (AnySet(this->bindings[i].deviceClass & deviceClass))
                    this->bindings[i].inputSource.deviceIndex = deviceIndex;
            }
        }

        /**
         * Gets a pointer to an array containing information about all the actions
         * The final element in the array must be a 'null' InputActionInfo
         */
        virtual const InputActionInfo* GetInputActionInfo() const = 0;

        /** Gets information about an action, using an action identifier. */
        const InputActionInfo& GetActionInfoByID(InputActionID action) const
        {
            auto actionInfo = GetInputActionInfo();
            assert(actionInfo != nullptr);

            size_t index = 0;
            for (; !actionInfo[index].IsNull(); index++)
            {
                if (action == actionInfo[index].action)
                    return actionInfo[index];
            }

            //Return null InputActionInfo
            return actionInfo[index];
        }

        /** Gets information about an action, using an action name. */
        template <typename T>
        const InputActionInfo& GetActionInfoByName(const T& action) const
        {
            auto actionInfo = GetInputActionInfo();
            assert(actionInfo != nullptr);

            size_t index = 0;
            for (; !actionInfo[index].IsNull(); index++)
            {
                if (action == actionInfo[index].name)
                    return actionInfo[index];
            }

            //Return null InputActionInfo
            return actionInfo[index];
        }

        enum class GetActionBindingsResult
        {
            SUCCESS,
            CONFIG_FILE_READ_FAILED,
            CONFIG_FILE_PARSE_FAILED
        };

        template <typename InputContext>
        GetActionBindingsResult GetActionBindingsFromConfiguration
            (
            InputContext* inputContext,
            InputDeviceClass deviceClass,
            size_t deviceIndex,
            const AssetReference& configFileName,
            ByteBuffer& fileBuffer
            )
        {
            auto readResult = inputContext->ReadInputBinding(deviceClass, deviceIndex, configFileName, fileBuffer);
            if (readResult != FileOperationResult::SUCCESS)
                return GetActionBindingsResult::CONFIG_FILE_READ_FAILED;

            InputBindingsSerializer<InputBindings<Actions, maxBindings>, InputContext> serializer(*this);

            FINJIN_DECLARE_ERROR(error);
            serializer.Read(fileBuffer, error);
            if (error)
                return GetActionBindingsResult::CONFIG_FILE_PARSE_FAILED;

            return GetActionBindingsResult::SUCCESS;
        }

        template <typename InputContext>
        InputBindingsConfigurationResult GetFromConfiguration
            (
            InputContext* inputContext,
            const InputBindingsConfigurationSearchCriteria& criteria,
            const AssetReference& configFileName,
            ByteBuffer& configFileBuffer
            )
        {
            auto deviceCount = inputContext->GetDeviceCount(criteria.deviceClass);
            if (deviceCount == 0)
                return InputBindingsConfigurationResult::Type::NO_DEVICES;

            if (!criteria.instanceDescriptor.empty())
            {
                //Using an instance descriptor is the most direct search since it's device and instance-specific

                for (size_t i = 0; i < deviceCount; i++)
                {
                    if (inputContext->GetDeviceInstanceDescriptor(criteria.deviceClass, i) == criteria.instanceDescriptor)
                    {
                        if (AnySet(criteria.flags & InputBindingsConfigurationFlag::CONNECTED_ONLY) &&
                            !inputContext->IsDeviceConnected(criteria.deviceClass, i))
                        {
                            return InputBindingsConfigurationResult::Type::NOT_CONNECTED;
                        }

                        if (GetActionBindingsFromConfiguration(inputContext, criteria.deviceClass, i, configFileName, configFileBuffer) == GetActionBindingsResult::SUCCESS)
                        {
                            inputContext->TranslateInputBindingsSemantics(*this, criteria.deviceClass, i);
                            FinishBindingsInitialization();

                            return InputBindingsConfigurationResult
                                (
                                InputBindingsConfigurationResult::Type::SUCCESS,
                                criteria.productDescriptor,
                                criteria.instanceDescriptor,
                                i
                                );
                        }
                        else
                            return InputBindingsConfigurationResult::Type::NO_CONFIGURATION;
                    }
                }

                //At this point no match was found

                //The search is done if the caller doesn't also want to search by product descriptor
                if (!criteria.productDescriptor.empty())
                    return InputBindingsConfigurationResult::Type::NOT_FOUND;
            }

            if (!criteria.productDescriptor.empty())
            {
                //A product descriptor was specified, so find the matching device and initialize

                size_t validDeviceIndex = 0;
                for (size_t i = 0; i < deviceCount; i++)
                {
                    if (inputContext->GetDeviceProductDescriptor(criteria.deviceClass, i) == criteria.productDescriptor)
                    {
                        if (AnySet(criteria.flags & InputBindingsConfigurationFlag::CONNECTED_ONLY) &&
                            !inputContext->IsDeviceConnected(criteria.deviceClass, i))
                        {
                            //Device is disconnected. Skip
                            continue;
                        }

                        if (criteria.IsMatchingDeviceIndex(validDeviceIndex) &&
                            criteria.IsMatchingDeviceSemantic(inputContext->GetDeviceSemantic(criteria.deviceClass, i)))
                        {
                            if (GetActionBindingsFromConfiguration(inputContext, criteria.deviceClass, i, configFileName, configFileBuffer) == GetActionBindingsResult::SUCCESS)
                            {
                                inputContext->TranslateInputBindingsSemantics(*this, criteria.deviceClass, i);
                                FinishBindingsInitialization();

                                return InputBindingsConfigurationResult
                                    (
                                    InputBindingsConfigurationResult::Type::SUCCESS,
                                    criteria.productDescriptor,
                                    inputContext->GetDeviceInstanceDescriptor(criteria.deviceClass, i),
                                    i
                                    );
                            }
                            else
                                return InputBindingsConfigurationResult::Type::NO_CONFIGURATION;
                        }

                        validDeviceIndex++;
                    }
                }

                return InputBindingsConfigurationResult::Type::NOT_FOUND;
            }
            else
            {
                //Searching by index. Index might be in [0, deviceCount) or a wildcard (-1)

                if (criteria.deviceIndex != (size_t)-1 && criteria.deviceIndex >= deviceCount)
                    return InputBindingsConfigurationResult::Type::BAD_SEARCH_CRITERIA;

                if (criteria.deviceIndex != (size_t)-1 &&
                    AnySet(criteria.flags & InputBindingsConfigurationFlag::CONNECTED_ONLY) &&
                    !inputContext->IsDeviceConnected(criteria.deviceClass, criteria.deviceIndex))
                {
                    //Device is disconnected. Skip
                    return InputBindingsConfigurationResult::Type::NOT_FOUND;
                }

                size_t validDeviceIndex = 0;
                for (size_t i = 0; i < deviceCount; i++)
                {
                    if (criteria.IsMatchingDeviceIndex(validDeviceIndex) &&
                        criteria.IsMatchingDeviceSemantic(inputContext->GetDeviceSemantic(criteria.deviceClass, i)))
                    {
                        if (AnySet(criteria.flags & InputBindingsConfigurationFlag::CONNECTED_ONLY) &&
                            !inputContext->IsDeviceConnected(criteria.deviceClass, i))
                        {
                            //Device is disconnected. Skip
                            continue;
                        }

                        if (GetActionBindingsFromConfiguration(inputContext, criteria.deviceClass, i, configFileName, configFileBuffer) == GetActionBindingsResult::SUCCESS)
                        {
                            inputContext->TranslateInputBindingsSemantics(*this, criteria.deviceClass, i);
                            FinishBindingsInitialization();

                            return InputBindingsConfigurationResult
                                (
                                InputBindingsConfigurationResult::Type::SUCCESS,
                                criteria.productDescriptor,
                                inputContext->GetDeviceInstanceDescriptor(criteria.deviceClass, i),
                                i
                                );
                        }
                        else
                            return InputBindingsConfigurationResult::Type::NO_CONFIGURATION;

                        validDeviceIndex++;
                    }
                }

                return InputBindingsConfigurationResult::Type::NOT_FOUND;
            }
        }

        template <typename Device>
        void TranslateTouchScreenBindings(size_t deviceIndex, Device* device)
        {
            //Do nothing
        }

        template <typename Device>
        void TranslateGameControllerBindings(size_t deviceIndex, Device* device)
        {
            for (size_t bindingIndex = 0; bindingIndex < GetBindingCount(); bindingIndex++)
            {
                auto& binding = GetBinding(bindingIndex);
                if (TranslateGameControllerBindingSemantic(binding, device) || TranslateGameControllerBindingCode(binding, device))
                    binding.inputSource.deviceIndex = deviceIndex;
            }
        }

        template <typename InputToAction, typename Device>
        bool TranslateGameControllerBindingSemantic(InputToAction& binding, Device* device)
        {
            if (binding.inputSource.semantic != InputComponentSemantic::NONE)
            {
                for (size_t i = 0; i < device->GetAxisCount(); i++)
                {
                    auto component = device->GetAxis(i);
                    if (AnySet(component->GetSemantic() & binding.inputSource.semantic))
                    {
                        int direction = InputComponentSemanticUtilities::GetMoveLookToggleDirection(binding.inputSource.semantic);
                        binding.inputSource = InputSource::FromGameControllerAxis(i, direction);
                        return true;
                    }
                }

                for (size_t i = 0; i < device->GetButtonCount(); i++)
                {
                    auto component = device->GetButton(i);
                    if (AnySet(component->GetSemantic() & binding.inputSource.semantic))
                    {
                        binding.inputSource = InputSource::FromGameControllerButton(i);
                        return true;
                    }
                }

                for (size_t i = 0; i < device->GetPovCount(); i++)
                {
                    auto component = device->GetPov(i);
                    if (AnySet(component->GetSemantic() & binding.inputSource.semantic))
                    {
                        auto direction = PovDirection::CENTERED;
                        if (AnySet(binding.inputSource.semantic & InputComponentSemantic::TOGGLE_UP))
                            direction |= PovDirection::UP;
                        else if (AnySet(binding.inputSource.semantic & InputComponentSemantic::TOGGLE_DOWN))
                            direction |= PovDirection::DOWN;
                        if (AnySet(binding.inputSource.semantic & InputComponentSemantic::TOGGLE_LEFT))
                            direction |= PovDirection::LEFT;
                        else if (AnySet(binding.inputSource.semantic & InputComponentSemantic::TOGGLE_RIGHT))
                            direction |= PovDirection::RIGHT;
                        binding.inputSource = InputSource::FromGameControllerPov(i, direction);
                        return true;
                    }
                }

                for (size_t i = 0; i < device->GetLocatorCount(); i++)
                {
                    auto component = device->GetLocator(i);
                    if (AnySet(component->GetSemantic() & binding.inputSource.semantic))
                    {
                        binding.inputSource = InputSource::FromGameControllerLocator(i);
                        return true;
                    }
                }
            }

            return false;
        }

        template <typename InputToAction, typename Device>
        bool TranslateGameControllerBindingCode(InputToAction& binding, Device* device)
        {
            if (binding.inputSource.HasDeviceComponentCode())
            {
                switch (binding.inputSource.deviceComponent)
                {
                    case InputDeviceComponent::GAME_CONTROLLER_BUTTON:
                    {
                        for (size_t i = 0; i < device->GetButtonCount(); i++)
                        {
                            if (device->GetButton(i)->GetCode() == binding.inputSource.buttonCode)
                            {
                                binding.inputSource.buttonIndex = i;
                                return true;
                            }
                        }

                        break;
                    }
                    case InputDeviceComponent::GAME_CONTROLLER_AXIS:
                    {
                        for (size_t i = 0; i < device->GetAxisCount(); i++)
                        {
                            if (device->GetAxis(i)->GetCode() == binding.inputSource.axisCode)
                            {
                                binding.inputSource.axisIndex = i;
                                return true;
                            }
                        }

                        break;
                    }
                    case InputDeviceComponent::GAME_CONTROLLER_POV:
                    {
                        for (size_t i = 0; i < device->GetPovCount(); i++)
                        {
                            if (device->GetPov(i)->GetCode() == binding.inputSource.povCode)
                            {
                                binding.inputSource.povIndex = i;
                                return true;
                            }
                        }

                        break;
                    }
                    case InputDeviceComponent::GAME_CONTROLLER_LOCATOR:
                    {
                        for (size_t i = 0; i < device->GetLocatorCount(); i++)
                        {
                            if (device->GetLocator(i)->GetCode() == binding.inputSource.locatorCode)
                            {
                                binding.inputSource.locatorIndex = i;
                                return true;
                            }
                        }

                        break;
                    }
                    default: break;
                }
            }

            return false;
        }

        template <typename Device>
        void TranslateKeyboardBindings(size_t deviceIndex, Device* device)
        {
            for (size_t bindingIndex = 0; bindingIndex < GetBindingCount(); bindingIndex++)
            {
                auto& binding = GetBinding(bindingIndex);
                if (TranslateKeyboardBindingSemantic(binding, device) || TranslateKeyboardBindingCode(binding, device))
                    binding.inputSource.deviceIndex = deviceIndex;
            }
        }

        template <typename InputToAction, typename Device>
        bool TranslateKeyboardBindingSemantic(InputToAction& binding, Device* device)
        {
            if (binding.inputSource.semantic != InputComponentSemantic::NONE)
            {
                for (size_t i = 0; i < device->GetButtonCount(); i++)
                {
                    auto component = device->GetButton(i);
                    if (AnySet(component->GetSemantic() & binding.inputSource.semantic))
                    {
                        binding.inputSource = InputSource::FromKeyboardKey(i);
                        return true;
                    }
                }
            }

            return false;
        }

        template <typename InputToAction, typename Device>
        bool TranslateKeyboardBindingCode(InputToAction& binding, Device* device)
        {
            if (binding.inputSource.HasDeviceComponentCode())
            {
                switch (binding.inputSource.deviceComponent)
                {
                    case InputDeviceComponent::KEYBOARD_KEY:
                    {
                        for (size_t i = 0; i < device->GetButtonCount(); i++)
                        {
                            if (device->GetButton(i)->GetCode() == binding.inputSource.keyCode)
                            {
                                binding.inputSource.keyIndex = i;
                                return true;
                            }
                        }

                        break;
                    }
                    default: break;
                }
            }

            return false;
        }

        template <typename Device>
        void TranslateMouseBindings(size_t deviceIndex, Device* device)
        {
            for (size_t bindingIndex = 0; bindingIndex < GetBindingCount(); bindingIndex++)
            {
                auto& binding = GetBinding(bindingIndex);
                if (TranslateMouseBindingSemantic(binding, device) || TranslateMouseBindingCode(binding, device))
                    binding.inputSource.deviceIndex = deviceIndex;
            }
        }

        template <typename InputToAction, typename Device>
        bool TranslateMouseBindingSemantic(InputToAction& binding, Device* device)
        {
            if (binding.inputSource.semantic != InputComponentSemantic::NONE)
            {
                for (size_t i = 0; i < device->GetAxisCount(); i++)
                {
                    auto component = device->GetAxis(i);
                    if (AnySet(component->GetSemantic() & binding.inputSource.semantic))
                    {
                        int direction = InputComponentSemanticUtilities::GetMoveLookToggleDirection(binding.inputSource.semantic);
                        binding.inputSource = InputSource::FromMouseRelativeAxis(i, direction);
                        return true;
                    }
                }

                for (size_t i = 0; i < device->GetButtonCount(); i++)
                {
                    auto component = device->GetButton(i);
                    if (AnySet(component->GetSemantic() & binding.inputSource.semantic))
                    {
                        binding.inputSource = InputSource::FromMouseButton(i);
                        return true;
                    }
                }
            }

            return false;
        }

        template <typename InputToAction, typename Device>
        bool TranslateMouseBindingCode(InputToAction& binding, Device* device)
        {
            if (binding.inputSource.HasDeviceComponentCode())
            {
                switch (binding.inputSource.deviceComponent)
                {
                    case InputDeviceComponent::MOUSE_BUTTON:
                    {
                        for (size_t i = 0; i < device->GetButtonCount(); i++)
                        {
                            if (device->GetButton(i)->GetCode() == binding.inputSource.buttonCode)
                            {
                                binding.inputSource.buttonIndex = i;
                                return true;
                            }
                        }

                        break;
                    }
                    case InputDeviceComponent::MOUSE_RELATIVE_AXIS:
                    case InputDeviceComponent::MOUSE_ABSOLUTE_AXIS:
                    {
                        for (size_t i = 0; i < device->GetAxisCount(); i++)
                        {
                            if (device->GetAxis(i)->GetCode() == binding.inputSource.axisCode)
                            {
                                binding.inputSource.axisIndex = i;
                                return true;
                            }
                        }

                        break;
                    }
                    default: break;
                }
            }

            return false;
        }

        template <typename Device>
        void TranslateHeadsetBindings(size_t deviceIndex, Device* device)
        {
            for (size_t bindingIndex = 0; bindingIndex < GetBindingCount(); bindingIndex++)
            {
                auto& binding = GetBinding(bindingIndex);
                if (TranslateHeadsetBindingSemantic(binding, device) || TranslateHeadsetBindingCode(binding, device))
                    binding.inputSource.deviceIndex = deviceIndex;
            }
        }

        template <typename InputToAction, typename Device>
        bool TranslateHeadsetBindingSemantic(InputToAction& binding, Device* device)
        {
            if (binding.inputSource.semantic != InputComponentSemantic::NONE)
            {
                for (size_t i = 0; i < device->GetLocatorCount(); i++)
                {
                    auto component = device->GetLocator(i);
                    if (AnySet(component->GetSemantic() & binding.inputSource.semantic))
                    {
                        binding.inputSource = InputSource::FromHeadsetLocator(i);
                        return true;
                    }
                }
            }

            return false;
        }

        template <typename InputToAction, typename Device>
        bool TranslateHeadsetBindingCode(InputToAction& binding, Device* device)
        {
            if (binding.inputSource.HasDeviceComponentCode())
            {
                switch (binding.inputSource.deviceComponent)
                {
                    case InputDeviceComponent::HEADSET_LOCATOR:
                    {
                        for (size_t i = 0; i < device->GetLocatorCount(); i++)
                        {
                            if (device->GetLocator(i)->GetCode() == binding.inputSource.locatorCode)
                            {
                                binding.inputSource.locatorIndex = i;
                                return true;
                            }
                        }

                        break;
                    }
                    default: break;
                }
            }

            return false;
        }

        bool ModifiedMouseSensitivity() const
        {
            return FloatsDiffer(this->mouseAxisSensitivity, this->defaultMouseAxisSensitivity);
        }

        bool ModifiedGameControllerAxisDeadZones() const
        {
            return FloatsDiffer(this->gameControllerAxisDeadZone, this->defaultGameControllerAxisDeadZone);
        }

        bool ModifiedGameControllerAxisSensitivities()
        {
            return FloatsDiffer(this->gameControllerAxisSensitivity, this->defaultGameControllerAxisSensitivity);
        }

        template <typename T>
        bool FloatsDiffer(const T& a, const T& b) const
        {
            for (size_t i = 0; i < a.size(); i++)
            {
                if (a[i] != b[i])
                    return true;
            }
            return false;
        }

        template <typename T>
        bool FloatsDiffer(const T& a, float b)
        {
            for (size_t i = 0; i < a.size(); i++)
            {
                if (a[i] != b)
                    return true;
            }
            return false;
        }

        template <typename Keyboard>
        void ProcessKeyboardKeyBinding(const InputToAction& binding, Keyboard& keyboard, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.keyIndex >= keyboard.GetButtonCount())
                return;

            auto key = keyboard.GetButton(binding.inputSource.keyIndex);
            if (!key->IsEnabled())
                return;

            InputBindingValue amount(1.0f);

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && key->IsFirstDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::KEYBOARD_KEY, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && key->IsHoldingDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::KEYBOARD_KEY, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && key->IsFirstUp())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::KEYBOARD_KEY, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename Mouse>
        void ProcessMouseButtonBinding(const InputToAction& binding, Mouse& mouse, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.buttonIndex >= mouse.GetButtonCount())
                return;

            auto button = mouse.GetButton(binding.inputSource.buttonIndex);
            if (!button->IsEnabled())
                return;

            InputBindingValue amount(1.0f);

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && button->IsFirstDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MOUSE_BUTTON, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && button->IsHoldingDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MOUSE_BUTTON, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && button->IsFirstUp())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MOUSE_BUTTON, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename Mouse>
        void ProcessMouseRelativeAxisBinding(const InputToAction& binding, Mouse& mouse, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.axisIndex >= mouse.GetAxisCount())
                return;

            auto axis = mouse.GetAxis(binding.inputSource.axisIndex);
            if (!axis->IsEnabled())
                return;

            auto axisRelativeValue = axis->GetRelativeValue() * axis->GetDirection();
            if (!TestValueAgainstDirection(axisRelativeValue, binding.inputSource.directionFloat))
                return;

            InputBindingValue amount =
                axisRelativeValue *
                BestDirection(binding.inputSource.directionFloat) *
                this->mouseAxisSensitivity[binding.inputSource.axisIndex];

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && axis->IsFirstDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MOUSE_RELATIVE_AXIS, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && axis->IsHoldingDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MOUSE_RELATIVE_AXIS, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && axis->IsFirstUp())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MOUSE_RELATIVE_AXIS, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename Mouse>
        void ProcessMouseAbsoluteAxisBinding(const InputToAction& binding, Mouse& mouse, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.axisIndex >= mouse.GetAxisCount())
                return;

            auto axis = mouse.GetAxis(binding.inputSource.axisIndex);
            if (!axis->IsEnabled())
                return;

            auto absoluteValue = axis->GetAbsoluteValue();
            if (!TestValueAgainstDirection(absoluteValue, binding.inputSource.directionFloat))
                return;

            InputBindingValue amount =
                absoluteValue *
                BestDirection(binding.inputSource.directionFloat) *
                this->mouseAxisSensitivity[binding.inputSource.axisIndex];

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && axis->IsFirstDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MOUSE_ABSOLUTE_AXIS, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && axis->IsHoldingDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MOUSE_ABSOLUTE_AXIS, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && axis->IsFirstUp())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MOUSE_ABSOLUTE_AXIS, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename GameController>
        void ProcessGameControllerButtonBinding(const InputToAction& binding, GameController& gameController, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.buttonIndex >= gameController.GetButtonCount())
                return;

            auto button = gameController.GetButton(binding.inputSource.buttonIndex);
            if (!button->IsEnabled())
                return;

            InputBindingValue amount(1.0f);

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && button->IsFirstDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::GAME_CONTROLLER_BUTTON, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && button->IsHoldingDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::GAME_CONTROLLER_BUTTON, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && button->IsFirstUp())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::GAME_CONTROLLER_BUTTON, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename GameController>
        void ProcessGameControllerAxisBinding(const InputToAction& binding, GameController& gameController, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.axisIndex >= gameController.GetAxisCount())
                return;

            auto axis = gameController.GetAxis(binding.inputSource.axisIndex);
            if (!axis->IsEnabled())
                return;

            auto absoluteValue = axis->GetAbsoluteValue() * axis->GetDirection();

            auto deadZone = this->gameControllerAxisDeadZone[binding.inputSource.axisIndex];
            if (std::abs(absoluteValue) <= deadZone)
                absoluteValue = 0;

            if (!TestValueAgainstDirection(absoluteValue, binding.inputSource.directionFloat))
                return;

            InputBindingValue amount =
                absoluteValue *
                BestDirection(binding.inputSource.directionFloat) *
                this->gameControllerAxisSensitivity[binding.inputSource.axisIndex];

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && axis->IsFirstDown(deadZone))
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::GAME_CONTROLLER_AXIS, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && axis->IsHoldingDown(deadZone))
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::GAME_CONTROLLER_AXIS, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && axis->IsFirstUp(deadZone))
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::GAME_CONTROLLER_AXIS, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename GameController>
        void ProcessGameControllerPovBinding(const InputToAction& binding, GameController& gameController, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.povIndex >= gameController.GetPovCount())
                return;

            auto pov = gameController.GetPov(binding.inputSource.povIndex);
            if (!pov->IsEnabled())
                return;

            InputBindingValue amount(1.0f);

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && pov->IsFirstDown(binding.inputSource.povDirection, AnySet(binding.triggerCriteria.flags & InputTriggerFlag::POV_STRONG)))
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::GAME_CONTROLLER_POV, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && pov->IsHoldingDown(binding.inputSource.povDirection, AnySet(binding.triggerCriteria.flags & InputTriggerFlag::POV_STRONG)))
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::GAME_CONTROLLER_POV, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && pov->IsFirstUp(binding.inputSource.povDirection, AnySet(binding.triggerCriteria.flags & InputTriggerFlag::POV_STRONG)))
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::GAME_CONTROLLER_POV, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename Device>
        void ProcessGameControllerLocatorBinding(const InputToAction& binding, Device& device, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.locatorIndex >= device.GetLocatorCount())
                return;

            auto locator = device.GetLocator(binding.inputSource.locatorIndex);

            InputBindingValue amount;
            amount.type = InputBindingValue::Type::LOCATOR;
            locator->GetOrientationMatrix33(amount.locator.orientation);
            locator->GetPositionVector3(amount.locator.position, DistanceUnitType::METERS);
            locator->GetVelocityVector3(amount.locator.velocity, DistanceUnitType::METERS);

            _ProcessInputAction(actions, binding.action, InputDeviceComponent::GAME_CONTROLLER_LOCATOR, InputTriggerFlag::PRESSED | InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
        }

        template <typename Device>
        void ProcessHeadsetLocatorBinding(const InputToAction& binding, Device& device, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.locatorIndex >= device.GetLocatorCount())
                return;

            auto locator = device.GetLocator(binding.inputSource.locatorIndex);

            InputBindingValue amount;
            amount.type = InputBindingValue::Type::LOCATOR;
            locator->GetOrientationMatrix33(amount.locator.orientation);
            locator->GetPositionVector3(amount.locator.position, DistanceUnitType::METERS);
            locator->GetVelocityVector3(amount.locator.velocity, DistanceUnitType::METERS);

            _ProcessInputAction(actions, binding.action, InputDeviceComponent::HEADSET_LOCATOR, InputTriggerFlag::PRESSED | InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
        }

        template <typename TouchScreen>
        void ProcessTouchCountBinding(const InputToAction& binding, TouchScreen& touchScreen, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            auto touchCount = touchScreen.GetTouchCount();
            auto previousTouchCount = touchScreen.GetPreviousTouchCount();

            InputBindingValue amount = RoundToFloat(touchCount);

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && (touchCount > previousTouchCount) && IsInRange(binding.inputSource.touchCount, previousTouchCount, touchCount))
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::TOUCH_COUNT, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && (touchCount == previousTouchCount) && (touchCount == binding.inputSource.touchCount))
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::TOUCH_COUNT, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && (touchCount < previousTouchCount) && IsInRange(binding.inputSource.touchCount, touchCount, previousTouchCount))
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::TOUCH_COUNT, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename Pointer>
        void ProcessTouchRelativeAxisBinding(const InputToAction& binding, Pointer& pointer, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.axisIndex >= pointer.GetAxisCount())
                return;

            auto axis = pointer.GetAxis(binding.inputSource.axisIndex);
            auto relativeValue = axis->GetRelativeValue() * axis->GetDirection();
            if (!TestValueAgainstDirection(relativeValue, binding.inputSource.directionFloat))
                return;

            InputBindingValue amount = BestDirection(binding.inputSource.directionFloat) * relativeValue;

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && axis->IsFirstDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::TOUCH_RELATIVE_AXIS, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && axis->IsHoldingDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::TOUCH_RELATIVE_AXIS, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && axis->IsFirstUp())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::TOUCH_RELATIVE_AXIS, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename Pointer>
        void ProcessTouchAbsoluteAxisBinding(const InputToAction& binding, Pointer& pointer, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (binding.inputSource.axisIndex >= pointer.GetAxisCount())
                return;

            auto axis = pointer.GetAxis(binding.inputSource.axisIndex);
            auto absoluteValue = axis->GetAbsoluteValue();
            if (!TestValueAgainstDirection(absoluteValue, binding.inputSource.directionFloat))
                return;

            InputBindingValue amount = BestDirection(binding.inputSource.directionFloat) * absoluteValue;

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && axis->IsFirstDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::TOUCH_RELATIVE_AXIS, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && axis->IsHoldingDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::TOUCH_RELATIVE_AXIS, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && axis->IsFirstUp())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::TOUCH_RELATIVE_AXIS, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename TouchScreen>
        void ProcessMultitouchRelativeRadiusBinding(const InputToAction& binding, TouchScreen& touchScreen, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (touchScreen.GetPointerCount() != binding.inputSource.touchCount)
                return;

            auto radius = touchScreen.GetMultitouchRadius();
            auto relativeValue = radius.GetRelativeValue();
            if (!TestValueAgainstDirection(relativeValue, binding.inputSource.directionFloat))
                return;

            InputBindingValue amount = BestDirection(binding.inputSource.directionFloat) * relativeValue;

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && radius.IsFirstDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && radius.IsHoldingDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && radius.IsFirstUp())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MULTITOUCH_RELATIVE_RADIUS, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

        template <typename TouchScreen>
        void ProcessMultitouchRelativeAxisBinding(const InputToAction& binding, TouchScreen& touchScreen, Actions& actions, SimpleTimeDelta elapsedTimeSeconds)
        {
            if (touchScreen.GetPointerCount() != binding.inputSource.touchCount || binding.inputSource.axisIndex >= touchScreen.GetMultitouchAxisCount())
                return;

            auto axis = touchScreen.GetMultitouchAxis(binding.inputSource.axisIndex);
            auto relativeValue = axis->GetRelativeValue() * axis->GetDirection();
            if (!TestValueAgainstDirection(relativeValue, binding.inputSource.directionFloat))
                return;

            InputBindingValue amount = BestDirection(binding.inputSource.directionFloat) * relativeValue;

            if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::PRESSED) && axis->IsFirstDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS, InputTriggerFlag::PRESSED, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::HOLDING) && axis->IsHoldingDown())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS, InputTriggerFlag::HOLDING, amount, elapsedTimeSeconds);
            else if (AnySet(binding.triggerCriteria.flags & InputTriggerFlag::RELEASED) && axis->IsFirstUp())
                _ProcessInputAction(actions, binding.action, InputDeviceComponent::MULTITOUCH_RELATIVE_AXIS, InputTriggerFlag::RELEASED, amount, elapsedTimeSeconds);
        }

    protected:
        void FinishBindingsInitialization()
        {
            for (auto& binding : this->bindings)
                binding.FinishInitialization();
        }

        void _ProcessInputAction
            (
            Actions& actions,
            InputActionID action,
            InputDeviceComponent deviceComponent,
            InputTriggerFlag trigger,
            InputBindingValue& amount,
            SimpleTimeDelta elapsedTimeSeconds
            )
        {
            if (this->unitConverter != nullptr)
                this->unitConverter(action, deviceComponent, amount, elapsedTimeSeconds);

            ProcessInputAction(actions, action, deviceComponent, trigger, amount, elapsedTimeSeconds);
        }

        bool TestValueAgainstDirection(float value, float direction)
        {
            return value * BestDirection(direction) >= 0;
        }

        float BestDirection(float direction)
        {
            return direction != 0.0f ? direction : 1.0f;
        }

    public:
        /** All the input bindings. */
        StaticVector<InputToAction, maxBindings> bindings;

        //Mouse axis sensitivity
        std::array<float, MouseConstants::MAX_AXIS_COUNT> defaultMouseAxisSensitivity;
        std::array<float, MouseConstants::MAX_AXIS_COUNT> mouseAxisSensitivity;

        //Game controller axis dead zone
        std::array<float, GameControllerConstants::MAX_AXIS_COUNT> defaultGameControllerAxisDeadZone;
        std::array<float, GameControllerConstants::MAX_AXIS_COUNT> gameControllerAxisDeadZone;

        //Game controller axis sensitivity
        std::array<float, GameControllerConstants::MAX_AXIS_COUNT> defaultGameControllerAxisSensitivity;
        std::array<float, GameControllerConstants::MAX_AXIS_COUNT> gameControllerAxisSensitivity;

        InputBindingsUnitConverter unitConverter; //This survives a Reset()
    };

} }
