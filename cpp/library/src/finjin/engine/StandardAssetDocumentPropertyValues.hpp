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
#if defined(FINJIN_EXPORTER)
    #include <wx/string.h>
    typedef wxString FinjinChunkPropertyString;
#else
    typedef const char* FinjinChunkPropertyString;
#endif


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    struct StandardAssetDocumentPropertyValues
    {
        struct CameraProjection
        {
            static const FinjinChunkPropertyString PERSPECTIVE;
            static const FinjinChunkPropertyString ORTHOGRAPHIC;
        };

        struct CullMode
        {
            static const FinjinChunkPropertyString NORMAL;
            static const FinjinChunkPropertyString NONE;
        };

        struct PolygonMode
        {
            static const FinjinChunkPropertyString SOLID;
            static const FinjinChunkPropertyString WIREFRAME;
        };

        struct ShadingMode
        {
            static const FinjinChunkPropertyString FLAT;
            static const FinjinChunkPropertyString SMOOTH;
        };

        struct Transparency
        {
            static const FinjinChunkPropertyString NONE;
            static const FinjinChunkPropertyString ADDITIVE;
            static const FinjinChunkPropertyString SUBTRACTIVE;
            static const FinjinChunkPropertyString FILTER;
        };

        struct FogMode
        {
            static const FinjinChunkPropertyString LINEAR;
            static const FinjinChunkPropertyString EXP;
            static const FinjinChunkPropertyString EXP2;
        };

        struct IndexBufferType
        {
            static const FinjinChunkPropertyString UINT16;
            static const FinjinChunkPropertyString UINT32;
        };

        struct LightType
        {
            static const FinjinChunkPropertyString SPOT;
            static const FinjinChunkPropertyString POINT;
            static const FinjinChunkPropertyString DIRECTIONAL;
        };

        struct NodeVisibility
        {
            static const FinjinChunkPropertyString VISIBLE;
            static const FinjinChunkPropertyString HIDDEN;
            static const FinjinChunkPropertyString TREE_VISIBLE;
            static const FinjinChunkPropertyString TREE_HIDDEN;
        };

        struct PrimitiveType
        {
            static const FinjinChunkPropertyString TRIANGLE_LIST;
            static const FinjinChunkPropertyString LINE_LIST;
            static const FinjinChunkPropertyString POINT_LIST;
        };

        struct Shape
        {
            static const FinjinChunkPropertyString BOX;
            static const FinjinChunkPropertyString CAPSULE;
            static const FinjinChunkPropertyString CYLINDER;
            static const FinjinChunkPropertyString MESH;
            static const FinjinChunkPropertyString PLANE;
            static const FinjinChunkPropertyString SPHERE;
        };

        struct TextureAddressMode
        {
            static const FinjinChunkPropertyString CLAMP;
            static const FinjinChunkPropertyString MIRROR;
            static const FinjinChunkPropertyString WRAP;
        };

        struct TextureAlpha
        {
            static const FinjinChunkPropertyString PREMULTIPLIED;
            static const FinjinChunkPropertyString STANDARD;
        };

        struct TypeName
        {
            static const FinjinChunkPropertyString CAMERA;
            static const FinjinChunkPropertyString ENTITY;
            static const FinjinChunkPropertyString LIGHT;
            static const FinjinChunkPropertyString MATERIAL_MAP_BUMP;
            static const FinjinChunkPropertyString MATERIAL_MAP_DIFFUSE;
            static const FinjinChunkPropertyString MATERIAL_MAP_ENVIRONMENT;
            static const FinjinChunkPropertyString MATERIAL_MAP_HEIGHT;
            static const FinjinChunkPropertyString MATERIAL_MAP_OPACITY;
            static const FinjinChunkPropertyString MATERIAL_MAP_REFLECTION;
            static const FinjinChunkPropertyString MATERIAL_MAP_REFRACTION;
            static const FinjinChunkPropertyString MATERIAL_MAP_SELF_ILLUMINATION;
            static const FinjinChunkPropertyString MATERIAL_MAP_SHININESS;
            static const FinjinChunkPropertyString MATERIAL_MAP_SPECULAR;
            static const FinjinChunkPropertyString MATERIAL_STANDARD;
            static const FinjinChunkPropertyString MESH;
            static const FinjinChunkPropertyString PHYSICAL;
            static const FinjinChunkPropertyString SCENE_NODE;
            static const FinjinChunkPropertyString SKY;
            static const FinjinChunkPropertyString TRACK_TARGET;
        };
    };

} }
