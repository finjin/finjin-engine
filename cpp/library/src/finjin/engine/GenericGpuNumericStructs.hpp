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
#include "finjin/common/NumericStruct.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    //Metadata common to structured and constants buffers------------------
    class GpuStructuredAndConstantBufferStructMetadata
    {
    public:
        enum class ElementID
        {
            NONE,

            ENVIRONMENT_MATRIX,
            MODEL_MATRIX,
            INVERSE_MODEL_MATRIX,
            INVERSE_TRANSPOSE_MODEL_MATRIX,
            VIEW_MATRIX,
            INVERSE_VIEW_MATRIX,
            INVERSE_TRANSPOSE_VIEW_MATRIX,
            PROJECTION_MATRIX,
            INVERSE_PROJECTION_MATRIX,
            VIEW_PROJECTION_MATRIX,
            INVERSE_VIEW_PROJECTION_MATRIX,
            EYE_POSITION,
            RENDER_TARGET_SIZE,
            INVERSE_RENDER_TARGET_SIZE,
            Z_NEAR,
            Z_FAR,
            TOTAL_ELAPSED_TIME_SECONDS,
            FRAME_ELAPSED_TIME_SECONDS,
            AMBIENT_LIGHT_COLOR,
            LIGHT_COLOR,
            LIGHT_RANGE,
            LIGHT_DIRECTION,
            LIGHT_POSITION,
            LIGHT_INDEX,
            LIGHT_POWER,
            LIGHT_CONE_RANGE,
            LIGHT_SIN_CONE_RANGE,
            MATERIAL_INDEX,
            MATERIAL_AMBIENT_COLOR,
            MATERIAL_DIFFUSE_COLOR,
            MATERIAL_SPECULAR_COLOR,
            MATERIAL_SELF_ILLUMINATION_COLOR,
            MATERIAL_SHININESS,
            MATERIAL_TEXTURE_COORDINATE_MATRIX,
            MATERIAL_OPACITY,
            MAP_DIFFUSE_TEXTURE_INDEX,
            MAP_SPECULAR_TEXTURE_INDEX,
            MAP_REFLECTION_TEXTURE_INDEX,
            MAP_REFRACTION_TEXTURE_INDEX,
            MAP_BUMP_TEXTURE_INDEX,
            MAP_HEIGHT_TEXTURE_INDEX,
            MAP_SELF_ILLUMINATION_TEXTURE_INDEX,
            MAP_OPACITY_TEXTURE_INDEX,
            MAP_ENVIRONMENT_TEXTURE_INDEX,
            MAP_SHININESS_TEXTURE_INDEX,
            MAP_DIFFUSE_AMOUNT,
            MAP_SPECULAR_AMOUNT,
            MAP_REFLECTION_AMOUNT,
            MAP_REFRACTION_AMOUNT,
            MAP_BUMP_AMOUNT,
            MAP_HEIGHT_AMOUNT,
            MAP_SELF_ILLUMINATION_AMOUNT,
            MAP_OPACITY_AMOUNT,
            MAP_ENVIRONMENT_AMOUNT,
            MAP_SHININESS_AMOUNT,

            COUNT
        };

        static const Utf8String& GetConfigElementSectionName()
        {
            static const Utf8String name("element");
            return name;
        }

        template <typename StringType>
        static uint32_t ParseIfdef(const StringType& value)
        {
            return 0;
        }

        template <typename T>
        static void ParseElementID(ElementID& result, const T& value, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(ElementID, ElementID::COUNT) lookup
                (
                "environment-matrix", ElementID::ENVIRONMENT_MATRIX,
                "model-matrix", ElementID::MODEL_MATRIX,
                "inverse-model-matrix", ElementID::INVERSE_MODEL_MATRIX,
                "inverse-transpose-model-matrix", ElementID::INVERSE_TRANSPOSE_MODEL_MATRIX,
                "view-matrix", ElementID::VIEW_MATRIX,
                "inverse-view-matrix", ElementID::INVERSE_VIEW_MATRIX,
                "inverse-transpose-view-matrix", ElementID::INVERSE_TRANSPOSE_VIEW_MATRIX,
                "projection-matrix", ElementID::PROJECTION_MATRIX,
                "inverse-projection-matrix", ElementID::INVERSE_PROJECTION_MATRIX,
                "view-projection-matrix", ElementID::VIEW_PROJECTION_MATRIX,
                "inverse-view-projection-matrix", ElementID::INVERSE_VIEW_PROJECTION_MATRIX,
                "eye-position", ElementID::EYE_POSITION,
                "render-target-size", ElementID::RENDER_TARGET_SIZE,
                "inverse-render-target-size", ElementID::INVERSE_RENDER_TARGET_SIZE,
                "z-near", ElementID::Z_NEAR,
                "z-far", ElementID::Z_FAR,
                "total-elapsed-time-seconds", ElementID::TOTAL_ELAPSED_TIME_SECONDS,
                "frame-elapsed-time-seconds", ElementID::FRAME_ELAPSED_TIME_SECONDS,
                "ambient-light-color", ElementID::AMBIENT_LIGHT_COLOR,
                "light-color", ElementID::LIGHT_COLOR,
                "light-range", ElementID::LIGHT_RANGE,
                "light-direction", ElementID::LIGHT_DIRECTION,
                "light-position", ElementID::LIGHT_POSITION,
                "light-index", ElementID::LIGHT_INDEX,
                "light-power", ElementID::LIGHT_POWER,
                "light-cone-range", ElementID::LIGHT_CONE_RANGE,
                "light-sin-cone-range", ElementID::LIGHT_SIN_CONE_RANGE,
                "material-index", ElementID::MATERIAL_INDEX,
                "material-ambient-color", ElementID::MATERIAL_AMBIENT_COLOR,
                "material-diffuse-color", ElementID::MATERIAL_DIFFUSE_COLOR,
                "material-specular-color", ElementID::MATERIAL_SPECULAR_COLOR,
                "material-self-illumination-color", ElementID::MATERIAL_SELF_ILLUMINATION_COLOR,
                "material-shininess", ElementID::MATERIAL_SHININESS,
                "material-texture-coordinate-matrix", ElementID::MATERIAL_TEXTURE_COORDINATE_MATRIX,
                "material-opacity", ElementID::MATERIAL_OPACITY,
                "map-diffuse-texture-index", ElementID::MAP_DIFFUSE_TEXTURE_INDEX,
                "map-specular-texture-index", ElementID::MAP_SPECULAR_TEXTURE_INDEX,
                "map-reflection-texture-index", ElementID::MAP_REFLECTION_TEXTURE_INDEX,
                "map-refraction-texture-index", ElementID::MAP_REFRACTION_TEXTURE_INDEX,
                "map-bump-texture-index", ElementID::MAP_BUMP_TEXTURE_INDEX,
                "map-height-texture-index", ElementID::MAP_HEIGHT_TEXTURE_INDEX,
                "map-self-illumination-texture-index", ElementID::MAP_SELF_ILLUMINATION_TEXTURE_INDEX,
                "map-opacity-texture-index", ElementID::MAP_OPACITY_TEXTURE_INDEX,
                "map-environment-texture-index", ElementID::MAP_ENVIRONMENT_TEXTURE_INDEX,
                "map-shininess-texture-index", ElementID::MAP_SHININESS_TEXTURE_INDEX,
                "map-diffuse-amount", ElementID::MAP_DIFFUSE_AMOUNT,
                "map-specular-amount", ElementID::MAP_SPECULAR_AMOUNT,
                "map-reflection-amount", ElementID::MAP_REFLECTION_AMOUNT,
                "map-refraction-amount", ElementID::MAP_REFRACTION_AMOUNT,
                "map-bump-amount", ElementID::MAP_BUMP_AMOUNT,
                "map-height-amount", ElementID::MAP_HEIGHT_AMOUNT,
                "map-self-illumination-amount", ElementID::MAP_SELF_ILLUMINATION_AMOUNT,
                "map-opacity-amount", ElementID::MAP_OPACITY_AMOUNT,
                "map-environment-amount", ElementID::MAP_ENVIRONMENT_AMOUNT,
                "map-shininess-amount", ElementID::MAP_SHININESS_AMOUNT
                );

            auto foundAt = lookup.find(value);
            if (foundAt != lookup.end())
                result = foundAt->second;
            else
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse GPU constants/structured buffer ID '%1%'.", value));
        }
    };


    //Generic rendering GPU constant buffer struct------------------------
    class GpuConstantStructMetadata : public GpuStructuredAndConstantBufferStructMetadata
    {
    public:
        static const Utf8String& GetConfigSectionName()
        {
            static const Utf8String name("constant-buffer-format");
            return name;
        }
    };
    using GpuRenderingConstantBufferStruct = NumericStruct<GpuConstantStructMetadata>;
    using GpuRenderingConstantBuffer = NumericStructInstance<GpuRenderingConstantBufferStruct>;


    //Generic rendering GPU structured buffer struct
    /*class GpuStructuredBufferStructMetadata : public GpuStructuredAndConstantBufferStructMetadata
    {
    public:
        static const Utf8String& GetConfigSectionName()
        {
            static const Utf8String name("structured-buffer-format");
            return name;
        }
    };
    using GpuRenderingStructuredBufferStruct = NumericStruct<GpuStructuredBufferStructMetadata>;
    using GpuRenderingStructuredBuffer = NumericStructInstance<GpuRenderingStructuredBufferStruct>;*/

    //Vertex/input format-------------------------------------------------
    class InputFormatStructMetadata
    {
    public:
        static const Utf8String& GetConfigSectionName()
        {
            static const Utf8String name("input-format");
            return name;
        }

        static const Utf8String& GetConfigElementSectionName()
        {
            static const Utf8String name("element");
            return name;
        }

        template <typename StringType>
        static uint32_t ParseIfdef(const StringType& value)
        {
            return 0;
        }
    };

    //Generic vertex format struct
    class GpuInputFormatStructMetadata : public InputFormatStructMetadata
    {
    public:
        enum class ElementID
        {
            NONE,

            BINORMAL, //float4
            /*BLEND_INDICES_0, //uint
            BLEND_INDICES_1, //uint
            BLEND_INDICES_2, //uint
            BLEND_INDICES_3, //uint
            BLEND_WEIGHT_0, //float
            BLEND_WEIGHT_1, //float
            BLEND_WEIGHT_2, //float
            BLEND_WEIGHT_3, //float*/
            COLOR_0, //float4
            COLOR_1, //float4
            COLOR_2, //float4
            COLOR_3, //float4
            NORMAL, //float4
            POSITION, //float4
            POSITION_TRANSFORMED, //float4
            POINT_SIZE, //float
            TANGENT, //float4
            TEX_COORD_0, //float4
            TEX_COORD_1, //float4
            TEX_COORD_2, //float4
            TEX_COORD_3, //float4
            TEX_COORD_4, //float4
            TEX_COORD_5, //float4
            TEX_COORD_6, //float4
            TEX_COORD_7, //float4

            COUNT
        };

        template <typename T>
        static void ParseElementID(ElementID& result, const T& value, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            static const FINJIN_LITERAL_STRING_STATIC_UNORDERED_MAP(ElementID, (size_t)ElementID::COUNT + 2) lookup //"color" and "tex-coord" are extra/redundant
                (
                "binormal", ElementID::BINORMAL,
                /*"blend-indices-0", ElementID::BLEND_INDICES_0,
                "blend-indices-1", ElementID::BLEND_INDICES_1,
                "blend-indices-2", ElementID::BLEND_INDICES_2,
                "blend-indices-3", ElementID::BLEND_INDICES_3,
                "blend-weight-0", ElementID::BLEND_WEIGHT_0,
                "blend-weight-1", ElementID::BLEND_WEIGHT_1,
                "blend-weight-2", ElementID::BLEND_WEIGHT_2,
                "blend-weight-3", ElementID::BLEND_WEIGHT_3,*/
                "color", ElementID::COLOR_0,
                "color-0", ElementID::COLOR_0,
                "color-1", ElementID::COLOR_1,
                "color-2", ElementID::COLOR_2,
                "color-3", ElementID::COLOR_3,
                "normal", ElementID::NORMAL,
                "position", ElementID::POSITION,
                "position-transformed", ElementID::POSITION_TRANSFORMED,
                "point-size", ElementID::POINT_SIZE,
                "tangent", ElementID::TANGENT,
                "tex-coord", ElementID::TEX_COORD_0,
                "tex-coord-0", ElementID::TEX_COORD_0,
                "tex-coord-1", ElementID::TEX_COORD_1,
                "tex-coord-2", ElementID::TEX_COORD_2,
                "tex-coord-3", ElementID::TEX_COORD_3,
                "tex-coord-4", ElementID::TEX_COORD_4,
                "tex-coord-5", ElementID::TEX_COORD_5,
                "tex-coord-6", ElementID::TEX_COORD_6,
                "tex-coord-7", ElementID::TEX_COORD_7
                );

            auto foundAt = lookup.find(value);
            if (foundAt != lookup.end())
                result = foundAt->second;
            else
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse vertex element ID '%1%'.", value));
        }
    };
    using GpuInputFormatStruct = NumericStruct<GpuInputFormatStructMetadata>;
    using GpuInputFormatStructInstance = NumericStructInstance<GpuInputFormatStruct>;

} }
