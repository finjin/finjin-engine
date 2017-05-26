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
#include "AssetReference.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
AssetReference::AssetReference(Allocator* allocator) : filePath(allocator), objectName(allocator)
{
}

const AssetReference& AssetReference::GetEmpty()
{
    static const AssetReference value;
    return value;
}

bool AssetReference::Create(Allocator* allocator)
{
    auto result = true;
    result &= this->filePath.Create(allocator);
    result &= this->objectName.Create(allocator);
    return result;
}

void AssetReference::Destroy()
{
    this->filePath.Destroy();
    this->objectName.Destroy();
}

void AssetReference::Clear()
{
    this->filePath.clear();
    this->objectName.clear();
}

bool AssetReference::IsValid() const
{
    return !this->filePath.empty() || !this->objectName.empty();
}

ValueOrError<void> AssetReference::ToSimpleUri(SimpleUri& result) const
{
    result.clear();

    if (!this->filePath.empty())
    {
        if (result.SetScheme("file").HasError())
            return ValueOrError<void>::CreateError();
        if (!this->filePath.empty())
        {
            if (this->filePath.StartsWith("/"))
            {
                if (result.SetPath(this->filePath.ToString()).HasError())
                    return ValueOrError<void>::CreateError();
            }
            else
            {
                if (result.path.assign("/").HasError())
                    return ValueOrError<void>::CreateError();
                if (result.path.append(this->filePath.c_str()).HasError())
                    return ValueOrError<void>::CreateError();
            }
        }
        if (!this->objectName.empty())
        {
            if (result.SetFragment(this->objectName).HasError())
                return ValueOrError<void>::CreateError();
        }
    }
    else if (!this->objectName.empty())
    {
        if (result.SetScheme("object").HasError())
            return ValueOrError<void>::CreateError();
        if (result.SetHost(this->objectName).HasError())
            return ValueOrError<void>::CreateError();
    }

    return ValueOrError<void>();
}

SimpleUri AssetReference::ToSimpleUri() const
{
    SimpleUri result;
    ToSimpleUri(result);
    return result;
}

ValueOrError<void> AssetReference::ToUriString(Utf8String& result) const
{
    return ToSimpleUri().ToString(result);
}

Utf8String AssetReference::ToUriString() const
{
    Utf8String result;
    ToUriString(result);
    return result;
}
