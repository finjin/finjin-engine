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
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/Chrono.hpp"
#include "finjin/common/ClassDescription.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/AssetClass.hpp"
#include "finjin/engine/AssetHandle.hpp"
#include "finjin/engine/CameraState.hpp"
#include "finjin/engine/GenericGpuNumericStructs.hpp"
#include "finjin/engine/LightState.hpp"
#include "finjin/engine/ShaderFeatureFlag.hpp"
#include "finjin/engine/StringTable.hpp"
#include "finjin/engine/UserDataTypes.hpp"

    
//Classes----------------------------------------------------------------------
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
        MathMatrix44 worldMatrix;
        MathMatrix44 inverseWorldMatrix;
        MathMatrix44 inverseTransposeWorldMatrix;
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

        uint8_t* GetBytes()
        {
            return static_cast<uint8_t*>(data());
        }

        AllocatedVector<float> floatValues;
        AllocatedVector<int32_t> int32Values;
    };

    class ASSET_CLASS(FinjinTexture) : public AllocatedClass
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

    class ASSET_CLASS(FinjinSound) : public AllocatedClass
    {
    public:
        FinjinSound(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
        }

        Utf8String name;
        ByteBuffer fileBytes;
    };
    
    class ASSET_CLASS(FinjinShader) : public AllocatedClass
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
            AllocatedVector<Key> keys;
        };
        AllocatedVector<NoteTrack> noteTracks;

        AllocatedVector<uint8_t> flags;

        FinjinSceneObjectBaseState* queryNext;
    };

    class FinjinSceneObjectBase : public AllocatedClass, public FinjinSceneObjectBaseState
    {
    public:
        FINJIN_DECLARE_ABSTRACT_BASE_CLASS_DESCRIPTION(FinjinSceneObjectBase)

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

        static inline const value_type& GetConstReference(const T* item) { return (const T*)*item; }
        static inline const value_type* GetConstPointer(const T* item) { return (const T&*)item; }
    }; 
    
    template <typename T>
    using QuerySceneObjectsList = IntrusiveSingleList<T, QuerySceneObjectAccessor<T> >;

    template <typename T>
    using QuerySceneObjectsResult = IntrusiveSingleListResult<QuerySceneObjectsList<T> >;

    class ASSET_CLASS(FinjinMaterial) : public FinjinSceneObjectBase
    {
    public:
        FINJIN_DECLARE_CLASS_DESCRIPTION(FinjinMaterial, FinjinSceneObjectBase)

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
        AllocatedVector<Map> maps;

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
        AllocatedVector<FinjinMeshSkeletonBone*> childBonePointers;
    };

    class ASSET_CLASS(FinjinMeshSkeletonAnimation) : public AllocatedClass
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
                MathMatrix44 m44;
            };
            AllocatedVector<Key> keys;
        };
        AllocatedVector<Bone> bones;
    };

    class ASSET_CLASS(FinjinMeshSkeleton) : public AllocatedClass
    {
    public:
        FinjinMeshSkeleton(Allocator* allocator) : AllocatedClass(allocator), name(allocator)
        {
        }

        Utf8String name;
        
        using Bone = FinjinMeshSkeletonBone;
        AllocatedVector<Bone> bones;

        using Animation = FinjinMeshSkeletonAnimation;
        AllocatedVector<AssetHandle<Animation> > animationHandles;
    };

    class ASSET_CLASS(FinjinMeshMorphAnimation) : public AllocatedClass
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
            AllocatedVector<FinjinVertexElementFormat> formatElements;
            struct Key
            {
                TimeDuration time;
                size_t vertexCount;
                AllocatedVector<FinjinVertexElementChannel> channels;
            };
            AllocatedVector<Key> keys;
        };
        AllocatedVector<Subanimation> subanimations;
    };

    class ASSET_CLASS(FinjinMeshPoseAnimation) : public AllocatedClass
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
                AllocatedVector<float> influences;
                AllocatedVector<size_t> submeshIndexes;
            };
            AllocatedVector<Key> keys;
        };
        AllocatedVector<Subanimation> subanimations;
    };

    class ASSET_CLASS(FinjinMesh) : public FinjinSceneObjectBase
    {
    public:
        FINJIN_DECLARE_CLASS_DESCRIPTION(FinjinMesh, FinjinSceneObjectBase)
            
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
                return !this->uint16s.empty() ? (void*)this->uint16s.data() : (void*)this->uint32s.data();
            }

            uint8_t* GetBytes()
            {
                return static_cast<uint8_t*>(data());
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
            AllocatedVector<uint16_t> uint16s;
            AllocatedVector<uint32_t> uint32s;
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

            Utf8String formatName;
            AllocatedVector<FinjinVertexElementFormat> formatElements;
            size_t vertexCount;
            AllocatedVector<FinjinVertexElementChannel> channels;
        };
        
        IndexBuffer indexBuffer;
        AllocatedVector<VertexBuffer> vertexBuffers;

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
            AllocatedVector<VertexBoneAssignment> vertexBoneAssignments;
        };
        AllocatedVector<Submesh> submeshes;
        
        AssetHandle<FinjinMeshSkeleton> skeletonHandle;
        
        using MorphAnimation = FinjinMeshMorphAnimation;
        AllocatedVector<AssetHandle<MorphAnimation> > morphAnimationHandles;
        
        struct Pose
        {
            struct Subpose
            {
                size_t submeshIndex;
                AllocatedVector<FinjinVertexElementFormat> formatElements;
                AllocatedVector<size_t> indexValues;
                AllocatedVector<FinjinVertexElementChannel> channels;
            };
            AllocatedVector<Subpose> subposes;
        };
        AllocatedVector<Pose> poses;
        
        using PoseAnimation = FinjinMeshPoseAnimation;
        AllocatedVector<AssetHandle<PoseAnimation> > poseAnimationHandles;

        struct ManualLod
        {
            ManualLod(Allocator* allocator) : meshHandle(allocator), distance(0)
            {   
            }

            AssetHandle<FinjinMesh> meshHandle;
            float distance;
        };
        AllocatedVector<ManualLod> manualLods;

        VoidHardwareAsset* gpuMesh;
        ShaderFeatureFlag gpuMeshFlags;
    };
    
    class ASSET_CLASS(FinjinPrefab) : public FinjinSceneObjectBase
    {
    public:
        FinjinPrefab(Allocator* allocator) : FinjinSceneObjectBase(allocator)
        {
        }

        AllocatedVector<FinjinSceneNode*> sceneNodes;
    };
    

    class FinjinSceneMovableObject : public FinjinSceneObjectBase
    {
    public:
        FINJIN_DECLARE_CLASS_DESCRIPTION(FinjinSceneMovableObject, FinjinSceneObjectBase)

        FinjinSceneMovableObject(Allocator* allocator);
        ~FinjinSceneMovableObject();
    
    public:
        FinjinSceneNode* parentNodePointer;
        MathMatrix44 transform;
    };

    class FinjinSceneRenderableMovableObject : public FinjinSceneMovableObject
    {
    public:
        FINJIN_DECLARE_CLASS_DESCRIPTION(FinjinSceneRenderableMovableObject, FinjinSceneMovableObject)

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
        FINJIN_DECLARE_CLASS_DESCRIPTION(FinjinSceneObjectEntity, FinjinSceneRenderableMovableObject)

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
        AllocatedVector<Subentity> subentities;
    };

    class FinjinSceneObjectCamera : public FinjinSceneRenderableMovableObject, public CameraState
    {
    public:
        FINJIN_DECLARE_CLASS_DESCRIPTION(FinjinSceneObjectCamera, FinjinSceneRenderableMovableObject)

        FinjinSceneObjectCamera(Allocator* allocator);
        
        CameraState& Evaluate(size_t frameStageIndex, size_t updateSequence, const SceneNodeState& sceneNodeState);

    public:
        CameraState frameStages[EngineConstants::MAX_FRAME_STAGES];
    };

    class FinjinSceneObjectLight : public FinjinSceneRenderableMovableObject, public LightState
    {
    public:
        FINJIN_DECLARE_CLASS_DESCRIPTION(FinjinSceneObjectLight, FinjinSceneRenderableMovableObject)

        FinjinSceneObjectLight(Allocator* allocator);
        
        LightState& Evaluate(size_t frameStageIndex, size_t updateSequence, const SceneNodeState& sceneNodeState);

    public:
        LightState frameStages[EngineConstants::MAX_FRAME_STAGES];
        VoidHardwareAsset* gpuLight;
    };

    class ASSET_CLASS(FinjinNodeAnimation) : public AllocatedClass
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
                this->time = TimeDuration::Zero();
                FINJIN_ZERO_ITEM(this->m44);
            }

            TimeDuration time;
            MathMatrix44 m44;
        };
        AllocatedVector<Key> keys;
    };
    
    class FinjinSceneNode : public FinjinSceneMovableObject
    {
    public:
        FINJIN_DECLARE_CLASS_DESCRIPTION(FinjinSceneNode, FinjinSceneMovableObject)

        FinjinSceneNode(Allocator* allocator);
        ~FinjinSceneNode();

        template <typename T = FinjinSceneObjectBaseState>
        QuerySceneObjectsResult<T> Get(const ClassDescription& classDescription = FINJIN_CLASS_DESCRIPTION(T))
        {
            QuerySceneObjectsResult<T> result;

            for (auto object : this->objects)
            {
                if (object->IsTypeOf(classDescription))
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
        
        void EvaluateTransform(MathMatrix44& result) const;
        
    public:
        Uuid id;
        FinjinSceneNodeVisibility visibility;
        AssetHandle<FinjinPrefab> prefabHandle;
        MathMatrix44 transform;
        AllocatedVector<FinjinSceneObjectBase*> objects;
        size_t parentNodeIndex;
        FinjinSceneNode* parentNodePointer;
        AllocatedVector<FinjinSceneNode*> childNodePointers;
        AllocatedVector<AssetHandle<FinjinNodeAnimation> > nodeAnimationHandles;

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
            MathMatrix44 transform;
            bool enable;
            AssetHandle<FinjinMesh> meshHandle;
            AllocatedVector<AssetHandle<FinjinNodeAnimation> > nodeAnimationHandles;
        };

        Fog fog;
        Shadows shadows;
        AllocatedVector<SkyNode> skyNodes;
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
        QuerySceneObjectsResult<T> Get(const ClassDescription& classDescription = FINJIN_CLASS_DESCRIPTION(T))
        {
            QuerySceneObjectsResult<T> result;

            if (classDescription.IsTypeOf(FINJIN_CLASS_DESCRIPTION(FinjinSceneNode)))
            {
                for (auto node : this->sceneNodes)
                {
                    if (node->IsTypeOf(classDescription))
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
                        if (object->IsTypeOf(classDescription))
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
        AllocatedVector<FinjinSceneNode*> sceneNodes;
    };

    class ASSET_CLASS(FinjinScene) : public FinjinSceneObjectBase
    {
    public:
        using Super = FinjinSceneObjectBase;

        FinjinScene(Allocator* allocator) : Super(allocator)
        {
            this->unitsPerMeter = 1.0f;
        }

        template <typename T = FinjinSceneObjectBaseState>
        QuerySceneObjectsResult<T> Get(const ClassDescription& classDescription = FINJIN_CLASS_DESCRIPTION(T))
        {
            QuerySceneObjectsResult<T> result;

            if (classDescription.IsTypeOf(FINJIN_CLASS_DESCRIPTION(FinjinSceneNode)))
            {
                for (auto& subscene : this->subscenes)
                {
                    for (auto node : subscene.sceneNodes)
                    {
                        if (node->IsTypeOf(classDescription))
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
                            if (object->IsTypeOf(classDescription))
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

        AllocatedVector<FinjinSubscene> subscenes;
        AllocatedVector<AssetHandle<FinjinPrefab> > prefabHandles;
        AllocatedVector<AssetHandle<FinjinMesh> > meshHandles;
        AllocatedVector<AssetHandle<FinjinMaterial> > materialHandles;
        AllocatedVector<AssetHandle<FinjinTexture> > textureHandles;
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
