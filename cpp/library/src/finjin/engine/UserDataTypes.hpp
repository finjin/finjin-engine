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
#include "finjin/common/AllocatedClass.hpp"
#include "finjin/common/AllocatedQueue.hpp"
#include "finjin/common/AllocatedUnorderedMap.hpp"
#include "finjin/common/AllocatedUnorderedSet.hpp"
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/ByteBuffer.hpp"
#include "finjin/common/ChunkName.hpp"
#include "finjin/common/ConfigDocumentReader.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/EnumBitwise.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/Asset.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    /** The different ways the class can be used. */
    enum class UserDataUsage
    {
        /** Use the class for anything. */
        ANY = 0,

        /** Use the class with object settings. */
        OBJECT = 1 << 0,

        /** Use the class with material settings. */
        MATERIAL = 1 << 1,

        /** Use the class with scene settings. */
        SCENE = 1 << 2,

        /** Do not allow the class to be seen. */
        PRIVATE = 1 << 3
    };
    FINJIN_ENUM_BITWISE_OPERATIONS(UserDataUsage)

    /** A control type. */
    enum class UserDataControlType
    {
        NONE,
        EDIT,
        COMBO,
        CHECK,
        RGB_SWATCH,
        RGBA_SWATCH,
        SPINNER
    };

    struct UserDataTypesConstants
    {
        enum { MAX_SUPERS = 4 };
        enum { MAX_USAGES = 4 }; //Number of non-zero entries in UserDataUsage enum 
        enum { MAX_SECTION_DEPTH = 3 };
    };

    template <typename T>
    class UserDataItemsStateBase
    {
    public:
        void Create(size_t maxObjects, Allocator* allocator, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (!this->visited.Create(maxObjects, maxObjects, allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate 'visited' list.");
                return;
            }

            if (!this->toVisit.Create(maxObjects, allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate 'to visit' queue.");
                return;
            }

            if (!this->inOrderObjects.CreateEmpty(maxObjects, allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate 'in order objects' list.");
                return;
            }
        }

        void Clear()
        {
            this->visited.clear();
            this->toVisit.clear();
            this->inOrderObjects.clear();
        }

        bool HasItemsToVisit() const
        {
            return !this->toVisit.empty();
        }

        ValueOrError<bool> ScheduleToVisit(T* obj)
        {
            if (!this->visited.contains(obj))
            {
                if (this->toVisit.full())
                    return ValueOrError<bool>::CreateError();
                else
                {
                    this->toVisit.push(obj);
                    return true;
                }
            }
            else
                return false;
        }

        template <typename ITEMS>
        ValueOrError<void> ScheduleSupersToVisit(ITEMS& supers)
        {
            //This is done in reverse order so that when in-order list is walked, the objects will be in the correct order
            for (size_t i = supers.size() - 1; i != (size_t)-1; i--)
            {
                if (ScheduleToVisit(supers[i].superPointer).HasError())
                    return ValueOrError<void>::CreateError();
            }

            return ValueOrError<void>();
        }

        T* GetToVisit()
        {
            T* current = nullptr;
            this->toVisit.pop(current);
            this->visited.insert(current);
            return current;
        }

        ValueOrError<void> AddInOrder(T* obj)
        {
            if (this->inOrderObjects.full())
                return ValueOrError<void>::CreateError();

            this->inOrderObjects.push_back(obj);
            return ValueOrError<void>();
        }

    protected:
        AllocatedUnorderedSet<T*> visited;
        AllocatedQueue<T*> toVisit;
        AllocatedVector<T*> inOrderObjects;
    };

    template <typename T>
    class UserDataItemsState : public UserDataItemsStateBase<T>
    {
    public:
        using Super = UserDataItemsStateBase<T>;
        using Items = AllocatedVector<typename T::Item>;
        using iterator = typename Items::iterator;

        void Create(size_t maxObjects, size_t maxItems, Allocator* allocator, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            Super::Create(maxObjects, allocator, error);
            if (error)
            {
                FINJIN_SET_ERROR_NO_MESSAGE(error);
                return;
            }

            if (!this->items.CreateEmpty(maxItems, allocator, allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate items.");
                return;
            }
        }

        void Clear()
        {
            Super::Clear();
            this->items.clear();
        }

        void Finish()
        {
            for (size_t i = this->inOrderObjects.size() - 1; i != (size_t)-1; i--)
            {
                auto obj = this->inOrderObjects[i];

                for (auto item : obj->items)
                {
                    auto foundAt = this->items.find(item);
                    if (foundAt == items.end())
                        this->items.push_back(item);
                    else
                        *foundAt = item;
                }
            }

            std::stable_sort(this->items.begin(), this->items.end());
        }

        iterator begin() { return this->items.begin(); }
        iterator end() { return this->items.end(); }
        size_t max_size() const { return this->items.max_size(); }
        size_t size() const { return this->items.size(); }
        size_t empty() const { return this->items.empty(); }
        typename T::Item& operator [] (size_t index) { return this->items[index]; }
        const typename T::Item& operator [] (size_t index) const { return this->items[index]; }

    private:
        Items items;
    };

    template <typename T>
    class UserDataItemsStatePointers : public UserDataItemsStateBase<T>
    {
    public:
        using Super = UserDataItemsStateBase<T>;
        using Items = AllocatedVector<typename T::Item*>;
        using iterator = typename Items::iterator;

        void Create(size_t maxObjects, size_t maxItems, Allocator* allocator, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            Super::Create(maxObjects, allocator, error);
            if (error)
            {
                FINJIN_SET_ERROR_NO_MESSAGE(error);
                return;
            }

            if (!this->items.CreateEmpty(maxItems, allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate items.");
                return;
            }
        }

        void Clear()
        {
            Super::Clear();
            this->items.clear();
        }

        void Finish()
        {
            for (size_t i = this->inOrderObjects.size() - 1; i != (size_t)-1; i--)
            {
                auto obj = this->inOrderObjects[i];

                for (auto item : obj->items)
                {
                    auto foundAt = (size_t)-1;
                    for (size_t i = 0; i < this->items.size(); i++)
                    {
                        if (*this->items[i] == item)
                        {
                            foundAt = i;
                            break;
                        }
                    }
                    if (foundAt == (size_t)-1)
                        this->items.push_back(&item);
                    else
                        this->items[foundAt] = &item;
                }
            }

            std::stable_sort(this->items.begin(), this->items.end(), ItemLess);
        }

        static bool ItemLess(const typename T::Item* item1, const typename T::Item* item2)
        {
            return *item1 < *item2;
        }

        iterator begin() { return this->items.begin(); }
        iterator end() { return this->items.end(); }
        size_t max_size() const { return this->items.max_size(); }
        size_t size() const { return this->items.size(); }
        size_t empty() const { return this->items.empty(); }
        typename T::Item* operator [] (size_t index) { return this->items[index]; }
        const typename T::Item* operator [] (size_t index) const { return this->items[index]; }

    private:
        Items items;
    };

    template <typename T, typename State>
    void _GetUserDataItems(T* object, State& itemsState, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        itemsState.Clear();

        if (itemsState.ScheduleToVisit(object).HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to schedule item for visit.");
            return;
        }

        while (itemsState.HasItemsToVisit())
        {
            auto current = itemsState.GetToVisit();

            if (itemsState.AddInOrder(current).HasError())
            {
                FINJIN_SET_ERROR(error, "Failed to add item in order.");
                return;
            }

            if (itemsState.ScheduleSupersToVisit(current->supers).HasError())
            {
                FINJIN_SET_ERROR(error, "Failed to schedule item supers to visit.");
                return;
            }
        }

        itemsState.Finish();
    }

    class UserDataEnum;

    /** A data type. */
    struct UserDataPrimitiveType
    {
    public:
        enum Type
        {
            STRING_DATA_TYPE,
            INT_DATA_TYPE,
            FLOAT_DATA_TYPE,
            BOOL_DATA_TYPE,
            RGB_DATA_TYPE,
            RGBA_DATA_TYPE
        };

        UserDataPrimitiveType()
        {
            Reset();
        }

        void Reset()
        {
            this->type = STRING_DATA_TYPE;
            this->count = 1;
            this->enumType = nullptr;
        }

    public:
        /** The basic type. */
        Type type;

        /** The number of values. Only relevant for INT_DATA_TYPE and FLOAT_DATA_TYPE. */
        int count;

        /** Pointer to an enum that constrains the values. May be null. */
        UserDataEnum* enumType;
    };

    /**
    * An enum is a list of predefined named values that constrain the values
    * that a data type can take on.
    */
    class UserDataEnum : public AllocatedClass
    {
    public:
        UserDataEnum(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
            this->next = nullptr;
            for (auto item = this->supers.begin(); item != this->supers.max_end(); ++item)
                item->name.Create(allocator);
        }

        /** A single enumeration item. */
        struct Item
        {
        public:
            Item(Allocator* allocator);

            bool IsValid() const;

            /** Determines if this item is less than the other item. Used for sorting. */
            bool operator < (const Item& item) const;

            /** Determines if this item is equal to the other item. */
            bool operator == (const Item& item) const;

        public:
            /** Explicit order for the enum. Ignored by default. */
            int order;

            /** The name of the item. This is displayed. */
            Utf8String name;

            /** The value of the item. */
            Utf8String value;
        };

        /**
        * Gets all the enum items for this enum, including the items in supers
        * @param items [out] - The retrieve items, can be an instance of HandwrittenUserDataTypesReader::EnumItems or HandwrittenUserDataTypesReader::EnumItemPointers.
        * Regardless of which is used, Create() should be called on it beforehand to preallocated it.
        */
        template <typename T>
        void GetEnumValues(T& items, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            _GetUserDataItems(this, items, error);
            if (error)
                FINJIN_SET_ERROR(error, "Failed to get enum values.");
        }

    public:
        /** The name of the enum. */
        Utf8String name;

        /** The enum data type. */
        UserDataPrimitiveType type;

        /** Pointers to enums that this enum inherits from. */
        struct SuperReference
        {
            SuperReference() : superPointer(nullptr) {}

            Utf8String name;
            UserDataEnum* superPointer;
        };
        StaticVector<SuperReference, UserDataTypesConstants::MAX_SUPERS> supers;

        /** All the enum items. Does not include the items from supers. */
        AllocatedVector<Item> items;

        UserDataEnum* next;
    };

    class UserDataClass : public AllocatedClass
    {
    public:
        UserDataClass(Allocator* allocator) : AllocatedClass(allocator), name(allocator), displayName(allocator)
        {
            this->usage = UserDataUsage::ANY;
            this->next = nullptr;
            for (auto item = this->supers.begin(); item != this->supers.max_end(); ++item)
                item->name.Create(allocator);            
        }

        /** Determines whether the class can be used in the specified way. */
        bool IsSupportedUsage(UserDataUsage usage) const
        {
            if (AnySet(this->usage & UserDataUsage::PRIVATE))
                return false;
            else
                return (this->usage == UserDataUsage::ANY || usage == UserDataUsage::ANY) ? true : AnySet(this->usage & usage);
        }

        /** A data item */
        struct Item
        {
        public:
            Item(Allocator* allocator);

            bool IsValid() const;

            /** Determines if this item is less than the other item. Used for sorting. */
            bool operator < (const Item& item) const;

            /** Determines if this item is equal to the other item. */
            bool operator == (const Item& item) const;

        public:
            /** The group that the item is assigned to. */
            Utf8String groupName;

            /** Explicit order for the item. Ignored by default. */
            int order;

            /** The internal name of the item. */
            Utf8String name;

            /** The display name of the item. */
            Utf8String displayName;

            Utf8String typeName;

            /** The item data type. */
            UserDataPrimitiveType type;

            /** Default value, formatted as a string. */
            Utf8String defaultValue;

            /** The type of control to be used to display/edit item. */
            UserDataControlType controlType;

            /** The width of the edit box used to edit item. Only relevant for some types. */
            int editWidth;

            /** The height of the edit box used to edit item. Only relevant for some types. */
            int editHeight;

            union Value
            {
                int intValue;
                float floatValue;
            };

            /** The minimum allowed value. Only relevant for INT_DATA_TYPE and FLOAT_DATA_TYPE. */
            Value minValue;

            /** The maximum allowed value. Only relevant for INT_DATA_TYPE and FLOAT_DATA_TYPE. */
            Value maxValue;

            /** The spinner increment. Only relevant for INT_DATA_TYPE and FLOAT_DATA_TYPE with a SPINNER_CONTROL_TYPE. */
            float increment;
        };

        /**
        * Gets all the data items for this class, including the items in supers
        * @param items [out] - The retrieve items, can be an instance of HandwrittenUserDataTypesReader::ClassProperties or HandwrittenUserDataTypesReader::ClassPropertyPointers.
        * Regardless of which is used, Create() should be called on it beforehand to preallocated it.
        */
        template <typename T>
        void GetProperties(T& items, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            _GetUserDataItems(this, items, error);
            if (error)
                FINJIN_SET_ERROR(error, "Failed to get properties.");
        }

    public:
        /** The class usage. */
        UserDataUsage usage;

        /** The internal class name. */
        Utf8String name;

        /** The display name. */
        Utf8String displayName;

        /** Pointers to classes that this class inherits from. */
        struct SuperReference
        {
            SuperReference() : superPointer(nullptr) {}

            Utf8String name;
            UserDataClass* superPointer;
        };
        StaticVector<SuperReference, UserDataTypesConstants::MAX_SUPERS> supers;

        /** All the items. Does not include the items from supers. */
        AllocatedVector<Item> items;

        UserDataClass* next;
    };

    using UserDataEnumItems = UserDataItemsState<UserDataEnum>;
    using UserDataEnumItemPointers = UserDataItemsStatePointers<UserDataEnum>;

    using UserDataClassProperties = UserDataItemsState<UserDataClass>;
    using UserDataClassPropertyPointers = UserDataItemsStatePointers<UserDataClass>;

    struct UserDataClassInstance
    {
        struct PropertyInstance : AllocatedClass
        {
            PropertyInstance(Allocator* allocator, UserDataClass::Item* prop) : AllocatedClass(allocator)
            {
                this->userDataProperty = prop;
                if (this->userDataProperty != nullptr)
                {
                    this->chunkPropertyNameID = ChunkPropertyName(this->userDataProperty->name.c_str()).id;
                    SetValue(this->userDataProperty->defaultValue);
                }
                else
                    this->chunkPropertyNameID = 0;
            }

            virtual ~PropertyInstance() {}

            virtual ValueOrError<void> GetString(Utf8String& result) const = 0;
            virtual int GetInt() const = 0;
            virtual float GetFloat() const = 0;
            virtual bool GetBool() const = 0;
            virtual MathVector4 GetRGBA() const = 0;

            void ResetValue() { SetValue(this->userDataProperty != nullptr ? this->userDataProperty->defaultValue : Utf8String::Empty()); }
            virtual void SetValue(const Utf8String& s) {}

            UserDataClass::Item* userDataProperty;
            ChunkPropertyName::ID chunkPropertyNameID;
        };
        struct StringPropertyInstance : public PropertyInstance
        {
            StringPropertyInstance(Allocator* allocator, UserDataClass::Item* prop) : PropertyInstance(allocator, prop), value(allocator) {}

            ValueOrError<void> GetString(Utf8String& result) const override { return result = this->value; }
            int GetInt() const override { return Convert::ToInteger(this->value, (int)0); }
            float GetFloat() const override { return Convert::ToNumber(this->value, 0.0f); }
            bool GetBool() const override { return Convert::ToBool(this->value, false); }
            MathVector4 GetRGBA() const override
            {
                MathVector4 values = { 0, 0, 0, 1 };

                size_t count = 0;
                Split(this->value, ' ', [&values, &count](Utf8StringView& value)
                {
                    if (count < 4)
                        values(count++) = Convert::ToNumber(value.ToString(), 0.0f);

                    return ValueOrError<bool>(true);
                });

                return values;
            }

            void SetValue(const Utf8String& s) override { this->value = s; }

            Utf8String value;
        };
        struct IntPropertyInstance : public PropertyInstance
        {
            IntPropertyInstance(Allocator* allocator, UserDataClass::Item* prop) : PropertyInstance(allocator, prop), value(0) {}

            ValueOrError<void> GetString(Utf8String& result) const override { return result = Convert::ToString(this->value); }
            int GetInt() const override { return this->value; }
            float GetFloat() const override { return RoundToFloat(this->value); }
            bool GetBool() const override { return this->value ? true : false; }
            MathVector4 GetRGBA() const override { auto f = GetFloat(); return { f, f, f, 1.0f }; }

            void SetValue(const Utf8String& s) override { this->value = Convert::ToInteger(s, (int)0); }

            int value;
        };
        struct FloatPropertyInstance : public PropertyInstance
        {
            FloatPropertyInstance(Allocator* allocator, UserDataClass::Item* prop) : PropertyInstance(allocator, prop), value(0) {}

            ValueOrError<void> GetString(Utf8String& result) const override { return result = Convert::ToString(this->value); }
            int GetInt() const override { return RoundToInt(this->value); }
            float GetFloat() const override { return this->value; }
            bool GetBool() const override { return this->value != 0.0f ? true : false; }
            MathVector4 GetRGBA() const override { return{ this->value, this->value, this->value, 1.0f }; }

            void SetValue(const Utf8String& s) override { this->value = Convert::ToNumber(s, 0.0f); }

            float value;
        };
        struct BoolPropertyInstance : public PropertyInstance
        {
            BoolPropertyInstance(Allocator* allocator, UserDataClass::Item* prop) : PropertyInstance(allocator, prop), value(false) {}

            ValueOrError<void> GetString(Utf8String& result) const override { return result = Convert::ToString(this->value); }
            int GetInt() const override { return this->value ? 1 : 0; }
            float GetFloat() const override { return this->value ? 1.0f : 0.0f; }
            bool GetBool() const override { return this->value; }
            MathVector4 GetRGBA() const override { auto f = GetFloat(); return { f, f, f, 1.0f }; }

            void SetValue(const Utf8String& s) override { this->value = Convert::ToBool(s, false); }

            bool value;
        };
        struct RGBAPropertyInstance : public PropertyInstance
        {
            RGBAPropertyInstance(Allocator* allocator, UserDataClass::Item* prop) : PropertyInstance(allocator, prop), value{ 0,0,0,1 } {}

            ValueOrError<void> GetString(Utf8String& result) const override
            {
                result.clear();

                for (size_t i = 0; i < 4; i++)
                {
                    if (i > 0)
                    {
                        if (result.append(' ').HasError())
                            return ValueOrError<void>::CreateError();
                    }
                    if (result.append(Convert::ToString(this->value[i])).HasError())
                        return ValueOrError<void>::CreateError();
                }

                return ValueOrError<void>();
            }
            int GetInt() const override { return RoundToInt(this->value(0)); }
            float GetFloat() const override { return this->value(0); }
            bool GetBool() const override { return this->value(0) != 0.0f ? true : false; }
            MathVector4 GetRGBA() const override { return this->value; }

            void SetValue(const Utf8String& s) override
            {
                this->value = MathVector4(0.0f, 0.0f, 0.0f, 1.0f);

                size_t count = 0;
                Split(s, ' ', [this, &count](Utf8StringView& value)
                {
                    if (count < 4)
                        this->value(count++) = Convert::ToNumber(value.ToString(), 0.0f);

                    return ValueOrError<bool>(true);
                });
            }

            MathVector4 value;
        };

        UserDataClassInstance(Allocator* allocator) : userDataClassName(allocator)
        {
            this->userDataClass = nullptr;
        }

        ~UserDataClassInstance()
        {
            for (auto inst : this->propertyInstances)
                delete inst;
        }

        Utf8String userDataClassName;
        UserDataClass* userDataClass;
        AllocatedVector<PropertyInstance*> propertyInstances;
    };

    class FINJIN_ASSET_CLASS(UserDataTypes) : public AllocatedClass
    {
    public:
        using Super = AllocatedClass;

        using EnumList = IntrusiveSingleList<UserDataEnum, NextAccessor<UserDataEnum> >;
        using ClassList = IntrusiveSingleList<UserDataClass, NextAccessor<UserDataClass> >;

        UserDataTypes(Allocator* allocator, bool useListForStorage);

        UserDataEnum* GetEnum(const Utf8String& name);
        const UserDataEnum* GetEnum(const Utf8String& name) const;

        UserDataClass* GetClass(const Utf8String& name);
        const UserDataClass* GetClass(const Utf8String& name) const;

        UserDataClass* GetClass(size_t index, UserDataUsage usage);
        const UserDataClass* GetClass(size_t index, UserDataUsage usage) const;

        void CreateLookups(Allocator* allocator, Error& error);
        
        void ResolveInternalDependencies(Error& error);

        EnumList& GetEnums();
        const EnumList& GetEnums() const;

        ClassList& GetClasses();
        const ClassList& GetClasses() const;

        //Determines a class item's data type.
        static UserDataPrimitiveType GetDataType(UserDataTypes* userDataTypes, const UserDataClass::Item& item, const Utf8String& type);

        //Determines an enum's data type.
        static UserDataPrimitiveType GetDataType(UserDataEnum& e, const Utf8String& type);

        //Determines the appropriate control type for a class item.
        static UserDataControlType GetControlType(const UserDataClass::Item& item, const Utf8String& controlType);

        static void FixName(Utf8String& name);

    public:
        Utf8String name;

        //All the enums.
        EnumList enums;
        AllocatedUnorderedMap<Utf8String, UserDataEnum*> enumsByName;

        //All the classes.
        ClassList classes;
        AllocatedUnorderedMap<Utf8String, UserDataClass*> classesByName;
    };
    
} }
