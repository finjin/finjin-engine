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
#include "finjin/common/Chrono.hpp"
#include "finjin/common/Distance.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/Setting.hpp"
#include "finjin/common/StaticUnorderedMap.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Utf8String.hpp"
#include "finjin/engine/InputConstants.hpp"
#include "finjin/engine/InputCoordinate.hpp"
#include "finjin/engine/PointerType.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    enum class PovDirection : int
    {
        CENTERED = 0,
        UP = 1 << 0,
        DOWN = 1 << 4,
        RIGHT = 1 << 8,
        LEFT = 1 << 12,
        UP_RIGHT = UP | RIGHT,
        DOWN_RIGHT = DOWN | RIGHT,
        UP_LEFT = UP | LEFT,
        DOWN_LEFT = DOWN | LEFT
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(PovDirection);

    struct PovDirectionUtilities
    {
        enum { COUNT = 9 }; //Number of values

        static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(PovDirection, COUNT)& GetLookup();

        static const char* ToString(PovDirection povDirection);

        template <typename T>
        static PovDirection Parse(const T& deviceComponent)
        {
            return GetLookup().GetOrDefault(deviceComponent, PovDirection::CENTERED);
        }
    };

    /** Device classes. These can be bitwise OR'ed together */
    enum class InputDeviceClass : uint32_t
    {
        NONE = 0,

        KEYBOARD = 1 << 0,
        MOUSE = 1 << 1,
        GAME_CONTROLLER = 1 << 2,
        TOUCH_SCREEN = 1 << 3,
        ACCELEROMETER = 1 << 4,
        HEADSET = 1 << 5,

        ALL = KEYBOARD | MOUSE | GAME_CONTROLLER | TOUCH_SCREEN | ACCELEROMETER | HEADSET
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(InputDeviceClass)

    struct InputDeviceClassUtilities
    {
        enum { COUNT = 8 }; //Number of values

        static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(InputDeviceClass, InputDeviceClassUtilities::COUNT)& GetLookup();

        static size_t ToIndex(InputDeviceClass deviceClass);

        static const char* ToString(InputDeviceClass deviceClass);
    };

    /** Device components classes. These can be bitwise OR'ed together */
    enum class InputDeviceComponentClass : uint32_t
    {
        NONE = 0,

        BUTTON = 1 << 0,
        AXIS = 1 << 1,
        POV = 1 << 2,
        COUNT = 1 << 3,
        RADIUS = 1 << 4,
        LOCATOR = 1 << 5,

        ALL = BUTTON | AXIS | POV | COUNT | RADIUS | LOCATOR
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(InputDeviceComponentClass)

    /** Device components */
    enum class InputDeviceComponent
    {
        NONE = 0,

        KEYBOARD_KEY,

        MOUSE_BUTTON,
        MOUSE_RELATIVE_AXIS,
        MOUSE_ABSOLUTE_AXIS,

        GAME_CONTROLLER_BUTTON,
        GAME_CONTROLLER_AXIS,
        GAME_CONTROLLER_POV,
        GAME_CONTROLLER_LOCATOR,

        TOUCH_COUNT,
        TOUCH_RELATIVE_AXIS,
        TOUCH_ABSOLUTE_AXIS,

        MULTITOUCH_RELATIVE_RADIUS,
        MULTITOUCH_RELATIVE_AXIS,

        ACCELEROMETER_RELATIVE_AXIS,
        ACCELEROMETER_ABSOLUTE_AXIS,

        LOCATOR,

        COUNT
    };

    struct InputDeviceComponentUtilities
    {
        static bool IsKeyboard(InputDeviceComponent type);
        static bool IsMouse(InputDeviceComponent type);
        static bool IsGameController(InputDeviceComponent type);
        static bool IsTouchScreen(InputDeviceComponent type);
        static bool IsAccelerometer(InputDeviceComponent type);
        static bool IsHeadset(InputDeviceComponent type);

        static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(InputDeviceComponent, InputDeviceComponent::COUNT)& GetLookup();

        static const char* ToString(InputDeviceComponent deviceComponent);

        template <typename T>
        static InputDeviceComponent Parse(const T& deviceComponent)
        {
            return GetLookup().GetOrDefault(deviceComponent, InputDeviceComponent::NONE);
        }

        /** Gets the device class stored in the device type flags */
        static InputDeviceClass GetDeviceClass(InputDeviceComponent deviceComponent);

        /** Gets the device component type stored in the device type flags */
        static InputDeviceComponentClass GetDeviceComponentClass(InputDeviceComponent deviceComponent);
    };

    enum class InputComponentSemantic : uint32_t
    {
        NONE = 0,

        ACCEPT = 1 << 0, //A on Xbox controller

        CANCEL = 1 << 1, //B on Xbox controller

        DEFAULT = 1 << 2, //X on Xbox controller
        HANDBRAKE = 1 << 2, //X on Xbox controller
        RELOAD = 1 << 2, //X on Xbox controller

        CHANGE = 1 << 3, //Y on Xbox controller
        CHANGE_VIEW = 1 << 3, //Y on Xbox controller
        CHANGE_WEAPON = 1 << 3, //Y on Xbox controller

        SETTINGS = 1 << 4, //Start on Xbox controller

        SYSTEM_SETTINGS = 1 << 5, //Back on Xbox controller

        SYSTEM_HOME = 1 << 6, //Home button on Xbox controller

        SHIFT_LEFT = 1 << 7, //L1 on Xbox controller
        GRENADE_LEFT = 1 << 7, //L1 on Xbox controller

        SHIFT_RIGHT = 1 << 8, //R1 on Xbox controller
        GRENADE_RIGHT = 1 << 8, //R1 on Xbox controller

        BRAKE = 1 << 9, //L2 on Xbox controller
        AIM = 1 << 9, //L2 on Xbox controller

        GAS = 1 << 10, //R2 Xbox controller
        FIRE = 1 << 10, //R2 Xbox controller

        TOGGLE_UP = 1 << 11, //D-pad up on Xbox controller
        TOGGLE_DOWN = 1 << 12, //D-pad down on Xbox controller
        TOGGLE_LEFT = 1 << 13, //D-pad left on Xbox controller
        TOGGLE_RIGHT = 1 << 14, //D-pad right on Xbox controller,
        TOGGLE_UP_DOWN = TOGGLE_UP | TOGGLE_DOWN,
        TOGGLE_LEFT_RIGHT = TOGGLE_LEFT | TOGGLE_RIGHT,
        TOGGLE_ALL = TOGGLE_UP | TOGGLE_DOWN | TOGGLE_LEFT | TOGGLE_RIGHT, //D-pad on Xbox controller

        STEER_UP = 1 << 15, //Left stick up on Xbox controller
        MOVE_UP = 1 << 15, //Left stick up on Xbox controller
        STEER_DOWN = 1 << 16, //Left stick down on Xbox controller
        MOVE_DOWN = 1 << 16, //Left stick down on Xbox controller
        STEER_UP_DOWN = STEER_UP | STEER_DOWN, //Left stick up/down on Xbox controller
        MOVE_UP_DOWN = MOVE_UP | MOVE_DOWN, //Left stick up/down on Xbox controller
        STEER_LEFT = 1 << 17, //Left stick left on Xbox controller
        MOVE_LEFT = 1 << 17, //Left stick left on Xbox controller
        STEER_RIGHT = 1 << 18, //Left stick right on Xbox controller
        MOVE_RIGHT = 1 << 18, //Left stick right on Xbox controller
        STEER_LEFT_RIGHT = STEER_LEFT | STEER_RIGHT, //Left stick left/right on Xbox controller
        MOVE_LEFT_RIGHT = MOVE_LEFT | MOVE_RIGHT, //Left stick left/right on Xbox controller
        STEER_ALL = STEER_UP_DOWN | STEER_LEFT_RIGHT, //Left stick on Xbox controller
        MOVE_ALL = MOVE_UP_DOWN | MOVE_LEFT_RIGHT, //Left stick on Xbox controller

        STEER_TOGGLE = 1 << 19, //L3 on Xbox controller
        MOVE_TOGGLE = 1 << 19, //L3 on Xbox controller

        LOOK_UP = 1 << 20, //Right stick up on Xbox controller
        LOOK_DOWN = 1 << 21, //Right stick down on Xbox controller
        LOOK_UP_DOWN = LOOK_UP | LOOK_DOWN, //Right stick up/down on Xbox controller
        LOOK_LEFT = 1 << 22, //Right stick left on Xbox controller
        LOOK_RIGHT = 1 << 23, //Right stick right on Xbox controller
        LOOK_LEFT_RIGHT = LOOK_LEFT | LOOK_RIGHT, //Right stick left/right on Xbox controller
        LOOK_ALL = LOOK_UP_DOWN | LOOK_LEFT_RIGHT, //Right stick on Xbox controller

        LOOK_TOGGLE = 1 << 24, //R3 on Xbox controller

        TOUCH_PAD_BUTTON = 1 << 25, //The touchpad, pressed in, on a PS4 controller

        BRAKE_CLICK = 1 << 26, //Pressing all the way in on the Gamecube L button until it clicks
        AIM_CLICK = 1 << 26, //Pressing all the way in on the Gamecube L button until it clicks

        GAS_CLICK = 1 << 27, //Pressing all the way in on the Gamecube R button until it clicks
        FIRE_CLICK = 1 << 27, //Pressing all the way in on the Gamecube R button until it clicks

        LOCATOR = 1 << 28 //Default sensor for detecting device orientation and position
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(InputComponentSemantic)

    struct InputComponentSemanticUtilities
    {
        enum { COUNT = 57 }; //Number of values InputComponentSemantic

        static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(InputComponentSemantic, COUNT)& GetLookup();

        static const char* ToString(InputComponentSemantic semantic);

        template <typename T>
        static InputComponentSemantic Parse(const T& semantic)
        {
            return GetLookup().GetOrDefault(semantic, InputComponentSemantic::NONE);
        }

        static int GetMoveLookToggleDirection(InputComponentSemantic semantic);
    };

    enum class InputDeviceSemantic : uint32_t
    {
        NONE = 0,

        LEFT_HAND = 1 << 0,
        RIGHT_HAND = 1 << 1
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(InputDeviceSemantic)

    struct InputDeviceSemanticUtilities
    {
        enum { COUNT = 3 }; //Number of values in InputDeviceSemantic

        static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(InputDeviceSemantic, COUNT)& GetLookup();

        static const char* ToString(InputDeviceSemantic semantic);

        template <typename T>
        static InputDeviceSemantic Parse(const T& semantic)
        {
            return GetLookup().GetOrDefault(semantic, InputDeviceSemantic::NONE);
        }
    };

    enum class InputAxisProcessing : uint32_t
    {
        NONE = 0,

        NORMALIZE = 1 << 0,
        LIMIT_MIN_MAX = 1 << 1,
        IS_ABSOLUTE = 1 << 2,
        REST_ON_CLEAR_CHANGES = 1 << 3,
        ACCUMULATE_MULTIPLE_CHANGES = 1 << 4,
        ROUND_UPDATE_TO_ONE = 1 << 5
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(InputAxisProcessing)

    enum class InputButtonProcessing : uint32_t
    {
        NONE = 0,
        REST_ON_CLEAR_CHANGES = 1 << 0,
        EVENT_DRIVEN = 1 << 1
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(InputButtonProcessing)

    class InputAxis
    {
    public:
        InputAxis();

        void Reset(bool isConstructing = false);

        void Update(float value, bool isFirstUpdate = false);

        bool Changed() const;
        void ClearChanged();

        float GetValue() const;
        float GetRelativeValue() const;
        float GetAbsoluteValue() const;

        bool IsFirstDown(float deadZone = 0) const;
        bool IsHoldingDown(float deadZone = 0) const;
        bool IsFirstUp(float deadZone = 0) const;

        float GetRestValue() const;
        InputAxis& SetRestValue(float value);

        float GetMinValue() const;
        InputAxis& SetMinValue(float value);

        float GetMaxValue() const;
        InputAxis& SetMaxValue(float value);

        InputAxis& SetMinMax(float minValue, float maxValue);

        float GetDeadZone() const;
        InputAxis& SetDeadZone(float value);

        size_t GetIndex() const;
        InputAxis& SetIndex(size_t index);

        bool HasCode() const;
        int GetCode() const;
        InputAxis& SetCode(int code);

        const Utf8String& GetDisplayName() const;
        template <typename T>
        InputAxis& SetDisplayName(const T& name)
        {
            this->displayName = name;
            return *this;
        }

        InputComponentSemantic GetSemantic() const;
        InputAxis& SetSemantic(InputComponentSemantic value);

        InputAxisProcessing GetProcessing() const;
        InputAxis& SetProcessing(InputAxisProcessing value);

        bool IsAbsolute() const;
        bool IsRelative() const;

        float GetDirection() const;
        InputAxis& SetDirection(float value);

        bool IsEnabled() const;
        InputAxis& Enable(bool value);

    protected:
        size_t index;
        int code;
        float restValue;
        float minValue;
        float maxValue;
        float deadZone;
        float direction;
        Utf8String displayName;
        InputComponentSemantic semantic;
        InputAxisProcessing processing;

        bool isEnabled;
        Setting<float> previousValue;
        Setting<float> value;
        bool changed;
    };

    class InputButton
    {
    public:
        InputButton();

        void Reset(bool isConstructing = false);

        void Update(bool pressedDown, bool isFirstUpdate = false);

        bool Changed() const;
        void ClearChanged();

        bool GetValue() const;
        bool IsFirstDown() const;
        bool IsHoldingDown() const;
        bool IsFirstUp() const;

        size_t GetIndex() const;
        InputButton& SetIndex(size_t index);

        bool HasCode() const;
        int GetCode() const;
        InputButton& SetCode(int code);

        const Utf8String& GetDisplayName() const;
        template <typename T>
        InputButton& SetDisplayName(const T& name)
        {
            this->displayName = name;
            return *this;
        }

        InputComponentSemantic GetSemantic() const;
        InputButton& SetSemantic(InputComponentSemantic value);

        InputButtonProcessing GetProcessing() const;
        InputButton& SetProcessing(InputButtonProcessing value);

        bool IsEnabled() const;
        InputButton& Enable(bool value);

    protected:
        size_t index;
        int code;
        Utf8String displayName;
        InputComponentSemantic semantic;
        InputButtonProcessing processing;

        bool isEnabled;
        Setting<bool> previousValue;
        bool value;
        bool changed;
    };

    class InputPov
    {
    public:
        InputPov();

        void Reset(bool isConstructing = false);

        void Update(long pov, bool isFirstUpdate = false);

        bool Changed() const;
        void ClearChanged();

        PovDirection GetValue() const;
        bool IsFirstDown(bool strict) const;
        bool IsFirstDown(PovDirection povDirection, bool strict) const;
        bool IsHoldingDown(PovDirection povDirection, bool strict) const;
        bool IsFirstUp(PovDirection povDirection, bool strict) const;

        size_t GetIndex() const;
        InputPov& SetIndex(size_t index);

        bool HasCode() const;
        int GetCode() const;
        InputPov& SetCode(int code);

        const Utf8String& GetDisplayName() const;
        template <typename T>
        InputPov& SetDisplayName(const T& name)
        {
            this->displayName = name;
            return *this;
        }

        InputComponentSemantic GetSemantic() const;
        InputPov& SetSemantic(InputComponentSemantic value);

        bool IsEnabled() const;
        InputPov& Enable(bool value);

    protected:
        size_t index;
        int code;
        Utf8String displayName;
        InputComponentSemantic semantic;

        bool isEnabled;
        Setting<PovDirection> previousValue;
        PovDirection value;
        bool changed;
    };

    class InputLocator
    {
    public:
        InputLocator();

        void Reset(bool isConstructing = false);

        void GetOrientationMatrix33(MathMatrix3& result) const;
        void GetPositionVector3(MathVector3& result, DistanceUnitType distanceUnitType) const;
        void GetVelocityVector3(MathVector3& result, DistanceUnitType distanceUnitType) const;

        size_t GetIndex() const;
        InputLocator& SetIndex(size_t index);

        bool HasCode() const;
        int GetCode() const;
        InputLocator& SetCode(int code);

        const Utf8String& GetDisplayName() const;
        template <typename T>
        InputLocator& SetDisplayName(const T& name)
        {
            this->displayName = name;
            return *this;
        }

        InputComponentSemantic GetSemantic() const;
        InputLocator& SetSemantic(InputComponentSemantic value);

        bool IsEnabled() const;
        InputLocator& Enable(bool value);

    protected:
        size_t index;
        int code;
        Utf8String displayName;
        InputComponentSemantic semantic;

        bool isEnabled;

    public:
        MathMatrix3 orientationMatrix;
        Distance<MathVector3> position;
        Distance<MathVector3> velocity;
    };

    class InputTouchScreen
    {
    public:
        InputTouchScreen();

        void Reset(bool isConstructing = false);

        class Pointer
        {
            friend class InputTouchScreen;

        public:
            Pointer();

            void Reset(bool isConstructing = false);

            void Update(bool down, float x, float y);

            const InputButton& GetContact() const;
            const InputAxis& GetX() const;
            const InputAxis& GetY() const;

            size_t GetButtonCount() const;
            InputButton* GetButton(size_t index);

            size_t GetAxisCount() const;
            InputAxis* GetAxis(size_t index);

            bool Changed() const;
            void ClearChanged();

            int GetID() const;
            PointerType GetPointerType() const;

        protected:
            InputTouchScreen* screen;

            int id;
            PointerType pointerType;

            bool isEnabled;

            InputButton contact;
            std::array<InputAxis, 2> axes;
        };

        Pointer* ConnectPointer(PointerType pointerType, int pointerID);

        Pointer* GetPointer(PointerType pointerType, int pointerID);

        size_t GetPointerCount() const;
        Pointer* GetPointer(size_t index);

        size_t GetPreviousTouchCount() const;
        size_t GetTouchCount() const;
        const InputAxis& GetMultitouchRadius() const;

        size_t GetMultitouchAxisCount() const;
        InputAxis* GetMultitouchAxis(size_t index);

        bool TouchCountChanged() const;
        bool PointersChanged() const;
        bool Changed() const;
        void ClearChanged();

        size_t GetIndex() const;
        InputTouchScreen& SetIndex(size_t index);

        const Utf8String& GetDisplayName() const;
        template <typename T>
        InputTouchScreen& SetDisplayName(const T& name)
        {
            this->displayName = name;
            return *this;
        }

        bool IsConnected() const;
        InputTouchScreen& Connect(bool value);

        void HandleChangedPointer(Pointer* pointer);

    private:
        size_t index;
        Utf8String displayName;

        bool isConnected;

        Setting<size_t> previousTouchCount;
        size_t touchCount;

        std::array<Pointer, InputPointerConstants::MAX_INPUT_POINTERS> allPointers;
        StaticVector<Pointer*, InputPointerConstants::MAX_INPUT_POINTERS> pointers;

        struct Multitouch
        {
            Multitouch();

            void Reset(bool isConstructing = false);

            bool Changed() const;
            void ClearChanged();

            InputButton contact;

            InputAxis centerAxes[2];

            InputAxis radius;

            size_t previousDraggingPointerCount;
            size_t draggingPointerCount;
        };
        Multitouch multitouch;
    };

    class InputAccelerometer
    {
    public:
        static const float ON_TABLE_DEAD_ZONE; //A smaller dead zone suitable for use when setting device on a table
        static const float IN_HAND_DEAD_ZONE; //A larger dead zone suitable for holding device in hand

    public:
        InputAccelerometer();

        void Reset(bool isConstructing = false);

        void Update(float x, float y, float z, bool isFirstUpdate = false);

        const InputAxis& GetX() const;
        const InputAxis& GetY() const;
        const InputAxis& GetZ() const;

        size_t GetAxisCount() const;
        InputAxis* GetAxis(size_t index);

        bool Changed() const;
        void ClearChanged();

        size_t GetIndex() const;
        InputAccelerometer& SetIndex(size_t index);

        const Utf8String& GetDisplayName() const;
        template <typename T>
        InputAccelerometer& SetDisplayName(const T& name)
        {
            this->displayName = name;
            return *this;
        }

        bool IsEnabled() const;
        InputAccelerometer& Enable(bool value);

        float GetDeadZone() const;
        InputAccelerometer& SetDeadZone(float deadZone);

    private:
        size_t index;
        Utf8String displayName;

        bool isEnabled;

        std::array<InputAxis, 3> axes;
    };

    template <typename Button, typename Axis, typename Pov, size_t maxButtonCount, size_t maxAxisCount = 1, size_t maxPovCount = 1>
    struct InputDeviceState
    {
        InputDeviceState()
        {
            this->isConnected = false;
            this->connectionChanged = false;
        }

        void Reset(bool isConstructing = false)
        {
            this->isConnected = false;
            this->connectionChanged = false;

            for (auto& component : this->buttons)
                component.Reset();
            this->buttons.clear();

            for (auto& component : this->axes)
                component.Reset();
            this->axes.clear();

            for (auto& component : this->povs)
                component.Reset();
            this->povs.clear();
        }

        void ClearChanged()
        {
            this->connectionChanged = false;

            for (auto& component : this->buttons)
                component.ClearChanged();

            for (auto& component : this->axes)
                component.ClearChanged();

            for (auto& component : this->povs)
                component.ClearChanged();
        }

        bool isConnected; //Connection state
        bool connectionChanged; //Indicates whether the connection state changed

        StaticVector<Button, maxButtonCount> buttons;
        StaticVector<Axis, maxAxisCount> axes;
        StaticVector<Pov, maxPovCount> povs;
    };

    struct HapticFeedbackSettings
    {
        HapticFeedbackSettings()
        {
            Reset();
        }

        void Reset()
        {
            this->motorIndex = (size_t)-1;
            this->intensity = 0;
            this->decayRate = 0;
            this->duration = 0;
        }

        void Update(SimpleTimeDelta elapsedTime)
        {
            this->duration -= elapsedTime;

            this->intensity -= this->decayRate * elapsedTime;
            this->intensity = std::max(this->intensity, 0.0f);
        }

        bool IsActive() const
        {
            return this->intensity > 0 && this->duration > 0;
        }

        size_t motorIndex;
        float intensity; //Magnitude from 0 to 1
        float decayRate; //Decay per unit of time
        SimpleTimeDelta duration; //Length of time for the effect
    };

    class InputGenericGameController
    {
    public:
        virtual ~InputGenericGameController() {}

        virtual const Utf8String& GetSystemInternalName() const = 0;

        virtual const Utf8String& GetProductDescriptor() const = 0;
        virtual const Utf8String& GetInstanceDescriptor() const = 0;

        virtual InputDeviceSemantic GetSemantic() const = 0;
        virtual void SetSemantic(InputDeviceSemantic value) = 0;

        virtual const Utf8String& GetDisplayName() const = 0;
        virtual void SetDisplayName(const Utf8String& value) = 0;

        virtual bool IsConnected() const = 0;
        virtual bool ConnectionChanged() const = 0;
        virtual bool IsNewConnection() const = 0;

        virtual size_t GetAxisCount() const = 0;
        virtual InputAxis* GetAxis(size_t index) = 0;

        virtual size_t GetPovCount() const = 0;
        virtual InputPov* GetPov(size_t index) = 0;

        virtual size_t GetButtonCount() const = 0;
        virtual InputButton* GetButton(size_t index) = 0;

        virtual size_t GetLocatorCount() const = 0;
        virtual InputLocator* GetLocator(size_t index) = 0;

        virtual void AddHapticFeedback(const HapticFeedbackSettings* forces, size_t count) = 0;
        virtual void StopHapticFeedback() = 0;
    };

    class InputGenericHeadset
    {
    public:
        virtual ~InputGenericHeadset() {}

        virtual const Utf8String& GetSystemInternalName() const = 0;

        virtual const Utf8String& GetProductDescriptor() const = 0;
        virtual const Utf8String& GetInstanceDescriptor() const = 0;

        virtual InputDeviceSemantic GetSemantic() const = 0;
        virtual void SetSemantic(InputDeviceSemantic value) = 0;

        virtual const Utf8String& GetDisplayName() const = 0;
        virtual void SetDisplayName(const Utf8String& value) = 0;

        virtual bool IsConnected() const = 0;
        virtual bool ConnectionChanged() const = 0;
        virtual bool IsNewConnection() const = 0;

        virtual size_t GetLocatorCount() const = 0;
        virtual InputLocator* GetLocator(size_t index) = 0;
    };

} }


//Functions---------------------------------------------------------------------
namespace Finjin { namespace Engine {

    template <typename Device>
    inline InputButton* GetButtonByCode(Device* device, int code)
    {
        if (code > 0 && code < device->GetButtonCount())
        {
            auto component = device->GetButton(code);
            if (component->GetCode() == code)
                return component;
        }

        auto count = device->GetButtonCount();
        for (size_t buttonIndex = 0; buttonIndex < count; buttonIndex++)
        {
            auto component = device->GetButton(buttonIndex);
            if (component->GetCode() == code)
                return component;
        }

        return nullptr;
    }

    template <typename Device>
    inline InputAxis* GetAxisByCode(Device* device, int code)
    {
        if (code > 0 && code < device->GetAxisCount())
        {
            auto component = device->GetAxis(code);
            if (component->GetCode() == code)
                return component;
        }

        auto count = device->GetAxisCount();
        for (size_t axisIndex = 0; axisIndex < count; axisIndex++)
        {
            auto component = device->GetAxis(axisIndex);
            if (component->GetCode() == code)
                return component;
        }

        return nullptr;
    }

    template <typename Device>
    inline InputPov* GetPovByCode(Device* device, int code)
    {
        if (code > 0 && code < device->GetPovCount())
        {
            auto component = device->GetPov(code);
            if (component->GetCode() == code)
                return component;
        }

        auto count = device->GetPovCount();
        for (size_t povIndex = 0; povIndex < count; povIndex++)
        {
            auto component = device->GetPov(povIndex);
            if (component->GetCode() == code)
                return component;
        }

        return nullptr;
    }

    template <typename Device>
    inline InputLocator* GetLocatorByCode(Device* device, int code)
    {
        if (code > 0 && code < device->GetLocatorCount())
        {
            auto component = device->GetLocator(code);
            if (component->GetCode() == code)
                return component;
        }

        auto count = device->GetLocatorCount();
        for (size_t locatorIndex = 0; locatorIndex < count; locatorIndex++)
        {
            auto component = device->GetLocator(locatorIndex);
            if (component->GetCode() == code)
                return component;
        }

        return nullptr;
    }

} }
