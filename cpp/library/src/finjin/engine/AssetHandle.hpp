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


//Includes---------------------------------------------------------------------
#include "finjin/common/Allocator.hpp"
#include "finjin/common/IntrusiveList.hpp"
#include "finjin/engine/AssetReference.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    template <typename T>
    struct AssetDeleter
    {
        void operator () (T* item)
        {
            delete item;
        }
    };
    
    template <>
    struct AssetDeleter<void>
    {
        void operator () (void* item)
        {
            //Do nothing
        }
    };
    
    template <typename T>
    struct AssetHandle
    {
        AssetHandle(const AssetHandle& other) = delete;
        AssetHandle(AssetHandle&& other) = delete;

        AssetHandle& operator = (const AssetHandle& other) = delete;
        AssetHandle& operator = (AssetHandle&& other) = delete;

        AssetHandle(Allocator* allocator) : assetRef(allocator), asset(nullptr)
        {
            this->referenceNext = nullptr;
            this->internalDependencyNext = nullptr;
            this->allocatedNext = nullptr;
            this->queryNext = nullptr;
            this->newNext = nullptr;
            this->isOwner = false;
        }

        ~AssetHandle()
        {
            Destroy();
        }

        bool Create(Allocator* allocator)
        {
            return this->assetRef.Create(allocator);
        }

        void Destroy()
        {
            this->assetRef.Destroy();

            if (this->isOwner)
            {
                AssetDeleter<T> deleteAsset;
                deleteAsset(this->asset);
                this->asset = nullptr;

                this->isOwner = false;
            }

            this->referenceNext = nullptr;
            this->internalDependencyNext = nullptr;
            this->allocatedNext = nullptr;
            this->queryNext = nullptr;
            this->newNext = nullptr;
        }

        AssetReference assetRef;
        T* asset;
        AssetHandle* referenceNext; //A linked list of references, all of which share the same asset. After a scene is loaded the first item should have isOwner=true, while all others are isOwner=false
        AssetHandle* internalDependencyNext; //A linked list of asset handles that contain an asset that must have internal dependencies resolved after the scene is loaded. After resolution, the list is cleared
        AssetHandle* allocatedNext; //A linked list of dynamically allocated assets. These must be deleted when the scene is destroyed
        AssetHandle* queryNext; //A linked list of queried assets. This is a very temporary connection and should not be expected to exist between queries of the same asset class
        AssetHandle* newNext; //A linked list of newly created assets
        bool isOwner;
    };

    //Results from a generic "get"
    template <typename T>
    class QueryAssetAccessor
    {
    public:
        using value_type = T;

        static inline AssetHandle<T>* GetNext(AssetHandle<T>* item) { return item ? item->queryNext : nullptr; }
        static inline const AssetHandle<T>* GetNext(const AssetHandle<T>* item) { return item ? item->queryNext : nullptr; }

        static inline void SetNext(AssetHandle<T>* item, AssetHandle<T>* next) { item->queryNext = next; }

        static inline value_type& GetReference(AssetHandle<T>* item) { return *item->asset; }
        static inline value_type* GetPointer(AssetHandle<T>* item) { return item->asset; }

        static inline const value_type& GetConstReference(const AssetHandle<T>* item) { return *item->asset; }
        static inline const value_type* GetConstPointer(const AssetHandle<T>* item) { return item->asset; }
    };

    template <typename T>
    using QueryAssetsList = IntrusiveSingleList<AssetHandle<T>, QueryAssetAccessor<T> >;

    template <typename T>
    using QueryAssetsResult = IntrusiveSingleListResult<QueryAssetsList<T> >;


    //Results from a "get new"
    template <typename T>
    class NewAssetAccessor
    {
    public:
        using value_type = T;

        static inline AssetHandle<T>* GetNext(AssetHandle<T>* item) { return item ? item->newNext : nullptr; }
        static inline const AssetHandle<T>* GetNext(const AssetHandle<T>* item) { return item ? item->newNext : nullptr; }

        static inline void SetNext(AssetHandle<T>* item, AssetHandle<T>* next) { item->newNext = next; }

        static inline value_type& GetReference(AssetHandle<T>* item) { return *item->asset; }
        static inline value_type* GetPointer(AssetHandle<T>* item) { return item->asset; }

        static inline const value_type& GetConstReference(const AssetHandle<T>* item) { return *item->asset; }
        static inline const value_type* GetConstPointer(const AssetHandle<T>* item) { return item->asset; }
    };

    template <typename T>
    using NewAssetsList = IntrusiveSingleList<AssetHandle<T>, NewAssetAccessor<T> >;

    template <typename T>
    using NewAssetsResult = IntrusiveSingleListResult<NewAssetsList<T> >;

} }
