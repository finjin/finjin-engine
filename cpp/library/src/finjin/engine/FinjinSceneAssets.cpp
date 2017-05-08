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
#include "finjin/engine/FinjinSceneAssets.hpp"
#include "finjin/engine/FinjinSceneTypeNames.hpp"

using namespace Finjin::Engine;


//Implemmentation---------------------------------------------------------------

//FinjinSceneObjectBase
FINJIN_IMPLEMENT_ABSTRACT_BASE_TYPE_DESCRIPTION(FinjinSceneObjectBase, "finjin.scene.object-base");

FinjinSceneObjectBase::FinjinSceneObjectBase(Allocator* allocator) : AllocatedClass(allocator), FinjinSceneObjectBaseState(allocator)
{
}

FinjinSceneObjectBase::~FinjinSceneObjectBase()
{
}


//FinjinSceneMovableObject
FINJIN_IMPLEMENT_ALLOCATED_TYPE_DESCRIPTION(FinjinSceneMovableObject, "finjin.scene.movable-object");

FinjinSceneMovableObject::FinjinSceneMovableObject(Allocator* allocator) : Super(allocator)
{
    this->parentNodePointer = nullptr;
    this->transform.setIdentity();
}

FinjinSceneMovableObject::~FinjinSceneMovableObject()
{
}


//FinjinSceneRenderableMovableObject
FINJIN_IMPLEMENT_ALLOCATED_TYPE_DESCRIPTION(FinjinSceneRenderableMovableObject, "finjin.scene.renderable-movable-object");

FinjinSceneRenderableMovableObject::FinjinSceneRenderableMovableObject(Allocator* allocator) :
    Super(allocator),
    renderQueue(0),
    renderPriority(0),
    renderDistance(0),
    castShadows(false),
    receiveShadows(false)
{
}

FinjinSceneRenderableMovableObject::~FinjinSceneRenderableMovableObject()
{
}


//FinjinSceneObjectEntity
FINJIN_IMPLEMENT_ALLOCATED_TYPE_DESCRIPTION(FinjinSceneObjectEntity, FINJIN_SCENE_ENTITY_TYPE_NAME)

FinjinSceneObjectEntity::FinjinSceneObjectEntity(Allocator* allocator) : Super(allocator), meshHandle(allocator)
{
}

//FinjinSceneObjectCamera
FINJIN_IMPLEMENT_ALLOCATED_TYPE_DESCRIPTION(FinjinSceneObjectCamera, FINJIN_SCENE_CAMERA_TYPE_NAME)

FinjinSceneObjectCamera::FinjinSceneObjectCamera(Allocator* allocator) : Super(allocator)
{
}

CameraState& FinjinSceneObjectCamera::Evaluate(size_t frameStageIndex, size_t updateSequence, const SceneNodeState& sceneNodeState)
{
    auto& result = this->frameStages[frameStageIndex];

    result = *this;
    result.worldMatrix = sceneNodeState.worldMatrix * this->transform;
    result.inverseWorldMatrix = result.worldMatrix.inverse();
    result.inverseTransposeWorldMatrix = result.inverseWorldMatrix.transpose();

    return result;
}

//FinjinSceneObjectLight
FINJIN_IMPLEMENT_ALLOCATED_TYPE_DESCRIPTION(FinjinSceneObjectLight, FINJIN_SCENE_LIGHT_TYPE_NAME)

FinjinSceneObjectLight::FinjinSceneObjectLight(Allocator* allocator) : Super(allocator)
{
    this->gpuLight = nullptr;
}

LightState& FinjinSceneObjectLight::Evaluate(size_t frameStageIndex, size_t updateSequence, const SceneNodeState& sceneNodeState)
{
    auto& result = this->frameStages[frameStageIndex];

    result = *this;
    result.worldMatrix = sceneNodeState.worldMatrix * this->transform;
    result.inverseWorldMatrix = result.worldMatrix.inverse();
    result.inverseTransposeWorldMatrix = result.inverseWorldMatrix.transpose();

    return result;
}

//FinjinSceneNode
FINJIN_IMPLEMENT_ALLOCATED_TYPE_DESCRIPTION(FinjinSceneNode, FINJIN_SCENE_NODE_TYPE_NAME);

FinjinSceneNode::FinjinSceneNode(Allocator* allocator) : Super(allocator), prefabHandle(allocator)
{
    this->visibility = FinjinSceneNodeVisibility::DEFAULT;
    this->parentNodeIndex = (size_t)-1;
    this->parentNodePointer = nullptr;
}

FinjinSceneNode::~FinjinSceneNode()
{
    for (auto obj : this->objects)
        delete obj;
}

SceneNodeState& FinjinSceneNode::Evaluate(size_t frameStageIndex, size_t updateSequence)
{
    auto& result = this->frameStages[frameStageIndex];

    result.visibility = EvaluateVisibility();
    EvaluateTransform(result.worldMatrix);
    result.inverseWorldMatrix = result.worldMatrix.inverse();
    result.inverseTransposeWorldMatrix = result.inverseWorldMatrix.transpose();

    return result;
}

FinjinSceneNodeVisibility FinjinSceneNode::EvaluateVisibility(FinjinSceneNodeVisibility defaultVisibility) const
{
    if (this->visibility == FinjinSceneNodeVisibility::VISIBLE || this->visibility == FinjinSceneNodeVisibility::TREE_VISIBLE)
        return FinjinSceneNodeVisibility::VISIBLE;
    if (this->visibility == FinjinSceneNodeVisibility::HIDDEN || this->visibility == FinjinSceneNodeVisibility::TREE_HIDDEN)
        return FinjinSceneNodeVisibility::HIDDEN;

    for (auto node = this->parentNodePointer; node != nullptr; node = node->parentNodePointer)
    {
        switch (node->visibility)
        {
            case FinjinSceneNodeVisibility::TREE_VISIBLE: return FinjinSceneNodeVisibility::VISIBLE;
            case FinjinSceneNodeVisibility::TREE_HIDDEN: return FinjinSceneNodeVisibility::HIDDEN;
            default: break;
        }
    }

    return defaultVisibility;
}

void FinjinSceneNode::EvaluateTransform(MathMatrix4& result) const
{
    result = this->transform;

    for (auto parent = this->parentNodePointer; parent != nullptr; parent = parent->parentNodePointer)
        result = parent->transform * result;
}

//FinjinMesh
FINJIN_IMPLEMENT_ALLOCATED_TYPE_DESCRIPTION(FinjinMesh, FINJIN_MESH_TYPE_NAME);

//FinjinMaterial
FINJIN_IMPLEMENT_ALLOCATED_TYPE_DESCRIPTION(FinjinMaterial, FINJIN_MATERIAL_TYPE_NAME);
