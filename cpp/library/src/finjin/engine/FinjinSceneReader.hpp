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
#include "finjin/common/Convert.hpp"
#include "finjin/common/DataChunkReaderCallbacks.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/common/OperationStatus.hpp"
#include "finjin/engine/AssetClassFileReader.hpp"
#include "finjin/engine/AssetReadQueue.hpp"
#include "finjin/engine/AssetReference.hpp"
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/StandardAssetDocumentPropertyNames.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    /*
    The following asset classes are handled by the scene loading code:
    AssetClass::MATERIAL
    AssetClass::MESH
    AssetClass::MORPH_ANIMATION
    AssetClass::NODE_ANIMATION
    AssetClass::PREFAB
    AssetClass::SCENE
    AssetClass::SKELETON
    AssetClass::SKELETON_ANIMATION
    AssetClass::SOUND
    AssetClass::POSE_ANIMATION
    AssetClass::STRING_TABLE
    AssetClass::TEXTURE
    AssetClass::USER_DATA_TYPES
    */

    class FinjinSceneReaderController;

    struct FinjinCommonDataChunkReaderCallbacksSettings
    {
        FinjinCommonDataChunkReaderCallbacksSettings()
        {
            this->setupAllocator = nullptr;
            this->userDataClassPropertiesLookup = nullptr;
        }

        ValueOrError<void> TranslateType(Utf8String& typeName) const
        {
            if (this->translateType != nullptr)
                return this->translateType(typeName);

            return ValueOrError<void>();
        }

        Allocator* setupAllocator; //Used for setup. NOT used for allocating memory while files are being read
        UserDataClassPropertyPointers* userDataClassPropertiesLookup; //Optional. Will be created internally if not specified
        std::function<ValueOrError<void>(Utf8String&)> translateType;
        std::function<ValueOrError<size_t>(const Utf8String&)> parseRenderQueue;
    };

    struct FinjinSceneReaderConstants
    {
        enum { MAX_USER_DATA_OBJECTS = 16 };
        enum { MAX_USER_DATA_ENUMS_OR_PROPERTIIES = 100 };
    };

    template <typename State, size_t mappingCount>
    class FinjinBaseDataChunkReaderCallbacks : public DataChunkReaderCallbacks<State, mappingCount>
    {
    public:
        FinjinBaseDataChunkReaderCallbacks()
        {
            this->lengthHintPropertyName = StandardAssetDocumentPropertyNames::LENGTH_HINT;
            this->userDataClassPropertiesLookupPointer = nullptr;
        }

        void Create(const FinjinCommonDataChunkReaderCallbacksSettings& settings, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            this->tempSimpleUri.Create(settings.setupAllocator);

            this->tempString.Create(settings.setupAllocator);

            for (size_t i = 0; i < 3; i++)
                this->textureAddressMode[i].Create(settings.setupAllocator);

            this->userDataClassPropertiesLookupPointer = settings.userDataClassPropertiesLookup;
            if (this->userDataClassPropertiesLookupPointer == nullptr)
            {
                this->userDataClassPropertiesLookup.Create(FinjinSceneReaderConstants::MAX_USER_DATA_OBJECTS, FinjinSceneReaderConstants::MAX_USER_DATA_ENUMS_OR_PROPERTIIES, settings.setupAllocator, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate user data properties lookup.");
                    return;
                }

                this->userDataClassPropertiesLookupPointer = &this->userDataClassPropertiesLookup;
            }
        }

        void Destroy()
        {
            this->tempSimpleUri.Destroy();

            this->tempString.Destroy();

            for (size_t i = 0; i < 3; i++)
                this->textureAddressMode[i].Destroy();

            this->userDataClassPropertiesLookupPointer = nullptr;
            this->userDataClassPropertiesLookup.Destroy();
        }

        UserDataClassPropertyPointers& GetUserDataClassPropertiesLookup()
        {
            return this->userDataClassPropertiesLookupPointer ? *this->userDataClassPropertiesLookupPointer : this->userDataClassPropertiesLookup;
        }

        SimpleUri tempSimpleUri; //Used to parse URIs for temporary use
        Utf8String tempString; //Used to read strings for temporary use
        Utf8String textureAddressMode[3]; //Used to read strings for temporary use

        UserDataClassPropertyPointers* userDataClassPropertiesLookupPointer;
        UserDataClassPropertyPointers userDataClassPropertiesLookup;
    };


    //Readers-----------------------

    class FinjinSceneReader;
    class FinjinCommonDataChunkReaderCallbacksState;

    struct FinjinCommonDataChunkReaderCallbacksStateSettings
    {
        FinjinCommonDataChunkReaderCallbacksStateSettings()
        {
            this->allocator = nullptr;

            for (auto& count : assetCounts)
                count = 0;

            this->cacheFullFilePaths = true;
            this->createFilesToLoadQueue = false;
            this->createOptimizedLookups = true;

            this->maxFileCount = 0;
        }

        Allocator* allocator; //Used to allocate new objects, buffers, etc, while reading a file

        EnumArray<AssetClass, AssetClass::COUNT, size_t> assetCounts; //At least one must be set to > 0 in order for initialization to succeed

        bool cacheFullFilePaths; //Indicates whether full file paths will be cached when keeping track of which file paths have been encountered. If false, hashes are used
        bool createFilesToLoadQueue; //Indicates whether a queue of file names that should be loaded will be created and maintained
        bool createOptimizedLookups; //A hint that indicates that table lookups are to be created (where possible) instead of using slower linear scans

        size_t maxFileCount;
    };

    class FinjinCommonDataChunkReaderCallbacksState : public DataChunkReaderCallbacksState
    {
    public:
        friend class FinjinSceneReaderController;

        FinjinCommonDataChunkReaderCallbacksState(FinjinCommonDataChunkReaderCallbacksState* parentState = nullptr);
        ~FinjinCommonDataChunkReaderCallbacksState();

        void Create(const FinjinCommonDataChunkReaderCallbacksStateSettings& settings, Error& error);
        void Destroy();

        FinjinCommonDataChunkReaderCallbacksState* GetRootState();
        const FinjinCommonDataChunkReaderCallbacksState* GetRootState() const;

        template <typename T>
        void RecordAssetHandle(AssetHandle<T>* assetHandle, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            RecordAssetHandle((AssetHandle<void>*)assetHandle, TypeToAssetInfo<T>::GetAssetClass(), TypeToAssetInfo<T>::RequiresInternalDependencyResolution(), error);
            if (error)
                FINJIN_SET_ERROR_NO_MESSAGE(error);
        }

        void HandleEncounteredAssetFilePath(const AssetReference& assetRef, AssetClass assetClass, Error& error);

        ValueOrError<bool> AddEncounteredFilePath(const Path& filePath);
        size_t GetEncounteredFileCount() const;

        UserDataClass* GetUserDataClass(const Utf8String& name);

        template <typename T>
        T* GetAssetByName(const Utf8String& name)
        {
            auto& assetBucket = this->assetBucketsByClass[TypeToAssetInfo<T>::GetAssetClass()];
            auto foundAt = assetBucket.assetHandlesByName.find(name);
            if (foundAt != assetBucket.assetHandlesByName.end() && foundAt->second->isOwner)
                return (T*)foundAt->second->asset;
            else
                return nullptr;
        }

        template <typename T>
        QueryAssetsResult<T> GetAssets()
        {
            return GetAssets<T>([] (T* assetHandle) { return true; });
        }

        template <typename T, typename Filter>
        QueryAssetsResult<T> GetAssets(Filter filter)
        {
            auto& assetBucket = this->assetBucketsByClass[TypeToAssetInfo<T>::GetAssetClass()];

            size_t count = 0;
            AssetHandle<void>* resultHead = nullptr;
            AssetHandle<void>* resultTail = nullptr;
            for (auto& item : assetBucket.assetHandlesByName)
            {
                if (item.second->isOwner && filter((T*)item.second))
                {
                    if (resultHead == nullptr)
                    {
                        resultHead = resultTail = item.second;
                        resultHead->queryNext = nullptr;
                    }
                    else
                    {
                        resultTail->queryNext = item.second;
                        resultTail = item.second;
                        resultTail->queryNext = nullptr;
                    }

                    count++;
                }
            }

            return QueryAssetsResult<T>((AssetHandle<T>*)resultHead, count);
        }

        template <typename T>
        NewAssetsResult<T> GetNewAssets(size_t maxCount = (size_t)-1)
        {
            auto& assetBucket = this->assetBucketsByClass[TypeToAssetInfo<T>::GetAssetClass()];

            size_t count = 0;
            auto result = assetBucket.newAssetHandles.SpliceFront(maxCount, &count);
            return NewAssetsResult<T>((AssetHandle<T>*)result.head, count);
        }

        template <typename T, typename ContinuePredicate>
        NewAssetsResult<T> GetNewAssetsIf(ContinuePredicate continuePred)
        {
            auto& assetBucket = this->assetBucketsByClass[TypeToAssetInfo<T>::GetAssetClass()];

            size_t count = 0;
            auto result = assetBucket.newAssetHandles.SpliceFront((size_t)-1, continuePred, &count);
            return NewAssetsResult<T>((AssetHandle<T>*)result.head, count);
        }

    private:
        void RecordAssetHandle(AssetHandle<void>* assetHandle, AssetClass assetClass, bool requiresInternalDependencyResolution, Error& error);

    public:
        FinjinCommonDataChunkReaderCallbacksState* parentState;

        Allocator* allocator; //Used to allocate new objects, buffers, etc, while reading a file

        FinjinSceneReaderController* controller;

        template <typename T>
        class AssetHandleAllocatedNextAccessor
        {
        public:
            using value_type = T;

            static inline T* GetNext(T* item) { return item ? item->newNext : nullptr; }
            static inline const T* GetNext(const T* item) { return item ? item->newNext : nullptr; }

            static inline void SetNext(T* item, T* next) { item->newNext = next; }

            static inline value_type& GetReference(T* item) { return *item; }
            static inline value_type* GetPointer(T* item) { return item; }

            static inline const value_type& GetConstReference(const T* item) { return *item; }
            static inline const value_type* GetConstPointer(const T* item) { return item; }
        };

        struct AssetBucket
        {
            AssetBucket()
            {
                this->internalDependencyHead = nullptr;
                this->allocatedHead = nullptr;
            }

            DynamicUnorderedMap<Utf8String, AssetHandle<void>*> assetHandlesByName; //Maps an object name to an AssetHandle
            AssetHandle<void>* internalDependencyHead;
            AssetHandle<void>* allocatedHead;
            IntrusiveSingleList<AssetHandle<void>, AssetHandleAllocatedNextAccessor<AssetHandle<void>>> newAssetHandles;
        };
        EnumArray<AssetClass, AssetClass::COUNT, AssetBucket> assetBucketsByClass;

        DynamicEncounteredSet<Path> encounteredFilePaths;

        DynamicQueue<Path> filesToLoad;

        bool createOptimizedLookups;
    };


    //Generic reader---------------
    template <typename AssetType>
    class FinjinNonEmbeddedReaderCallbacksState : public FinjinCommonDataChunkReaderCallbacksState
    {
    public:
        using Super = FinjinCommonDataChunkReaderCallbacksState;
        using Settings = FinjinCommonDataChunkReaderCallbacksStateSettings;

        FinjinNonEmbeddedReaderCallbacksState(FinjinCommonDataChunkReaderCallbacksState* parentState = nullptr) : Super(parentState)
        {
            this->userDataStringPropertyCount = 0;

            this->assetHandle = nullptr;
        }

        void Create(const Settings& settings, Error& error) { Super::Create(settings, error); }
        void Destroy() { Super::Destroy(); this->userDataStringPropertyCount = 0;}

        void ResetForChunkStart() { Super::ResetForChunkStart(); this->userDataStringPropertyCount = 0; }

        template <typename T>
        void CreateNewAssetHandle(Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (this->assetHandle != nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unexpectedly encountered a %1%. Existing %1% is not null.", AssetClassUtilities::ToString(TypeToAssetInfo<T>::GetAssetClass())));
                return;
            }

            auto rootState = GetRootState();

            //Allocate memory
            auto mem = rootState->allocator->Allocate(sizeof(AssetHandle<T>), FINJIN_CALLER_ARGUMENTS);
            if (mem == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate %1% asset handle.", AssetClassUtilities::ToString(TypeToAssetInfo<T>::GetAssetClass())));
                return;
            }

            //Construct asset holder
            this->assetHandle = new (mem) AssetHandle<T>(rootState->allocator);

            //Add asset holder to beginning of allocated list
            auto& allocatedHead = rootState->assetBucketsByClass[TypeToAssetInfo<T>::GetAssetClass()].allocatedHead;

            this->assetHandle->allocatedNext = (AssetHandle<T>*)allocatedHead;
            allocatedHead = (AssetHandle<void>*)this->assetHandle;
        }

        template <typename T, typename... Args>
        void CreateNewAssetHandleAndAsset(Path& workingPath, const Path& filePath, Error& error, Args&&... args)
        {
            FINJIN_ERROR_METHOD_START(error);

            CreateNewAssetHandle<T>(error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to create asset handle.");
                return;
            }

            this->assetHandle->isOwner = true;

            this->assetHandle->asset = AllocatedClass::New<T>(this->allocator, FINJIN_CALLER_ARGUMENTS, std::forward<Args>(args)...);
            if (this->assetHandle->asset == nullptr)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate %1% asset.", AssetClassUtilities::ToString(TypeToAssetInfo<T>::GetAssetClass())));
                return;
            }

            if (workingPath.assign(filePath).HasError())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to assign file path '%1%' to working path.", filePath));
                return;
            }
            workingPath.RemoveExtension();

            if (this->assetHandle->asset->name.assign(workingPath.c_str()).HasError())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to set %1% name from file path.", AssetClassUtilities::ToString(TypeToAssetInfo<T>::GetAssetClass())));
                return;
            }

            if (this->assetHandle->assetRef.ForObjectInLocalFile(filePath, this->assetHandle->asset->name).HasError())
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to set %1% handle asset reference.", AssetClassUtilities::ToString(TypeToAssetInfo<T>::GetAssetClass())));
                return;
            }

            RecordAssetHandle(this->assetHandle, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to record %1% asset.", AssetClassUtilities::ToString(TypeToAssetInfo<T>::GetAssetClass())));
                return;
            }
        }

    public:
        size_t userDataStringPropertyCount;

        AssetHandle<AssetType>* assetHandle;
    };


    //Mesh reader-------------
    using FinjinMeshDataChunkReaderCallbacksState = FinjinNonEmbeddedReaderCallbacksState<FinjinMesh>;

    class FinjinMeshDataChunkReaderCallbacks : public FinjinBaseDataChunkReaderCallbacks<FinjinMeshDataChunkReaderCallbacksState, 54>
    {
    public:
        using Super = FinjinBaseDataChunkReaderCallbacks;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;
        using State = FinjinMeshDataChunkReaderCallbacksState;

        FinjinMeshDataChunkReaderCallbacks() {}

        void Create(const Settings& settings, Error& error);

        Settings& GetSettings() { return this->settings; }

    private:
        Settings settings;
    };


    //Skeleton reader-------------
    using FinjinMeshSkeletonDataChunkReaderCallbacksState = FinjinNonEmbeddedReaderCallbacksState<FinjinMeshSkeleton>;

    class FinjinMeshSkeletonDataChunkReaderCallbacks : public FinjinBaseDataChunkReaderCallbacks<FinjinMeshSkeletonDataChunkReaderCallbacksState, 8>
    {
    public:
        using Super = FinjinBaseDataChunkReaderCallbacks;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;
        using State = FinjinMeshSkeletonDataChunkReaderCallbacksState;

        FinjinMeshSkeletonDataChunkReaderCallbacks() {}

        void Create(const Settings& settings, Error& error);

        Settings& GetSettings() { return this->settings; }

    private:
        Settings settings;
    };


    //Material reader-------------
    using FinjinMaterialDataChunkReaderCallbacksState = FinjinNonEmbeddedReaderCallbacksState<FinjinMaterial>;

    class FinjinMaterialDataChunkReaderCallbacks : public FinjinBaseDataChunkReaderCallbacks<FinjinMaterialDataChunkReaderCallbacksState, 3>
    {
    public:
        using Super = FinjinBaseDataChunkReaderCallbacks;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;
        using State = FinjinMaterialDataChunkReaderCallbacksState;

        FinjinMaterialDataChunkReaderCallbacks() {}

        void Create(const Settings& settings, Error& error);

        Settings& GetSettings() { return this->settings; }

    private:
        Settings settings;
    };


    //Prefab reader-------------
    using FinjinPrefabDataChunkReaderCallbacksState = FinjinNonEmbeddedReaderCallbacksState<FinjinPrefab>;

    class FinjinPrefabDataChunkReaderCallbacks : public FinjinBaseDataChunkReaderCallbacks<FinjinPrefabDataChunkReaderCallbacksState, 21>
    {
    public:
        using Super = FinjinBaseDataChunkReaderCallbacks;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;
        using State = FinjinPrefabDataChunkReaderCallbacksState;

        FinjinPrefabDataChunkReaderCallbacks() {}

        void Create(const Settings& settings, Error& error);

        Settings& GetSettings();

    private:
        Settings settings;
    };


    //Node animation reader-------------
    using FinjinNodeAnimationDataChunkReaderCallbacksState = FinjinNonEmbeddedReaderCallbacksState<FinjinNodeAnimation>;

    class FinjinNodeAnimationDataChunkReaderCallbacks : public FinjinBaseDataChunkReaderCallbacks<FinjinNodeAnimationDataChunkReaderCallbacksState, 2>
    {
    public:
        using Super = FinjinBaseDataChunkReaderCallbacks;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;
        using State = FinjinNodeAnimationDataChunkReaderCallbacksState;

        FinjinNodeAnimationDataChunkReaderCallbacks() {}

        void Create(const Settings& settings, Error& error);

        Settings& GetSettings() { return this->settings; }

    private:
        Settings settings;
    };


    //Skeleton animation reader-------------
    using FinjinSkeletonAnimationDataChunkReaderCallbacksState = FinjinNonEmbeddedReaderCallbacksState<FinjinMeshSkeletonAnimation>;

    class FinjinSkeletonAnimationDataChunkReaderCallbacks : public FinjinBaseDataChunkReaderCallbacks<FinjinSkeletonAnimationDataChunkReaderCallbacksState, 4>
    {
    public:
        using Super = FinjinBaseDataChunkReaderCallbacks;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;
        using State = FinjinSkeletonAnimationDataChunkReaderCallbacksState;

        FinjinSkeletonAnimationDataChunkReaderCallbacks() {}

        void Create(const Settings& settings, Error& error);

        Settings& GetSettings() { return this->settings; }

    private:
        Settings settings;
    };


    //Morph animation reader-------------
    using FinjinMorphAnimationDataChunkReaderCallbacksState = FinjinNonEmbeddedReaderCallbacksState<FinjinMeshMorphAnimation>;

    class FinjinMorphAnimationDataChunkReaderCallbacks : public FinjinBaseDataChunkReaderCallbacks<FinjinMorphAnimationDataChunkReaderCallbacksState, 9>
    {
    public:
        using Super = FinjinBaseDataChunkReaderCallbacks;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;
        using State = FinjinMorphAnimationDataChunkReaderCallbacksState;

        FinjinMorphAnimationDataChunkReaderCallbacks() {}

        void Create(const Settings& settings, Error& error);

        Settings& GetSettings() { return this->settings; }

    private:
        Settings settings;
    };


    //Pose animation reader-------------
    using FinjinPoseAnimationDataChunkReaderCallbacksState = FinjinNonEmbeddedReaderCallbacksState<FinjinMeshPoseAnimation>;

    class FinjinPoseAnimationDataChunkReaderCallbacks : public FinjinBaseDataChunkReaderCallbacks<FinjinPoseAnimationDataChunkReaderCallbacksState, 5>
    {
    public:
        using Super = FinjinBaseDataChunkReaderCallbacks;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;
        using State = FinjinPoseAnimationDataChunkReaderCallbacksState;

        FinjinPoseAnimationDataChunkReaderCallbacks() {}

        void Create(const Settings& settings, Error& error);

        Settings& GetSettings() { return this->settings; }

    private:
        Settings settings;
    };


    //Scene reader-------------
    using FinjinSceneDataChunkReaderCallbacksState = FinjinNonEmbeddedReaderCallbacksState<FinjinScene>;

    class FinjinSceneDataChunkReaderCallbacks : public FinjinBaseDataChunkReaderCallbacks<FinjinSceneDataChunkReaderCallbacksState, 114>
    {
    public:
        using Super = FinjinBaseDataChunkReaderCallbacks;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;
        using State = FinjinSceneDataChunkReaderCallbacksState;

        FinjinSceneDataChunkReaderCallbacks() {}

        void Create(const Settings& settings, Error& error);

        Settings& GetSettings();

    private:
        Settings settings;
    };


    //Handwritten user data types
    using HandwrittenUserDataTypesReaderState = FinjinNonEmbeddedReaderCallbacksState<UserDataTypes>;

    class HandwrittenUserDataTypesReader
    {
    public:
        HandwrittenUserDataTypesReader();
        ~HandwrittenUserDataTypesReader();

        using State = HandwrittenUserDataTypesReaderState;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;

        void Create(const Settings& settings, Error& error);
        void Destroy();

        Settings& GetSettings();

        void Parse(const AssetReference& assetRef, const Path& filePath, State& state, const ByteBufferReader& bytes, Error& error);

    private:
        void ParseInclude(State& state, ConfigDocumentReader& reader, const Utf8String& initialSectionName, Error& error);
        void ParseEnum(State& state, UserDataTypes* userDataTypes, ConfigDocumentReader& reader, const Utf8String& initialSectionName, Error& error);
        void ParseClass(State& state, UserDataTypes* userDataTypes, ConfigDocumentReader& reader, const Utf8String& initialSectionName, Error& error);

    private:
        Settings settings;

        AssetReference workingAssetRef;
        Path workingPath;
    };


    //Handwritten strings
    using HandwrittenStringsReaderState = FinjinNonEmbeddedReaderCallbacksState<StringTable>;

    class HandwrittenStringsReader
    {
    public:
        HandwrittenStringsReader();
        ~HandwrittenStringsReader();

        using State = HandwrittenStringsReaderState;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;

        void Create(const Settings& settings, Error& error);
        void Destroy();

        Settings& GetSettings();

        void Parse(const AssetReference& assetRef, const Path& filePath, State& state, const ByteBufferReader& bytes, Error& error);

    private:
        Settings settings;

        AssetReference workingAssetRef;
        Path workingPath;
    };


    //Texture
    using FinjinTextureReaderState = FinjinNonEmbeddedReaderCallbacksState<FinjinTexture>;

    class FinjinTextureReader
    {
    public:
        FinjinTextureReader();
        ~FinjinTextureReader();

        using State = FinjinTextureReaderState;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;

        void Create(const Settings& settings, Error& error);
        void Destroy();

        Settings& GetSettings();

        void Parse(const AssetReference& assetRef, const Path& filePath, State& state, const ByteBufferReader& bytes, Error& error);

    private:
        Settings settings;

        AssetReference workingAssetRef;
        Path workingPath;
    };


    //Sound
    using FinjinSoundReaderState = FinjinNonEmbeddedReaderCallbacksState<FinjinSound>;

    class FinjinSoundReader
    {
    public:
        FinjinSoundReader();
        ~FinjinSoundReader();

        using State = FinjinSoundReaderState;
        using Settings = FinjinCommonDataChunkReaderCallbacksSettings;

        void Create(const Settings& settings, Error& error);
        void Destroy();

        Settings& GetSettings();

        void Parse(const AssetReference& assetRef, const Path& filePath, State& state, const ByteBufferReader& bytes, Error& error);

    private:
        Settings settings;

        AssetReference workingAssetRef;
        Path workingPath;
    };

    //Controller---------------
    enum class AssetCollisionResolution
    {
        GENERATE_ERROR,
        KEEP_EXISTING,
        KEEP_NEW
    };

    class FinjinSceneReaderController
    {
    public:
        virtual ~FinjinSceneReaderController() {}

        virtual AssetCollisionResolution ResolveAssetCollision(AssetHandle<void>* existingAssetHandle, AssetClass assetClass, AssetHandle<void>* newAssetHandle)
        {
            return AssetCollisionResolution::GENERATE_ERROR;
        }

        virtual bool HandleNewAsset(const AssetReference& assetRef, AssetClass assetClass, Error& error)
        {
            return false;
        }
    };


    //All readers--------------------------------------
    class FinjinSceneReaderState : public FinjinCommonDataChunkReaderCallbacksState
    {
    public:
        FinjinSceneReaderState(const FinjinSceneReaderState& other) = delete;
        FinjinSceneReaderState(FinjinSceneReaderState&& other) = delete;

        FinjinSceneReaderState& operator = (const FinjinSceneReaderState& other) = delete;
        FinjinSceneReaderState& operator = (FinjinSceneReaderState&& other) = delete;

        using Super = FinjinCommonDataChunkReaderCallbacksState;
        using Settings = FinjinCommonDataChunkReaderCallbacksStateSettings;

        FinjinSceneReaderState(FinjinCommonDataChunkReaderCallbacksState* parentState = nullptr);

        void Create(const Settings& settings, Error& error);
        void Destroy();

        FinjinSceneDataChunkReaderCallbacksState sceneCallbacksState;
        FinjinMeshDataChunkReaderCallbacksState meshCallbacksState;
        FinjinMeshSkeletonDataChunkReaderCallbacksState skeletonCallbacksState;
        FinjinMaterialDataChunkReaderCallbacksState materialCallbacksState;
        FinjinPrefabDataChunkReaderCallbacksState prefabCallbacksState;
        FinjinNodeAnimationDataChunkReaderCallbacksState nodeAnimationCallbacksState;
        FinjinSkeletonAnimationDataChunkReaderCallbacksState skeletonAnimationCallbacksState;
        FinjinMorphAnimationDataChunkReaderCallbacksState morphAnimationCallbacksState;
        FinjinPoseAnimationDataChunkReaderCallbacksState poseAnimationCallbacksState;
        HandwrittenUserDataTypesReaderState handwrittenUserDataTypesReaderState;
        HandwrittenStringsReaderState handwrittenStringsReaderState;
        FinjinTextureReaderState textureReaderState;
        FinjinSoundReaderState soundReaderState;
    };

    class FinjinSceneReader : public FinjinSceneReaderController
    {
    public:
        FinjinSceneReader(const FinjinSceneReader& other) = delete;
        FinjinSceneReader(FinjinSceneReader&& other) = delete;

        FinjinSceneReader& operator = (const FinjinSceneReader& other) = delete;
        FinjinSceneReader& operator = (FinjinSceneReader&& other) = delete;

        using State = FinjinSceneReaderState;

        struct Settings : FinjinCommonDataChunkReaderCallbacksSettings
        {
            Settings()
            {
                this->assetReadQueue = nullptr;
                this->assetClassFileReaders = nullptr;

                this->state = nullptr;

                this->maxQueuedFiles = 0;
            }

            AssetReadQueue* assetReadQueue;
            EnumArray<AssetClass, AssetClass::COUNT, AssetClassFileReader>* assetClassFileReaders;

            State* state;

            size_t maxQueuedFiles;

            StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS> externalTextureExtensions; //Texture file extensions without a leading dot
            StaticVector<Utf8String, EngineConstants::MAX_EXTERNAL_ASSET_FILE_EXTENSIONS> externalSoundExtensions; //Sound file extensions without a leading dot
        };

        FinjinSceneReader(Allocator* allocator);

        void Create(const Settings& settings, Error& error);
        void Destroy();

        Settings& GetSettings();

        State* GetState();
        void SetState(State* state, Error& error);

        enum class SetStateResult
        {
            SUCCESS,
            READ_IN_PROGRESS,
            STATE_STILL_ATTACHED_TO_CONTROLLER
        };
        SetStateResult SetState(FinjinSceneReader::State* state);

        void RequestRead(const AssetReference* assetRefs, size_t count, Error& error);

        template <typename... Args>
        void RequestReadLocalFiles(Error& error, const Args&... args)
        {
            FINJIN_ERROR_METHOD_START(error);

            for (auto& item : { args... })
            {
                this->workingAssetRef.ForLocalFile(item);
                RequestRead(&this->workingAssetRef, 1, error);
                if (error)
                {
                    FINJIN_SET_ERROR_NO_MESSAGE(error);
                    return;
                }
            }
        }

        OperationStatus& GetReadStatus();

        AssetCollisionResolution ResolveAssetCollision(AssetHandle<void>* existingAssetHandle, AssetClass assetClass, AssetHandle<void>* newAssetHandle) override;
        bool HandleNewAsset(const AssetReference& assetRef, AssetClass assetClass, Error& error) override;

    private:
        void FinishedFile(Error& error);
        bool IsResidentOnGpu() const;

        bool RequestRead(const AssetReference& assetRef, bool addToEncountered, Error& error);

        template <typename Callbacks, typename State>
        void CreateStreamingAssetRequest(const AssetReference& assetRef, AssetClass assetClass, StreamingFileFormat fileFormat, Callbacks& callbacks, State& state)
        {
            this->workingAssetRequest.StreamingReadRequest([this, assetClass, &callbacks, &state](AssetReadHandle& operationHandle, DataChunkReader& reader, DataHeader& dataHeader, Error& error)
            {
                FINJIN_ERROR_METHOD_START(error);

                callbacks.HandleItem(reader, dataHeader, state, error);
                if (error)
                {
                    this->readStatus.SetStatus(OperationStatus::FAILURE);

                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to handle item for '%1%'.", AssetClassUtilities::ToString(assetClass)));
                    return;
                }

                if (dataHeader.type == DataHeaderType::END)
                {
                    FinishedFile(error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unexpected failure while finishing the processing of '%1%'.", AssetClassUtilities::ToString(assetClass)));
                        return;
                    }
                }
            }, fileFormat).SetAssetReference(assetRef, (*this->settings.assetClassFileReaders)[assetClass]);
        }

        template <typename Reader, typename State>
        void CreateFullParseAssetRequest(const AssetReference& assetRef, AssetClass assetClass, Reader& reader, State& state)
        {
            this->workingAssetRequest.FullReadRequest([this, assetClass, &reader, &state](AssetReadHandle& assetReadHandle, const uint8_t* bytes, size_t byteCount, Error& error)
            {
                FINJIN_ERROR_METHOD_START(error);

                ByteBufferReader byteBuffer(bytes, byteCount);
                reader.Parse(assetReadHandle.GetAssetReference(), assetReadHandle.GetAssetReference().filePath, state, byteBuffer, error);
                if (error)
                {
                    this->readStatus.SetStatus(OperationStatus::FAILURE);

                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse '%1%' (%2%).", assetReadHandle.GetAssetReference().filePath, AssetClassUtilities::ToString(assetClass)));
                    return;
                }

                FinishedFile(error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unexpected failure while finishing the processing of '%1%' (%2%).", assetReadHandle.GetAssetReference().filePath, AssetClassUtilities::ToString(assetClass)));
                    return;
                }
            }).SetAssetReference(assetRef, (*this->settings.assetClassFileReaders)[assetClass]);
        }

    public:
        Settings settings;

        UserDataClassPropertyPointers userDataClassPropertiesLookup;

        size_t filesLoaded;
        OperationStatus readStatus;

        SimpleUri tempSimpleUri;
        AssetReference workingAssetRef;
        AssetReadRequest workingAssetRequest;

        DynamicQueue<AssetReadHandle> assetReadHandleQueue;

        bool createdCallbacks;

        FinjinSceneDataChunkReaderCallbacks sceneCallbacks;
        FinjinMeshDataChunkReaderCallbacks meshCallbacks;
        FinjinMeshSkeletonDataChunkReaderCallbacks skeletonCallbacks;
        FinjinMaterialDataChunkReaderCallbacks materialCallbacks;
        FinjinPrefabDataChunkReaderCallbacks prefabCallbacks;
        FinjinNodeAnimationDataChunkReaderCallbacks nodeAnimationCallbacks;
        FinjinSkeletonAnimationDataChunkReaderCallbacks skeletonAnimationCallbacks;
        FinjinMorphAnimationDataChunkReaderCallbacks morphAnimationCallbacks;
        FinjinPoseAnimationDataChunkReaderCallbacks poseAnimationCallbacks;
        HandwrittenUserDataTypesReader handwrittenUserDataTypesReader;
        HandwrittenStringsReader handwrittenStringsReader;
        FinjinTextureReader textureReader;
        FinjinSoundReader soundReader;
    };

} }
