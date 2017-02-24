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
#if defined(FINJIN_EXPORTER)
    #include "finjin/common/WxChunkName.hpp"
    typedef Finjin::Common::WxChunkName FinjinChunkName;
#else
    #include "finjin/common/ChunkName.hpp"
    typedef Finjin::Common::ChunkName FinjinChunkName;
#endif


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    struct StandardAssetDocumentChunkNames
    {
        static const FinjinChunkName ATTENUATION;
        static const FinjinChunkName BONE;
        static const FinjinChunkName BONES;
        static const FinjinChunkName BONE_ATTACHMENT;
        static const FinjinChunkName BONE_ATTACHMENTS;
        static const FinjinChunkName BOUNDING_VOLUME;
        static const FinjinChunkName CAMERA;
        static const FinjinChunkName CHANNELS;
        static const FinjinChunkName CHANNEL;
        static const FinjinChunkName DOCUMENT_CREATION;
        static const FinjinChunkName ENTITIES;
        static const FinjinChunkName ENTITY;
        static const FinjinChunkName ENVIRONMENT;
        static const FinjinChunkName FACE;
        static const FinjinChunkName FOG;
        static const FinjinChunkName FORMAT;
        static const FinjinChunkName FORMAT_ELEMENT;
        static const FinjinChunkName INDEX_BUFFER;
        static const FinjinChunkName INDEX_BUFFER_RANGE;
        static const FinjinChunkName KEY;
        static const FinjinChunkName KEYS;
        static const FinjinChunkName MANUAL_LOD;
        static const FinjinChunkName MANUAL_LODS;
        static const FinjinChunkName MAP;
        static const FinjinChunkName MAPS;
        static const FinjinChunkName MATERIAL;
        static const FinjinChunkName MATERIALS;
        static const FinjinChunkName MESH;
        static const FinjinChunkName MESHES;
        static const FinjinChunkName MESH_ANIMATION;
        static const FinjinChunkName MESH_ANIMATIONS;
        static const FinjinChunkName MORPH_ANIMATION;
        static const FinjinChunkName MORPH_ANIMATIONS;
        static const FinjinChunkName NODE_ANIMATION;
        static const FinjinChunkName NODE_ANIMATIONS;
        static const FinjinChunkName NODE_ANIMATION_KEY;
        static const FinjinChunkName NODE_ANIMATION_KEYS;
        static const FinjinChunkName NOTE_TRACK;
        static const FinjinChunkName NOTE_TRACKS;
        static const FinjinChunkName OBJECT;
        static const FinjinChunkName OBJECTS;
        static const FinjinChunkName PHYSICAL;
        static const FinjinChunkName PLANE;
        static const FinjinChunkName POINT;
        static const FinjinChunkName POINTS;
        static const FinjinChunkName POSE;
        static const FinjinChunkName POSES;
        static const FinjinChunkName POSE_ANIMATION;
        static const FinjinChunkName POSE_ANIMATIONS;
        static const FinjinChunkName PREFAB;
        static const FinjinChunkName PREFABS;
        static const FinjinChunkName FLAG;
        static const FinjinChunkName FLAGS;
        static const FinjinChunkName SCENE;
        static const FinjinChunkName SCENE_NODE;
        static const FinjinChunkName SCENE_NODES;
        static const FinjinChunkName SHADOWS;
        static const FinjinChunkName SHAPE;
        static const FinjinChunkName SKELETON;
        static const FinjinChunkName SKELETON_ANIMATION;
        static const FinjinChunkName SKELETON_ANIMATIONS;
        static const FinjinChunkName SKY_NODE;
        static const FinjinChunkName SKY_NODES;
        static const FinjinChunkName SUBANIMATION;
        static const FinjinChunkName SUBANIMATIONS;
        static const FinjinChunkName SUBENTITIES;
        static const FinjinChunkName SUBENTITY;
        static const FinjinChunkName SUBMESH;
        static const FinjinChunkName SUBMESHES;
        static const FinjinChunkName SUBPOSE;
        static const FinjinChunkName SUBPOSES;
        static const FinjinChunkName SUBSCENE;
        static const FinjinChunkName SUBSCENES;
        static const FinjinChunkName TEXTURE;
        static const FinjinChunkName TEXTURES;
        static const FinjinChunkName TRACK_TARGET;
        static const FinjinChunkName TRANSFORM;
        static const FinjinChunkName USER_DATA;
        static const FinjinChunkName VERTEX;
        static const FinjinChunkName VERTEX_BONE_ASSIGNMENT;
        static const FinjinChunkName VERTEX_BONE_ASSIGNMENTS;
        static const FinjinChunkName VERTEX_BUFFER;
        static const FinjinChunkName VERTEX_BUFFERS;
        static const FinjinChunkName VERTEX_BUFFER_RANGE;
    };

} }
