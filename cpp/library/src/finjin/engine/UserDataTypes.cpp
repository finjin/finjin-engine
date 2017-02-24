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
#include "finjin/engine/UserDataTypes.hpp"
#include "finjin/common/Convert.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/engine/AssetClassFileReader.hpp"

#define SORT_ENUMS_AND_PROPERTIES 0 //Enabling this is helpful for debugging in some scenarios but isn't really necessary in general

#define FINJIN_USER_DATA_TYPE_ITEM_INVALID_ORDER std::numeric_limits<int>::max()

using namespace Finjin::Engine;


//Local functions--------------------------------------------------------------
static bool IsValidNameChar(char c)
{
    return isalpha(c) || isdigit(c) || c == '_' || c == '-';
}


//Implementation---------------------------------------------------------------

//UserDataEnum::Item
UserDataEnum::Item::Item(Allocator* allocator) : name(allocator), value(allocator)
{
    this->order = FINJIN_USER_DATA_TYPE_ITEM_INVALID_ORDER;
}

bool UserDataEnum::Item::IsValid() const
{
    return !this->name.empty() && !this->value.empty();
}

bool UserDataEnum::Item::operator < (const Item& item) const
{
    if (this->order != FINJIN_USER_DATA_TYPE_ITEM_INVALID_ORDER || item.order != FINJIN_USER_DATA_TYPE_ITEM_INVALID_ORDER)
        return this->order < item.order;

    return this->name < item.name;
}

bool UserDataEnum::Item::operator == (const Item& item) const
{
    return this->name == item.name;
}

//UserDataClass::Item
UserDataClass::Item::Item(Allocator* allocator) : groupName(allocator), name(allocator), displayName(allocator), typeName(allocator), defaultValue(allocator)
{
    this->order = FINJIN_USER_DATA_TYPE_ITEM_INVALID_ORDER;
    this->controlType = UserDataControlType::NONE;
    this->editWidth = 0;
    this->editHeight = 0;
    this->minValue.intValue = 0;
    this->maxValue.intValue = 0;
    this->increment = 0;
}

bool UserDataClass::Item::IsValid() const
{
    return true;
}

bool UserDataClass::Item::operator < (const Item& item) const
{
    if (!this->groupName.empty() || !item.groupName.empty())
        return (this->groupName < item.groupName) ? true : false;

    if (this->order != FINJIN_USER_DATA_TYPE_ITEM_INVALID_ORDER || item.order != FINJIN_USER_DATA_TYPE_ITEM_INVALID_ORDER)
        return this->order < item.order;

    return this->displayName < item.displayName;
}

bool UserDataClass::Item::operator == (const Item& item) const
{
    return (this->name == item.name) ? true : false;
}

//UserDataTypes
UserDataTypes::UserDataTypes(Allocator* allocator, bool useListForStorage) : 
    Super(allocator), 
    name(allocator),
    enums(useListForStorage), 
    classes(useListForStorage)
{
}

UserDataEnum* UserDataTypes::GetEnum(const Utf8String& name)
{
    if (!this->enumsByName.empty())
    {
        auto foundAt = this->enumsByName.find(name);
        if (foundAt != this->enumsByName.end())
            return foundAt->second;
    }
    else
    {
        for (auto& c : this->enums)
        {
            if (c.name == name)
                return &c;
        }
    }

    return nullptr;
}

const UserDataEnum* UserDataTypes::GetEnum(const Utf8String& name) const
{
    if (!this->enumsByName.empty())
    {
        auto foundAt = this->enumsByName.find(name);
        if (foundAt != this->enumsByName.end())
            return foundAt->second;
    }
    else
    {
        for (auto& c : this->enums)
        {
            if (c.name == name)
                return &c;
        }
    }

    return nullptr;
}

