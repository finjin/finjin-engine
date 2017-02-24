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
#include "StandardAssetDocumentPropertyNames.hpp"

using namespace Finjin::Engine;


//Static initialization--------------------------------------------------------
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ACTIVE("active");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::AMBIENT_COLOR("ambient-color");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::AMOUNT("amount");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ANIMATED("animated");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ANIMATION("animation");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::APPLICATION("application");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ATTENUATION_FACTORS("attenuation-factors");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::AUTHOR("author");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::BACKGROUND_COLOR("background-color");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::BASE("index");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::BATCH_COUNT("batch-count");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::BINORMAL("binormal");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::BIT("bit");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::BONE("bone");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::BOX_CENTER("box-center");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::BOX_SIZE("box-size");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::BUFFER("buffer");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CAST_SHADOWS("cast-shadows");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CHILD_COUNT("child-count");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CLIPPING("clipping");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::COLOR("color");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CONE_RANGE("cone-range");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CONSTANT("constant");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CONTENT("content");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CONTENT_FORMAT("content-format");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CONTENT_FORMAT_VERSION("content-format-version");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CONTENT_SIZE("content-size");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::COUNT("count");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CUBE_CENTER("cube-center");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CUBE_SIZE("cube-size");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::CULL_MODE("cull-mode");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::DIFFUSE_COLOR("diffuse-color");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::DISTANCE("distance");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::DRAW_FIRST("draw-first");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ENABLE("enable");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::EXP_DENSITY("exp-density");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::EXPORT_TIME("export-time");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::EXPORTER_VERSION_TEXT("exporter-version-text");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::EXPORTER_VERSION_NUMBER("exporter-version-number");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::FACE_COUNT("face-count");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::FADE_END("fade-end");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::FADE_START("fade-start");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::FAR_DISTANCE("far-distance");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::FINJIN_VERSION("finjin-version");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::FLAG("flag");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::FORMAT("format");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::FOV("fov");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::HEIGHT("height");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ID("id");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::INDEX("index");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::INFLUENCE("influence");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::INFLUENCE_COUNT("influence-count");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::INNER("inner");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::LAST_AUTHOR("last-author");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::LENGTH("length");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::LENGTH_HINT("_length-hint");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::LIGHT("light");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::LIGHT_TYPE("light-type");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::LINEAR("linear");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::LINEAR_END("linear-end");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::LINEAR_START("linear-start");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::LOCAL_DIRECTION("local-direction");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::LOOP("loop");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::MATERIAL_REF("material-ref");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::MESH_REF("mesh-ref");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::MODE("mode");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::MORPH_ANIMATION_REF("morphanim-ref");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::NAME("name");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::NODE("node");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::NODE_ANIMATION_REF("nodeanim-ref");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::NORMAL("normal");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::OBJECT("object");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::OPACITY("opacity");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ORIGIN("origin");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ORTHO_HEIGHT("ortho-height");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ORTHO_SIZE("ortho-size");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ORTHO_WIDTH("ortho-width");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::OUTER("outer");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::PARENT("parent");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::PLANE("plane");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::POINT_SIZE("point-size");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::POLYGON_MODE("polygon-mode");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::POOL_SIZE("pool-size");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::POSE_ANIMATION_REF("poseanim-ref");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::POSITION("position");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::POSITION_TRANSFORMED("position-transformed");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::POWER("power");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::PREFAB_REF("prefab-ref");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::PRIMITIVE_TYPE("primitive-type");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::PROJECTION_TYPE("projection-type");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::QUADRATIC("quadratic");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::RADIUS("radius");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::RANGE("range");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::RECEIVE_SHADOWS("receive-shadows");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::RENDER_DISTANCE("render-distance");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::RENDER_PRIORITY("render-queue-priority");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::RENDER_QUEUE("render-queue");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ROTATION("rotation");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::ROTATION_TYPE("rotation-type");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SCALE("scale");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SCENE_MANAGER("scene-manager");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SELF_ILLUMINATION_COLOR("self-illumination-color");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SELF_SHADOW("self-shadow");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SHADING_MODE("shading-mode");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SHADOW_COLOR("shadow-color");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SHININESS("shininess");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SIZE("size");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SKELETON_ANIMATION_BLEND_MODE("skeleton-animation-blend-mode");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SKELETON_ANIMATION_REF("skeletonanim-ref");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SKELETON_NAME("skeleton-name");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SKELETON_REF("skeleton-ref");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SORT("sort");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SPECULAR_COLOR("specular-color");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::START("start");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SUBMESH("submesh");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::SUBMESH_NAME("submesh-name");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TANGENT("tangent");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TARGET("target");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TECHNIQUE("technique");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TEXTURE_ADDRESS_MODE("texture-address-mode");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TEXTURE_ALPHA("texture-alpha");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TEXTURE_COORDINATE_SET("texture-coordinate-set");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TEXTURE_OFFSET("texture-offset");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TEXTURE_REF("texture-ref");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TEXTURE_ROTATION("texture-rotation");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TEXTURE_SCALE("texture-scale");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TIME("time");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TRACK_TARGET("track-target");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TRANSFORM("transform");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TRANSLATION("translation");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TRANSPARENCY("transparency");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::TYPE("type");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::UNITS_PER_METER("units-per-meter");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::UNIT_TYPE("unit-type");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::USAGE("usage");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::USER_DATA("user-data");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::USER_DATA_CLASS_NAME("_class");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::USER_DATA_STRING_PROPERTY_COUNT("_string-count");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::VALUES("values");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::VERTEX("vertex");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::VERTEX_COUNT("vertex-count");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::VISIBILITY("visibility");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::VISIBLE("visible");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::WEIGHT("weight");
const FinjinChunkPropertyName StandardAssetDocumentPropertyNames::WIDTH("width");
