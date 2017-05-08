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
#include "StandardAssetDocumentPropertyValues.hpp"
#include "FinjinSceneTypeNames.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#if defined(FINJIN_EXPORTER)
    #define FINJIN_CHUNK_PROPERTY_STRING(x) wxT(x)
#else
    #define FINJIN_CHUNK_PROPERTY_STRING(x) x
#endif


//Static initialization---------------------------------------------------------
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::CameraProjection::PERSPECTIVE(FINJIN_CHUNK_PROPERTY_STRING("perspective"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::CameraProjection::ORTHOGRAPHIC(FINJIN_CHUNK_PROPERTY_STRING("orthographic"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::CullMode::NORMAL(FINJIN_CHUNK_PROPERTY_STRING("normal"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::CullMode::NONE(FINJIN_CHUNK_PROPERTY_STRING("none"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::PolygonMode::SOLID(FINJIN_CHUNK_PROPERTY_STRING("solid"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::PolygonMode::WIREFRAME(FINJIN_CHUNK_PROPERTY_STRING("wireframe"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::ShadingMode::FLAT(FINJIN_CHUNK_PROPERTY_STRING("flat"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::ShadingMode::SMOOTH(FINJIN_CHUNK_PROPERTY_STRING("smooth"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::Transparency::NONE(FINJIN_CHUNK_PROPERTY_STRING("none"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::Transparency::ADDITIVE(FINJIN_CHUNK_PROPERTY_STRING("additive"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::Transparency::SUBTRACTIVE(FINJIN_CHUNK_PROPERTY_STRING("subtractive"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::Transparency::FILTER(FINJIN_CHUNK_PROPERTY_STRING("filter"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::FogMode::LINEAR(FINJIN_CHUNK_PROPERTY_STRING("linear"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::FogMode::EXP(FINJIN_CHUNK_PROPERTY_STRING("exp"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::FogMode::EXP2(FINJIN_CHUNK_PROPERTY_STRING("exp2"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::IndexBufferType::UINT16(FINJIN_CHUNK_PROPERTY_STRING("uint16"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::IndexBufferType::UINT32(FINJIN_CHUNK_PROPERTY_STRING("uint32"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::LightType::SPOT(FINJIN_CHUNK_PROPERTY_STRING("spot"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::LightType::POINT(FINJIN_CHUNK_PROPERTY_STRING("point"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::LightType::DIRECTIONAL(FINJIN_CHUNK_PROPERTY_STRING("directional"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::NodeVisibility::VISIBLE(FINJIN_CHUNK_PROPERTY_STRING("visible"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::NodeVisibility::HIDDEN(FINJIN_CHUNK_PROPERTY_STRING("hidden"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::NodeVisibility::TREE_VISIBLE(FINJIN_CHUNK_PROPERTY_STRING("tree-visible"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::NodeVisibility::TREE_HIDDEN(FINJIN_CHUNK_PROPERTY_STRING("tree-hidden"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::PrimitiveType::TRIANGLE_LIST(FINJIN_CHUNK_PROPERTY_STRING("triangle-list"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::PrimitiveType::LINE_LIST(FINJIN_CHUNK_PROPERTY_STRING("line-list"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::PrimitiveType::POINT_LIST(FINJIN_CHUNK_PROPERTY_STRING("point-list"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::Shape::BOX(FINJIN_CHUNK_PROPERTY_STRING("box"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::Shape::CAPSULE(FINJIN_CHUNK_PROPERTY_STRING("capsule"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::Shape::CYLINDER(FINJIN_CHUNK_PROPERTY_STRING("cylinder"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::Shape::MESH(FINJIN_CHUNK_PROPERTY_STRING("mesh"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::Shape::PLANE(FINJIN_CHUNK_PROPERTY_STRING("plane"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::Shape::SPHERE(FINJIN_CHUNK_PROPERTY_STRING("sphere"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TextureAddressMode::CLAMP(FINJIN_CHUNK_PROPERTY_STRING("clamp"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TextureAddressMode::MIRROR(FINJIN_CHUNK_PROPERTY_STRING("mirror"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TextureAddressMode::WRAP(FINJIN_CHUNK_PROPERTY_STRING("wrap"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TextureAlpha::PREMULTIPLIED(FINJIN_CHUNK_PROPERTY_STRING("premultiplied"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TextureAlpha::STANDARD(FINJIN_CHUNK_PROPERTY_STRING("standard"));

const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::CAMERA(FINJIN_CHUNK_PROPERTY_STRING(FINJIN_SCENE_CAMERA_TYPE_NAME));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::ENTITY(FINJIN_CHUNK_PROPERTY_STRING(FINJIN_SCENE_ENTITY_TYPE_NAME));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::LIGHT(FINJIN_CHUNK_PROPERTY_STRING(FINJIN_SCENE_LIGHT_TYPE_NAME));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_BUMP(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.material.map.bump"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_DIFFUSE(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.material.map.diffuse"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_ENVIRONMENT(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.material.map.environment"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_HEIGHT(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.material.map.height"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_OPACITY(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.material.map.opacity"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFLECTION(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.material.map.reflection"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_REFRACTION(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.material.map.refraction"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SELF_ILLUMINATION(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.material.map.self-illumination"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SHININESS(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.material.map.shininess"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_MAP_SPECULAR(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.material.map.specular"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MATERIAL_STANDARD(FINJIN_CHUNK_PROPERTY_STRING(FINJIN_MATERIAL_TYPE_NAME));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::MESH(FINJIN_CHUNK_PROPERTY_STRING(FINJIN_MESH_TYPE_NAME));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::PHYSICAL(FINJIN_CHUNK_PROPERTY_STRING(FINJIN_SCENE_PHYSICAL_TYPE_NAME));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::SCENE_NODE(FINJIN_CHUNK_PROPERTY_STRING(FINJIN_SCENE_NODE_TYPE_NAME));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::SKY(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.sky"));
const FinjinChunkPropertyString StandardAssetDocumentPropertyValues::TypeName::TRACK_TARGET(FINJIN_CHUNK_PROPERTY_STRING("finjin.scene.track-target"));