UserDataClass* UserDataTypes::GetClass(const Utf8String& name)
{
    if (!this->classesByName.empty())
    {
        auto foundAt = this->classesByName.find(name);
        if (foundAt != this->classesByName.end())
            return foundAt->second;
    }
    else
    {
        for (auto& c : this->classes)
        {
            if (c.name == name)
                return &c;
        }
    }

    return nullptr;
}

const UserDataClass* UserDataTypes::GetClass(const Utf8String& name) const
{
    if (!this->classesByName.empty())
    {
        auto foundAt = this->classesByName.find(name);
        if (foundAt != this->classesByName.end())
            return foundAt->second;
    }
    else
    {
        for (auto& c : this->classes)
        {
            if (c.name == name)
                return &c;
        }
    }

    return nullptr;
}

UserDataClass* UserDataTypes::GetClass(size_t index, UserDataUsage usage)
{
    size_t counter = 0;
    for (auto& c : this->classes)
    {
        if (c.IsSupportedUsage(usage))
        {
            if (index == counter)
                return &c;
            counter++;
        }
    }

    return nullptr;
}

const UserDataClass* UserDataTypes::GetClass(size_t index, UserDataUsage usage) const
{
    size_t counter = 0;
    for (auto& c : this->classes)
    {
        if (c.IsSupportedUsage(usage))
        {
            if (index == counter)
                return &c;
            counter++;
        }
    }

    return nullptr;
}

void UserDataTypes::CreateLookups(Allocator* allocator, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto enumCount = this->enums.size();
    if (!this->enumsByName.Create(enumCount, enumCount, allocator, allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create enum lookup.");
        return;
    }

    for (auto& enumType : this->enums)
    {
        auto insertResult = this->enumsByName.insert(enumType.name, &enumType, false);
        if (insertResult.HasError())
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to insert enum '%1%' into lookup.", enumType.name));
            return;
        }
        if (!insertResult.value)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to insert enum '%1%' into lookup. The name was already present.", enumType.name));
            return;
        }
    }

    auto classCount = this->classes.size();
    if (!this->classesByName.Create(classCount, classCount, allocator, allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create class lookup.");
        return;
    }

    for (auto& classType : this->classes)
    {
        auto insertResult = this->classesByName.insert(classType.name, &classType, false);
        if (insertResult.HasError())
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to insert class '%1%' into lookup.", classType.name));
            return;
        }
        if (!insertResult.value)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to insert class '%1%' into lookup. The name was already present.", classType.name));
            return;
        }
    }
}

void UserDataTypes::ResolveInternalDependencies(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //Resolve enum dependencies
    for (auto& enumType : this->enums)
    {
        for (auto& super : enumType.supers)
        {
            //Super enums
            super.superPointer = GetEnum(super.name);
            if (super.superPointer == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to resolve enum super '%1%'.", super.name));
                return;
            }
        }
    }

    //Resolve class dependencies
    for (auto& classType : this->classes)
    {
        //Properties
        for (auto& prop : classType.items)
            prop.type = UserDataTypes::GetDataType(this, prop, prop.typeName);

        //Super classes
        for (auto& super : classType.supers)
        {
            super.superPointer = GetClass(super.name);
            if (super.superPointer == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to resolve class super '%1%'.", super.name));
                return;
            }
        }
    }
}

UserDataTypes::EnumList& UserDataTypes::GetEnums()
{
    return this->enums;
}

const UserDataTypes::EnumList& UserDataTypes::GetEnums() const
{
    return this->enums;
}

UserDataTypes::ClassList& UserDataTypes::GetClasses()
{
    return this->classes;
}

const UserDataTypes::ClassList& UserDataTypes::GetClasses() const
{
    return this->classes;
}

