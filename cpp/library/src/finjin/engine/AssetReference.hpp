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
#include "finjin/common/Error.hpp"
#include "finjin/common/Path.hpp"
#include "finjin/common/StaticVector.hpp"
#include "finjin/common/Uri.hpp"
#include "finjin/common/Utf8String.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class AssetReference
    {
    public:
        static const AssetReference& GetEmpty();

        AssetReference(Allocator* allocator = nullptr);

        bool Create(Allocator* allocator);
        void Destroy();

        void Clear();

        bool IsValid() const;

        template <typename T>
        void ForUriString(const T& s, SimpleUri& tempUri, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if ((tempUri = s).HasError())
            {
                FINJIN_SET_ERROR(error, "Unexpected error while parsing URI.");
                return;
            }

            if (!tempUri.IsValid())
            {
                FINJIN_SET_ERROR(error, "Failed to parse URI.");
                return;
            }

            if (ForUri(tempUri).HasError())
            {
                FINJIN_SET_ERROR(error, "Failed to assign URI components.");
                return;
            }
        }

        template <typename T>
        ValueOrError<void> ForUri(const T& uri)
        {
            Clear();

            if (uri.IsValid())
            {
                if (uri.GetScheme().empty() || uri.GetScheme() == "file")
                {
                    if (uri.GetPath().StartsWith("/"))
                    {
                        if (this->filePath.assign(uri.GetPath().c_str() + 1).HasError())
                            return ValueOrError<void>::CreateError();
                    }
                    else
                    {
                        if (this->filePath.assign(uri.GetPath()).HasError())
                            return ValueOrError<void>::CreateError();
                    }

                    if (!uri.GetScheme().empty())
                    {
                        if (this->objectName.assign(uri.GetFragment()).HasError())
                            return ValueOrError<void>::CreateError();
                    }
                }
                else if (uri.GetScheme() == "object")
                {
                    if (this->objectName.assign(uri.GetHost()).HasError())
                        return ValueOrError<void>::CreateError();
                }
            }

            return ValueOrError<void>();
        }

        ValueOrError<void> ForObject(const Utf8String& objectName)
        {
            Clear();
            return this->objectName = objectName;
        }

        template <typename T>
        ValueOrError<void> ForLocalFile(const T& filePath)
        {
            Clear();
            return this->filePath = filePath;
        }

        template <typename T>
        ValueOrError<void> ForObjectInLocalFile(const T& filePath, const Utf8String& objectName)
        {
            Clear();

            if (this->filePath.assign(filePath).HasError())
                return ValueOrError<void>::CreateError();
            if (this->objectName.assign(objectName).HasError())
                return ValueOrError<void>::CreateError();

            return ValueOrError<void>();
        }

        ValueOrError<void> ToSimpleUri(SimpleUri& result) const;
        SimpleUri ToSimpleUri() const;

        ValueOrError<void> ToUriString(Utf8String& result) const;
        Utf8String ToUriString() const;

    public:
        Path filePath;
        Utf8String objectName;
    };

    template <size_t maxCount>
    struct AssetReferences : StaticVector<AssetReference, maxCount>
    {
        using Super = StaticVector<AssetReference, maxCount>;

        AssetReferences(Allocator* allocator = nullptr)
        {
            if (allocator != nullptr)
                Create(allocator);
        }

        void Create(Allocator* allocator)
        {
            for (auto item = Super::begin(); item != Super::max_end(); ++item)
                item->Create(allocator);
        }

        void Destroy()
        {
            for (auto item = Super::begin(); item != Super::max_end(); ++item)
                item->Destroy();
        }

        template <typename T>
        void AddLocalFile(const T& value, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (Super::full())
            {
                FINJIN_SET_ERROR(error, "Failed to add asset reference. Collection is full.");
                return;
            }

            Super::push_back();
            auto& assetRef = Super::back();
            if (assetRef.filePath.assign(value).HasError())
            {
                Super::pop_back();

                FINJIN_SET_ERROR(error, "Failed to assign file asset reference.");
                return;
            }

            assetRef.objectName.clear();
        }
    };

} }
