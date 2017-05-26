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
#include "finjin/common/DynamicVector.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/common/TypeDescription.hpp"
#include "finjin/engine/AssetClass.hpp"
#include "finjin/engine/AssetHandle.hpp"
#include "finjin/engine/CameraState.hpp"
#include "finjin/engine/GenericGpuNumericStructs.hpp"
#include "finjin/engine/LightState.hpp"
#include "finjin/engine/ShaderFeatureFlag.hpp"
#include "finjin/engine/StringTable.hpp"
#include "finjin/engine/UserDataTypes.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class FinjinSceneNode;

    enum class FinjinPrimitiveType
    {
        TRIANGLE_LIST,
        LINE_LIST,
        POINT_LIST
    };

    enum class FinjinTextureAddressMode
    {
        CLAMP,
        MIRROR,
        WRAP
    };

    enum class FinjinCullMode
    {
        NORMAL,
        NONE //No culling
    };

    enum class FinjinPolygonMode
    {
        SOLID,
        WIREFRAME
    };

    enum class FinjinShadingMode
    {
        SMOOTH,
        FLAT
    };

    enum class FinjinTransparency
    {
        NONE,
        ADDITIVE,
        FILTER,
        SUBTRACTIVE
    };

    enum class FinjinTextureAlpha
    {
        STANDARD,
        PREMULTIPLIED
    };

    enum class FinjinSceneNodeVisibility
    {
        DEFAULT,
        VISIBLE,
        HIDDEN,
        TREE_VISIBLE,
        TREE_HIDDEN
    };

    enum class FinjinIndexBufferType
    {
        UINT16,
        UINT32
    };

    struct AmbientLightState
    {
        AmbientLightState() : color(0, 0, 0, 1)
        {
        }

        MathVector4 color;
    };

    class AmbientLight : public AmbientLightState
    {
    public:
        AmbientLightState& Evaluate(size_t frameStageIndex, size_t updateSequence)
        {
            auto& frameStage = this->frameStages[frameStageIndex];
            frameStage = *this;
            return frameStage;
        }

    public:
        AmbientLightState frameStages[EngineConstants::MAX_FRAME_STAGES];
    };

    struct SceneNodeState
    {
        FinjinSceneNodeVisibility visibility;
        MathMatrix4 worldMatrix;
        MathMatrix4 inverseWorldMatrix;
        MathMatrix4 inverseTransposeWorldMatrix;
    };

    struct FinjinVertexElementFormat
    {
        FinjinVertexElementFormat()
        {
            this->id = GpuInputFormatStructMetadata::ElementID::NONE;
            this->type = NumericStructElementType::NONE;
        }

        GpuInputFormatStructMetadata::ElementID id;
        NumericStructElementType type;
    };

    struct FinjinVertexElementChannel
    {
        bool empty() const
        {
            return this->floatValues.empty() && this->int32Values.empty();
        }

        void* data()
        {
            if (!this->floatValues.empty())
                return this->floatValues.data();
            else if (!this->int32Values.empty())
                return this->int32Values.data();

            return nullptr;
        }

        const void* data() const
        {
            if (!this->floatValues.empty())
                return this->floatValues.data();
            else if (!this->int32Values.empty())
                return this->int32Values.data();

            return nullptr;
        }

        uint8_t* GetBytes()
        {
            return static_cast<uint8_t*>(data());
        }

        const uint8_t* GetBytes() const
        {
            return static_cast<const uint8_t*>(data());
        }

        DynamicVector<float> floatValues;
        DynamicVector<int32_t> int32Values;
    };

    FINJIN_ASSET_CLASS class FinjinTexture : public AllocatedClass
    {
    public:
        FinjinTexture(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
            this->gpuTexture = nullptr;
        }

        Utf8String name;
        ByteBuffer fileBytes;
        VoidHardwareAsset* gpuTexture;
    };

    FINJIN_ASSET_CLASS class FinjinSound : public AllocatedClass
    {
    public:
        FinjinSound(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
        }

        Utf8String name;
        ByteBuffer fileBytes;
    };

    FINJIN_ASSET_CLASS class FinjinShader : public AllocatedClass
    {
    public:
        FinjinShader(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
        }

        Utf8String name;
        ByteBuffer fileBytes;
    };

    class FinjinSceneObjectBaseState
    {
    public:
        FinjinSceneObjectBaseState(Allocator* allocator) : name(allocator), userData(allocator)
        {
            this->queryNext = nullptr;
        }

        virtual ~FinjinSceneObjectBaseState() {}

        Utf8String name;

        Uuid id;

        UserDataClassInstance userData;

        struct NoteTrack
        {
            NoteTrack(Allocator* allocator) : name(allocator)
            {
            }

            Utf8String name;

            struct Key
            {
                Key(Allocator* allocator) : content(allocator)
                {
                }

                TimeDuration time;
                Utf8String content;
            };
            DynamicVector<Key> keys;
        };
        DynamicVector<NoteTrack> noteTracks;

        DynamicVector<uint8_t> flags;

        FinjinSceneObjectBaseState* queryNext;
    };

    class FinjinSceneObjectBase : public AllocatedClass, public FinjinSceneObjectBaseState
    {
    public:
        FINJIN_DECLARE_ABSTRACT_BASE_TYPE_DESCRIPTION(FinjinSceneObjectBase)

        FinjinSceneObjectBase(Allocator* allocator);
        virtual ~FinjinSceneObjectBase();
    };

    //Results from a generic "get"
    template <typename T>
    class QuerySceneObjectAccessor
    {
    public:
        using value_type = T;

        static inline T* GetNext(T* item) { return item ? (T*)item->queryNext : nullptr; }
        static inline const T* GetNext(const T* item) { return item ? item->queryNext : nullptr; }

        static inline void SetNext(T* item, T* next) { item->queryNext = next; }

        static inline value_type& GetReference(T* item) { return (T&)*item; }
        static inline value_type* GetPointer(T* item) { return (T*)item; }

        static inline const value_type& GetConstReference(const T* item) { return (const T&)*item; }
        static inline const value_type* GetConstPointer(const T* item) { return (const T*)item; }
    };

    template <typename T>
    using QuerySceneObjectsList = IntrusiveSingleList<T, QuerySceneObjectAccessor<T> >;

    template <typename T>
    using QuerySceneObjectsResult = IntrusiveSingleListResult<QuerySceneObjectsList<T> >;

    FINJIN_ASSET_CLASS class FinjinMaterial : public FinjinSceneObjectBase
    {
    public:
        FINJIN_DECLARE_TYPE_DESCRIPTION(FinjinMaterial, FinjinSceneObjectBase)

        FinjinMaterial(Allocator* allocator) :
            Super(allocator),
            typeName(allocator),
            ambientColor(1, 1, 1, 1),
            diffuseColor(1, 1, 1, 1),
            specularColor(1, 1, 1, 1),
            selfIlluminationColor(0, 0, 0, 1)
        {
            this->shininess = 0;
            this->opacity = 1;
            this->cullMode = FinjinCullMode::NORMAL;
            this->polygonMode = FinjinPolygonMode::SOLID;
            this->shadingMode = FinjinShadingMode::SMOOTH;
            this->transparency = FinjinTransparency::NONE;

            this->gpuMaterial = nullptr;
            this->gpuMapFlags = ShaderFeatureFlag::NONE;
        }

        Utf8String typeName;
        MathVector4 ambientColor;
        MathVector4 diffuseColor;
        MathVector4 specularColor;
        MathVector4 selfIlluminationColor;
        float shininess;
        float opacity;
        FinjinCullMode cullMode;
        FinjinPolygonMode polygonMode;
        FinjinShadingMode shadingMode;
        FinjinTransparency transparency;

        struct Map
        {
            Map(Allocator* allocator) :
                typeName(allocator),
                name(allocator),
                textureHandle(allocator),
                textureScale{ 1,1,1 },
                textureOffset{ 0,0,0 },
                textureAddressMode{ FinjinTextureAddressMode::CLAMP, FinjinTextureAddressMode::CLAMP, FinjinTextureAddressMode::CLAMP }
            {
                this->textureCoordinateSet = 0;
                this->textureAlpha = FinjinTextureAlpha::STANDARD;
                this->amount = 1.0f;

                this->gpuMaterialMapIndex = (size_t)-1;
                this->gpuBufferTextureIndexElementID = (size_t)-1;
                this->gpuBufferAmountElementID = (size_t)-1;
                this->gpuMaterialMapFlag = ShaderFeatureFlag::NONE;
            }

            Utf8String typeName;
            Utf8String name;
            AssetHandle<FinjinTexture> textureHandle;
            uint8_t textureCoordinateSet;
            FinjinTextureAlpha textureAlpha;
            MathVector3 textureScale;
            Radians textureRotation;
            MathVector3 textureOffset;
            float amount;
            FinjinTextureAddressMode textureAddressMode[3];

            size_t gpuMaterialMapIndex; //Index into D3D12Material maps that corresponds to this map
            size_t gpuBufferTextureIndexElementID; //ElementID (index) into shader constant or structured buffer for 'texture index'
            size_t gpuBufferAmountElementID; //ElementID (index) into shader constant or structured buffer for 'amount'
            ShaderFeatureFlag gpuMaterialMapFlag; //Single flag identifying this map's shader usage
        };
        DynamicVector<Map> maps;

        VoidHardwareAsset* gpuMaterial;
        ShaderFeatureFlag gpuMapFlags;
    };

    class FinjinMeshSkeletonBone
    {
    public:
        FinjinMeshSkeletonBone(Allocator* allocator) : name(allocator)
        {
            this->parentBonePointer = nullptr;
        }

        Utf8String name;
        FinjinMeshSkeletonBone* parentBonePointer;
        DynamicVector<FinjinMeshSkeletonBone*> childBonePointers;
    };

    FINJIN_ASSET_CLASS class FinjinMeshSkeletonAnimation : public AllocatedClass
    {
    public:
        FinjinMeshSkeletonAnimation(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
        }

        Utf8String name;
        TimeDuration length;

        struct Bone
        {
            size_t index;

            struct Key
            {
                TimeDuration time;
                MathMatrix4 m44;
            };
            DynamicVector<Key> keys;
        };
        DynamicVector<Bone> bones;
    };

    FINJIN_ASSET_CLASS class FinjinMeshSkeleton : public AllocatedClass
    {
    public:
        FinjinMeshSkeleton(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
        }

        Utf8String name;

        using Bone = FinjinMeshSkeletonBone;
        DynamicVector<Bone> bones;

        using Animation = FinjinMeshSkeletonAnimation;
        DynamicVector<AssetHandle<Animation> > animationHandles;
    };

    FINJIN_ASSET_CLASS class FinjinMeshMorphAnimation : public AllocatedClass
    {
    public:
        FinjinMeshMorphAnimation(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
        }

        Utf8String name;
        TimeDuration length;

        struct Subanimation
        {
            size_t submeshIndex;
            DynamicVector<FinjinVertexElementFormat> formatElements;
            struct Key
            {
                TimeDuration time;
                size_t vertexCount;
                DynamicVector<FinjinVertexElementChannel> channels;
            };
            DynamicVector<Key> keys;
        };
        DynamicVector<Subanimation> subanimations;
    };

    FINJIN_ASSET_CLASS class FinjinMeshPoseAnimation : public AllocatedClass
    {
    public:
        FinjinMeshPoseAnimation(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
        }

        Utf8String name;
        TimeDuration length;

        struct Subanimation
        {
            size_t submeshIndex;

            struct Key
            {
                TimeDuration time;
                DynamicVector<float> influences;
                DynamicVector<size_t> submeshIndexes;
            };
            DynamicVector<Key> keys;
        };
        DynamicVector<Subanimation> subanimations;
    };

    FINJIN_ASSET_CLASS class FinjinMesh : public FinjinSceneObjectBase
    {
    public:
        FINJIN_DECLARE_TYPE_DESCRIPTION(FinjinMesh, FinjinSceneObjectBase)

        FinjinMesh(Allocator* allocator) :
            Super(allocator),
            typeName(allocator),
            skeletonHandle(allocator)
        {
            this->gpuMesh = nullptr;
            this->gpuMeshFlags = ShaderFeatureFlag::NONE;
        }

        Utf8String typeName;

        struct BoundingVolume
        {
            BoundingVolume()
            {
                FINJIN_ZERO_ITEM(*this);
            }

            MathVector3 cubeCenter;
            float cubeSize;
            MathVector3 boxCenter;
            MathVector3 boxSize;
            float radius;
        };
        BoundingVolume boundingVolume;

        struct IndexBuffer
        {
            IndexBuffer()
            {
                this->type = FinjinIndexBufferType::UINT16;
            }

            size_t size() const
            {
                return !this->uint16s.empty() ? this->uint16s.size() : this->uint32s.size();
            }

            bool empty() const
            {
                return this->uint16s.empty() && this->uint32s.empty();
            }

            void* data()
            {
                return !this->uint16s.empty() ? static_cast<void*>(this->uint16s.data()) : static_cast<void*>(this->uint32s.data());
            }

            const void* data() const
            {
                return !this->uint16s.empty() ? static_cast<const void*>(this->uint16s.data()) : static_cast<const void*>(this->uint32s.data());
            }

            uint8_t* GetBytes()
            {
                return static_cast<uint8_t*>(data());
            }

            const uint8_t* GetBytes() const
            {
                return static_cast<const uint8_t*>(data());
            }

            size_t GetByteCount() const
            {
                return size() * GetElementSize();
            }

            size_t GetElementSize() const
            {
                return this->type == FinjinIndexBufferType::UINT16 ? 2 : 4;
            }

            FinjinIndexBufferType type;
            DynamicVector<uint16_t> uint16s;
            DynamicVector<uint32_t> uint32s;
        };

        struct VertexBuffer
        {
            VertexBuffer(Allocator* allocator) : formatName(allocator)
            {
                this->vertexCount = 0;
            }

            bool empty() const
            {
                return this->vertexCount == 0;
            }

            size_t GetVertexSize() const
            {
                size_t count = 0;

                for (auto& element : this->formatElements)
                    count += NumericStructElementTypeUtilities::GetSimpleTypeSizeInBytes(element.type);

                return count;
            }

            template <typename Bytes>
            bool Interleave(Bytes& interleaved) const
            {
                //Resize buffer
                auto interleavedByteCount = GetVertexSize() * this->vertexCount;
                if (interleaved.resize(interleavedByteCount) < interleavedByteCount)
                    return false;

                //Interleave
                return Interleave(interleaved.data(), interleavedByteCount);
            }

            bool Interleave(void* interleavedBytes, size_t maxInterleavedByteCount) const
            {
                //Validate size
                auto vertexSize = GetVertexSize();
                auto interleavedByteCount = vertexSize * this->vertexCount;
                if (interleavedByteCount > maxInterleavedByteCount)
                    return false;

                //Convert the channel-based data into a single interleaved stream
                //Iterate on each channel....
                size_t interleavedChannelElementOffset = 0;
                for (size_t channelIndex = 0; channelIndex < this->channels.size(); channelIndex++)
                {
                    auto& channel = this->channels[channelIndex];
                    auto channelBytes = channel.GetBytes();

                    auto& formatElement = this->formatElements[channelIndex]; //Each format corresponds to a channel
                    auto elementSize = NumericStructElementTypeUtilities::GetSimpleTypeSizeInBytes(formatElement.type);

                    //Copy each value from the channel to the interleaved vertex stream
                    auto interleavedVertex = static_cast<uint8_t*>(interleavedBytes) + interleavedChannelElementOffset;
                    for (size_t vertexIndex = 0; vertexIndex < this->vertexCount; vertexIndex++)
                    {
                        FINJIN_COPY_MEMORY(interleavedVertex, channelBytes, elementSize); //Copy to interleaved vertex
                        interleavedVertex += vertexSize; //Advanced to next interleaved vertex
                        channelBytes += elementSize; //Advance to next value in channel
                    }

                    //Advance to next element in interleaved vertex
                    interleavedChannelElementOffset += elementSize;
                }

                return true;
            }

            Utf8String formatName;
            DynamicVector<FinjinVertexElementFormat> formatElements;
            size_t vertexCount;
            DynamicVector<FinjinVertexElementChannel> channels;
        };

        IndexBuffer indexBuffer;
        DynamicVector<VertexBuffer> vertexBuffers;

        struct BufferRange
        {
            BufferRange()
            {
                this->bufferIndex = 0;
                this->startIndex = 0;
                this->count = 0;
            }

            bool empty() const
            {
                return this->count == 0;
            }

            size_t bufferIndex;
            size_t startIndex;
            size_t count;
        };

        using IndexBufferRange = BufferRange;
        using VertexBufferRange = BufferRange;

        struct Submesh
        {
            Submesh(Allocator* allocator) :
                name(allocator),
                materialHandle(allocator),
                vertexBuffer(allocator)
            {
                this->primitiveType = FinjinPrimitiveType::TRIANGLE_LIST;
            }

            Utf8String name;

            AssetHandle<FinjinMaterial> materialHandle;

            FinjinPrimitiveType primitiveType;

            IndexBuffer indexBuffer;
            IndexBufferRange indexBufferRange;

            VertexBuffer vertexBuffer;
            VertexBufferRange vertexBufferRange;

            struct VertexBoneAssignment
            {
                uint32_t vertexIndex;
                uint32_t boneIndex;
                float weight;

                VertexBoneAssignment()
                {
                    this->vertexIndex = 0;
                    this->boneIndex = 0;
                    this->weight = 0;
                }
            };
            DynamicVector<VertexBoneAssignment> vertexBoneAssignments;
        };
        DynamicVector<Submesh> submeshes;

        AssetHandle<FinjinMeshSkeleton> skeletonHandle;

        using MorphAnimation = FinjinMeshMorphAnimation;
        DynamicVector<AssetHandle<MorphAnimation> > morphAnimationHandles;

        struct Pose
        {
            struct Subpose
            {
                size_t submeshIndex;
                DynamicVector<FinjinVertexElementFormat> formatElements;
                DynamicVector<size_t> indexValues;
                DynamicVector<FinjinVertexElementChannel> channels;
            };
            DynamicVector<Subpose> subposes;
        };
        DynamicVector<Pose> poses;

        using PoseAnimation = FinjinMeshPoseAnimation;
        DynamicVector<AssetHandle<PoseAnimation> > poseAnimationHandles;

        struct ManualLod
        {
            ManualLod(Allocator* allocator) : meshHandle(allocator), distance(0)
            {
            }

            AssetHandle<FinjinMesh> meshHandle;
            float distance;
        };
        DynamicVector<ManualLod> manualLods;

        VoidHardwareAsset* gpuMesh;
        ShaderFeatureFlag gpuMeshFlags;
    };

    FINJIN_ASSET_CLASS class FinjinPrefab : public FinjinSceneObjectBase
    {
    public:
        FinjinPrefab(Allocator* allocator) : FinjinSceneObjectBase(allocator)
        {
        }

        DynamicVector<FinjinSceneNode*> sceneNodes;
    };


    class FinjinSceneMovableObject : public FinjinSceneObjectBase
    {
    public:
        FINJIN_DECLARE_TYPE_DESCRIPTION(FinjinSceneMovableObject, FinjinSceneObjectBase)

        FinjinSceneMovableObject(Allocator* allocator);
        ~FinjinSceneMovableObject();

    public:
        FinjinSceneNode* parentNodePointer;
        MathMatrix4 transform;
    };

    class FinjinSceneRenderableMovableObject : public FinjinSceneMovableObject
    {
    public:
        FINJIN_DECLARE_TYPE_DESCRIPTION(FinjinSceneRenderableMovableObject, FinjinSceneMovableObject)

        FinjinSceneRenderableMovableObject(Allocator* allocator);
        ~FinjinSceneRenderableMovableObject();

    public:
        size_t renderQueue;
        int32_t renderPriority;
        float renderDistance;
        bool castShadows;
        bool receiveShadows;
    };

    class FinjinSceneObjectEntity : public FinjinSceneRenderableMovableObject
    {
    public:
        FINJIN_DECLARE_TYPE_DESCRIPTION(FinjinSceneObjectEntity, FinjinSceneRenderableMovableObject)

        FinjinSceneObjectEntity(Allocator* allocator);

    public:
        AssetHandle<FinjinMesh> meshHandle;

        struct Subentity
        {
            Subentity(Allocator* allocator) : materialHandle(allocator)
            {
            }

            AssetHandle<FinjinMaterial> materialHandle;
        };
        DynamicVector<Subentity> subentities;
    };

    class FinjinSceneObjectCamera : public FinjinSceneRenderableMovableObject, public CameraState
    {
    public:
        FINJIN_DECLARE_TYPE_DESCRIPTION(FinjinSceneObjectCamera, FinjinSceneRenderableMovableObject)

        FinjinSceneObjectCamera(Allocator* allocator);

        CameraState& Evaluate(size_t frameStageIndex, size_t updateSequence, const SceneNodeState& sceneNodeState);

    public:
        CameraState frameStages[EngineConstants::MAX_FRAME_STAGES];
    };

    class FinjinSceneObjectLight : public FinjinSceneRenderableMovableObject, public LightState
    {
    public:
        FINJIN_DECLARE_TYPE_DESCRIPTION(FinjinSceneObjectLight, FinjinSceneRenderableMovableObject)

        FinjinSceneObjectLight(Allocator* allocator);

        LightState& Evaluate(size_t frameStageIndex, size_t updateSequence, const SceneNodeState& sceneNodeState);

    public:
        LightState frameStages[EngineConstants::MAX_FRAME_STAGES];
        VoidHardwareAsset* gpuLight;
    };

    FINJIN_ASSET_CLASS class FinjinNodeAnimation : public AllocatedClass
    {
    public:
        FinjinNodeAnimation(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
            this->enable = this->loop = false;
        }

    public:
        bool enable;
        bool loop;
        Utf8String name;
        TimeDuration length;

        struct Key
        {
            Key()
            {
                this->time = TimeDuration::GetZero();
                FINJIN_ZERO_ITEM(this->m44);
            }

            TimeDuration time;
            MathMatrix4 m44;
        };
        DynamicVector<Key> keys;
    };

    class FinjinSceneNode : public FinjinSceneMovableObject
    {
    public:
        FINJIN_DECLARE_TYPE_DESCRIPTION(FinjinSceneNode, FinjinSceneMovableObject)

        FinjinSceneNode(Allocator* allocator);
        ~FinjinSceneNode();

        template <typename T = FinjinSceneObjectBaseState>
        QuerySceneObjectsResult<T> Get(const TypeDescription& typeDescription = FINJIN_TYPE_DESCRIPTION(T))
        {
            QuerySceneObjectsResult<T> result;

            for (auto object : this->objects)
            {
                if (object->IsTypeOf(typeDescription))
                {
                    object->queryNext = nullptr;
                    result.push_back((T*)object);
                }
            }

            return result;
        }

        SceneNodeState& Evaluate(size_t frameStageIndex, size_t updateSequence);

    private:
        FinjinSceneNodeVisibility EvaluateVisibility(FinjinSceneNodeVisibility defaultVisibility = FinjinSceneNodeVisibility::VISIBLE) const;

        void EvaluateTransform(MathMatrix4& result) const;

    public:
        Uuid id;
        FinjinSceneNodeVisibility visibility;
        AssetHandle<FinjinPrefab> prefabHandle;
        MathMatrix4 transform;
        DynamicVector<FinjinSceneObjectBase*> objects;
        size_t parentNodeIndex;
        FinjinSceneNode* parentNodePointer;
        DynamicVector<FinjinSceneNode*> childNodePointers;
        DynamicVector<AssetHandle<FinjinNodeAnimation> > nodeAnimationHandles;

        SceneNodeState frameStages[EngineConstants::MAX_FRAME_STAGES];
    };

    struct FinjinSceneEnvironment
    {
        FinjinSceneEnvironment(Allocator* allocator) :
            fog(allocator),
            shadows(allocator),
            backgroundColor(0, 0, 0, 0),
            range{ 0, 0 }
        {
        }

        AmbientLight ambientLight;
        MathVector4 backgroundColor;
        float range[2];

        struct Fog
        {
            Fog(Allocator* allocator) :
                mode(allocator),
                expDensity(1),
                range{ 0, 0 },
                color(0, 0, 0, 0)
            {
            }

            Utf8String mode;
            float expDensity;
            float range[2];
            MathVector4 color;
        };

        struct Shadows
        {
            Shadows(Allocator* allocator) :
                technique(allocator),
                farDistance(0),
                color(0, 0, 0, 0)
            {
            }

            Utf8String technique;
            float farDistance;
            MathVector4 color;
        };

        struct SkyNode
        {
            SkyNode(Allocator* allocator) :
                typeName(allocator),
                enable(false),
                meshHandle(allocator)
            {
            }

            Utf8String typeName;
            MathMatrix4 transform;
            bool enable;
            AssetHandle<FinjinMesh> meshHandle;
            DynamicVector<AssetHandle<FinjinNodeAnimation> > nodeAnimationHandles;
        };

        Fog fog;
        Shadows shadows;
        DynamicVector<SkyNode> skyNodes;
    };

    class FinjinSubscene : public FinjinSceneObjectBaseState
    {
    public:
        using Super = FinjinSceneObjectBaseState;

        FinjinSubscene(Allocator* allocator) :
            Super(allocator),
            sceneManager(allocator),
            environment(allocator)
        {
        }

        ~FinjinSubscene()
        {
            for (auto& node : this->sceneNodes)
                delete node;
        }

        template <typename T = FinjinSceneObjectBaseState>
        QuerySceneObjectsResult<T> Get(const TypeDescription& typeDescription = FINJIN_TYPE_DESCRIPTION(T))
        {
            QuerySceneObjectsResult<T> result;

            if (typeDescription.IsTypeOf(FINJIN_TYPE_DESCRIPTION(FinjinSceneNode)))
            {
                for (auto node : this->sceneNodes)
                {
                    if (node->IsTypeOf(typeDescription))
                    {
                        node->queryNext = nullptr;
                        result.push_back((T*)node);
                    }
                }
            }
            else
            {
                for (auto node : this->sceneNodes)
                {
                    for (auto object : node->objects)
                    {
                        if (object->IsTypeOf(typeDescription))
                        {
                            object->queryNext = nullptr;
                            result.push_back((T*)object);
                        }
                    }
                }
            }

            return result;
        }

    public:
        Utf8String sceneManager;
        FinjinSceneEnvironment environment;
        DynamicVector<FinjinSceneNode*> sceneNodes;
    };

    FINJIN_ASSET_CLASS class FinjinScene : public FinjinSceneObjectBase
    {
    public:
        using Super = FinjinSceneObjectBase;

        FinjinScene(Allocator* allocator) : Super(allocator)
        {
            this->unitsPerMeter = 1.0f;
        }

        template <typename T = FinjinSceneObjectBaseState>
        QuerySceneObjectsResult<T> Get(const TypeDescription& typeDescription = FINJIN_TYPE_DESCRIPTION(T))
        {
            QuerySceneObjectsResult<T> result;

            if (typeDescription.IsTypeOf(FINJIN_TYPE_DESCRIPTION(FinjinSceneNode)))
            {
                for (auto& subscene : this->subscenes)
                {
                    for (auto node : subscene.sceneNodes)
                    {
                        if (node->IsTypeOf(typeDescription))
                        {
                            node->queryNext = nullptr;
                            result.push_back((T*)node);
                        }
                    }
                }
            }
            else
            {
                for (auto& subscene : this->subscenes)
                {
                    for (auto node : subscene.sceneNodes)
                    {
                        for (auto object : node->objects)
                        {
                            if (object->IsTypeOf(typeDescription))
                            {
                                object->queryNext = nullptr;
                                result.push_back((T*)object);
                            }
                        }
                    }
                }
            }

            return result;
        }

        float unitsPerMeter;

        DynamicVector<FinjinSubscene> subscenes;
        DynamicVector<AssetHandle<FinjinPrefab> > prefabHandles;
        DynamicVector<AssetHandle<FinjinMesh> > meshHandles;
        DynamicVector<AssetHandle<FinjinMaterial> > materialHandles;
        DynamicVector<AssetHandle<FinjinTexture> > textureHandles;
    };

    template <typename T> struct TypeToAssetInfo {};
    template <> struct TypeToAssetInfo<FinjinMaterial> { static AssetClass GetAssetClass() { return AssetClass::MATERIAL; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinMesh> { static AssetClass GetAssetClass() { return AssetClass::MESH; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinMeshSkeleton> { static AssetClass GetAssetClass() { return AssetClass::SKELETON; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinPrefab> { static AssetClass GetAssetClass() { return AssetClass::PREFAB; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinNodeAnimation> { static AssetClass GetAssetClass() { return AssetClass::NODE_ANIMATION; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinMeshSkeletonAnimation> { static AssetClass GetAssetClass() { return AssetClass::SKELETON_ANIMATION; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinMeshMorphAnimation> { static AssetClass GetAssetClass() { return AssetClass::MORPH_ANIMATION; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinMeshPoseAnimation> { static AssetClass GetAssetClass() { return AssetClass::POSE_ANIMATION; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinScene> { static AssetClass GetAssetClass() { return AssetClass::SCENE; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinTexture> { static AssetClass GetAssetClass() { return AssetClass::TEXTURE; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinSound> { static AssetClass GetAssetClass() { return AssetClass::SOUND; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<FinjinShader> { static AssetClass GetAssetClass() { return AssetClass::SHADER; } static bool RequiresInternalDependencyResolution() { return false; } };
    template <> struct TypeToAssetInfo<UserDataTypes> { static AssetClass GetAssetClass() { return AssetClass::USER_DATA_TYPES; } static bool RequiresInternalDependencyResolution() { return true; } };
    template <> struct TypeToAssetInfo<StringTable> { static AssetClass GetAssetClass() { return AssetClass::STRING_TABLE; } static bool RequiresInternalDependencyResolution() { return false; } };

} }