UserDataPrimitiveType UserDataTypes::GetDataType(UserDataTypes* userDataTypes, const UserDataClass::Item& item, const Utf8String& type)
{
    UserDataPrimitiveType dt;

    if (type.empty() || type == "string")
    {
        //Do nothing
    }
    else if (type.StartsWith("int"))
    {
        dt.type = UserDataPrimitiveType::INT_DATA_TYPE;

        Utf8String countText;
        type.substr(countText, 3);
        dt.count = std::max(1, Convert::ToInteger(countText, 0));
    }
    else if (type.StartsWith("float"))
    {
        dt.type = UserDataPrimitiveType::FLOAT_DATA_TYPE;

        Utf8String countText;
        type.substr(countText, 5);
        dt.count = std::max(1, Convert::ToInteger(countText, 0));
    }
    else if (type == "bool")
        dt.type = UserDataPrimitiveType::BOOL_DATA_TYPE;
    else if (type == "rgb")
        dt.type = UserDataPrimitiveType::RGB_DATA_TYPE;
    else if (type == "rgba")
        dt.type = UserDataPrimitiveType::RGBA_DATA_TYPE;
    else
    {
        //It's hopefully an enum of some sort
        auto enumType = userDataTypes->GetEnum(type);
        if (enumType != nullptr)
            dt = GetDataType(*enumType, type);
    }

    return dt;
}

UserDataPrimitiveType UserDataTypes::GetDataType(UserDataEnum& e, const Utf8String& type)
{
    UserDataPrimitiveType dt;
    dt.enumType = &e;

    if (type.empty() || type == "string")
    {
        //Do nothing
    }
    else if (type.StartsWith("int"))
    {
        dt.type = UserDataPrimitiveType::INT_DATA_TYPE;

        Utf8String countText;
        type.substr(countText, 3);
        dt.count = std::max(1, Convert::ToInteger(countText, 0));
    }
    else if (type.StartsWith("float"))
    {
        dt.type = UserDataPrimitiveType::FLOAT_DATA_TYPE;

        Utf8String countText;
        type.substr(countText, 5);
        dt.count = std::max(1, Convert::ToInteger(countText, 0));
    }
    else if (type == "bool")
        dt.type = UserDataPrimitiveType::BOOL_DATA_TYPE;
    else if (type == "rgb")
        dt.type = UserDataPrimitiveType::RGB_DATA_TYPE;
    else if (type == "rgba")
        dt.type = UserDataPrimitiveType::RGBA_DATA_TYPE;

    return dt;
}

UserDataControlType UserDataTypes::GetControlType(const UserDataClass::Item& item, const Utf8String& controlType)
{
    if (item.type.enumType != nullptr)
    {
        //Enums always use a combo box
        return UserDataControlType::COMBO;
    }
    else
    {
        switch (item.type.type)
        {
            case UserDataPrimitiveType::STRING_DATA_TYPE:
            {
                return UserDataControlType::EDIT;
            }
            case UserDataPrimitiveType::INT_DATA_TYPE:
            {
                if (controlType == "edit")
                    return UserDataControlType::EDIT;
                else if (controlType == "spinner")
                    return UserDataControlType::SPINNER;
                else
                    return UserDataControlType::SPINNER;
            }
            case UserDataPrimitiveType::FLOAT_DATA_TYPE:
            {
                if (controlType == "edit")
                    return UserDataControlType::EDIT;
                else if (controlType == "spinner")
                    return UserDataControlType::SPINNER;
                else
                    return UserDataControlType::SPINNER;
            }
            case UserDataPrimitiveType::BOOL_DATA_TYPE:
            {
                return UserDataControlType::CHECK;
            }
            case UserDataPrimitiveType::RGB_DATA_TYPE:
            {
                return UserDataControlType::RGB_SWATCH;
            }
            case UserDataPrimitiveType::RGBA_DATA_TYPE:
            {
                return UserDataControlType::RGBA_SWATCH;
            }
        }
    }

    return UserDataControlType::EDIT;
}

void UserDataTypes::FixName(Utf8String& name)
{
    for (size_t i = 0; i < name.length(); i++)
    {
        if (!IsValidNameChar(name[i]))
            name[i] = ' ';
    }

    name.RemoveWhitespace();
}
