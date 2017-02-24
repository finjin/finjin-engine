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
    typedef Finjin::Common::WxChunkPropertyName FinjinChunkPropertyName;
#else
    #include "finjin/common/ChunkName.hpp"
    typedef Finjin::Common::ChunkPropertyName FinjinChunkPropertyName;
#endif

    
//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    struct StandardAssetDocumentPropertyNames
    {
        static const FinjinChunkPropertyName ACTIVE;
        static const FinjinChunkPropertyName AMBIENT_COLOR;
        static const FinjinChunkPropertyName AMOUNT;
        static const FinjinChunkPropertyName ANIMATED;
        static const FinjinChunkPropertyName ANIMATION;
        static const FinjinChunkPropertyName APPLICATION;
        static const FinjinChunkPropertyName ATTENUATION_FACTORS;
        static const FinjinChunkPropertyName AUTHOR;
        static const FinjinChunkPropertyName BACKGROUND_COLOR;
        static const FinjinChunkPropertyName BASE;
        static const FinjinChunkPropertyName BATCH_COUNT;
        static const FinjinChunkPropertyName BINORMAL;
        static const FinjinChunkPropertyName BIT;
        static const FinjinChunkPropertyName BONE;
        static const FinjinChunkPropertyName BOX_CENTER;
        static const FinjinChunkPropertyName BOX_SIZE;
        static const FinjinChunkPropertyName BUFFER;
        static const FinjinChunkPropertyName CAST_SHADOWS;
        static const FinjinChunkPropertyName CHILD_COUNT;
        static const FinjinChunkPropertyName CLIPPING;
        static const FinjinChunkPropertyName COLOR;
        static const FinjinChunkPropertyName CONE_RANGE;
        static const FinjinChunkPropertyName CONSTANT;
        static const FinjinChunkPropertyName CONTENT;
        static const FinjinChunkPropertyName CONTENT_FORMAT;
        static const FinjinChunkPropertyName CONTENT_FORMAT_VERSION;
        static const FinjinChunkPropertyName CONTENT_SIZE;
        static const FinjinChunkPropertyName COUNT;
        static const FinjinChunkPropertyName CUBE_CENTER;
        static const FinjinChunkPropertyName CUBE_SIZE;
        static const FinjinChunkPropertyName CULL_MODE;        
        static const FinjinChunkPropertyName DIFFUSE_COLOR;
        static const FinjinChunkPropertyName DISTANCE;
        static const FinjinChunkPropertyName DRAW_FIRST;
        static const FinjinChunkPropertyName ENABLE;
        static const FinjinChunkPropertyName EXP_DENSITY;
        static const FinjinChunkPropertyName EXPORT_TIME;
        static const FinjinChunkPropertyName EXPORTER_VERSION_TEXT;
        static const FinjinChunkPropertyName EXPORTER_VERSION_NUMBER;
        static const FinjinChunkPropertyName FACE_COUNT;
        static const FinjinChunkPropertyName FADE_END;
        static const FinjinChunkPropertyName FADE_START;
        static const FinjinChunkPropertyName FAR_DISTANCE;
        static const FinjinChunkPropertyName FINJIN_VERSION;
        static const FinjinChunkPropertyName FLAG;
        static const FinjinChunkPropertyName FORMAT;
        static const FinjinChunkPropertyName FOV;
        static const FinjinChunkPropertyName HEIGHT;
        static const FinjinChunkPropertyName ID;
        static const FinjinChunkPropertyName INDEX;
        static const FinjinChunkPropertyName INFLUENCE;
        static const FinjinChunkPropertyName INFLUENCE_COUNT;
        static const FinjinChunkPropertyName INNER;
        static const FinjinChunkPropertyName LAST_AUTHOR;
        static const FinjinChunkPropertyName LENGTH;
        static const FinjinChunkPropertyName LENGTH_HINT;
        static const FinjinChunkPropertyName LIGHT;
        static const FinjinChunkPropertyName LIGHT_TYPE;
        static const FinjinChunkPropertyName LINEAR;
        static const FinjinChunkPropertyName LINEAR_END;
        static const FinjinChunkPropertyName LINEAR_START;
        static const FinjinChunkPropertyName LOCAL_DIRECTION;
        static const FinjinChunkPropertyName LOOP;
        static const FinjinChunkPropertyName MATERIAL_REF;
        static const FinjinChunkPropertyName MESH_REF;
        static const FinjinChunkPropertyName MODE;
        static const FinjinChunkPropertyName MORPH_ANIMATION_REF;
        static const FinjinChunkPropertyName NAME;
        static const FinjinChunkPropertyName NODE;
        static const FinjinChunkPropertyName NODE_ANIMATION_REF;
        static const FinjinChunkPropertyName NORMAL;
        static const FinjinChunkPropertyName OBJECT;
        static const FinjinChunkPropertyName OPACITY;
        static const FinjinChunkPropertyName ORIGIN;
        static const FinjinChunkPropertyName ORTHO_HEIGHT;
        static const FinjinChunkPropertyName ORTHO_SIZE;
        static const FinjinChunkPropertyName ORTHO_WIDTH;
        static const FinjinChunkPropertyName OUTER;
        static const FinjinChunkPropertyName PARENT;
        static const FinjinChunkPropertyName PLANE;
        static const FinjinChunkPropertyName POINT_SIZE;
        static const FinjinChunkPropertyName POLYGON_MODE;
        static const FinjinChunkPropertyName POOL_SIZE;
        static const FinjinChunkPropertyName POSE_ANIMATION_REF;
        static const FinjinChunkPropertyName POSITION;
        static const FinjinChunkPropertyName POSITION_TRANSFORMED;
        static const FinjinChunkPropertyName POWER;
        static const FinjinChunkPropertyName PREFAB_REF;
        static const FinjinChunkPropertyName PRIMITIVE_TYPE;
        static const FinjinChunkPropertyName PROJECTION_TYPE;
        static const FinjinChunkPropertyName QUADRATIC;
        static const FinjinChunkPropertyName RADIUS;
        static const FinjinChunkPropertyName RANGE;
        static const FinjinChunkPropertyName RECEIVE_SHADOWS;
        static const FinjinChunkPropertyName RENDER_DISTANCE;
        static const FinjinChunkPropertyName RENDER_PRIORITY;
        static const FinjinChunkPropertyName RENDER_QUEUE;
        static const FinjinChunkPropertyName ROTATION;
        static const FinjinChunkPropertyName ROTATION_TYPE;
        static const FinjinChunkPropertyName SCALE;
        static const FinjinChunkPropertyName SCENE_MANAGER;
        static const FinjinChunkPropertyName SELF_ILLUMINATION_COLOR;
        static const FinjinChunkPropertyName SELF_SHADOW;
        static const FinjinChunkPropertyName SHADING_MODE;
        static const FinjinChunkPropertyName SHADOW_COLOR;
        static const FinjinChunkPropertyName SHININESS;
        static const FinjinChunkPropertyName SIZE;
        static const FinjinChunkPropertyName SKELETON_ANIMATION_BLEND_MODE;
        static const FinjinChunkPropertyName SKELETON_ANIMATION_REF;
        static const FinjinChunkPropertyName SKELETON_NAME;
        static const FinjinChunkPropertyName SKELETON_REF;
        static const FinjinChunkPropertyName SORT;
        static const FinjinChunkPropertyName SPECULAR_COLOR;
        static const FinjinChunkPropertyName START;
        static const FinjinChunkPropertyName SUBMESH;
        static const FinjinChunkPropertyName SUBMESH_NAME;
        static const FinjinChunkPropertyName TANGENT;
        static const FinjinChunkPropertyName TARGET;
        static const FinjinChunkPropertyName TECHNIQUE;
        static const FinjinChunkPropertyName TEXTURE_ADDRESS_MODE;
        static const FinjinChunkPropertyName TEXTURE_ALPHA;
        static const FinjinChunkPropertyName TEXTURE_COORDINATE_SET;
        static const FinjinChunkPropertyName TEXTURE_OFFSET;
        static const FinjinChunkPropertyName TEXTURE_REF;
        static const FinjinChunkPropertyName TEXTURE_ROTATION;
        static const FinjinChunkPropertyName TEXTURE_SCALE;
        static const FinjinChunkPropertyName TIME;
        static const FinjinChunkPropertyName TRACK_TARGET;
        static const FinjinChunkPropertyName TRANSFORM;
        static const FinjinChunkPropertyName TRANSLATION;
        static const FinjinChunkPropertyName TRANSPARENCY;
        static const FinjinChunkPropertyName TYPE;
        static const FinjinChunkPropertyName UNITS_PER_METER;
        static const FinjinChunkPropertyName UNIT_TYPE;
        static const FinjinChunkPropertyName USAGE;
        static const FinjinChunkPropertyName USER_DATA;
        static const FinjinChunkPropertyName USER_DATA_CLASS_NAME;        
        static const FinjinChunkPropertyName USER_DATA_STRING_PROPERTY_COUNT;
        static const FinjinChunkPropertyName VALUES;
        static const FinjinChunkPropertyName VERTEX;
        static const FinjinChunkPropertyName VERTEX_COUNT;
        static const FinjinChunkPropertyName VISIBILITY;
        static const FinjinChunkPropertyName VISIBLE;
        static const FinjinChunkPropertyName WEIGHT;
        static const FinjinChunkPropertyName WIDTH;        
    };

} }
