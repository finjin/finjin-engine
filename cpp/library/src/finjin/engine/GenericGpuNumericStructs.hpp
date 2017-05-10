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

            static StaticUnorderedMap<size_t, ElementID, (size_t)ElementID::COUNT, FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT(ElementID::COUNT), MapPairConstructNone<size_t, ElementID>, PassthroughHash> lookup
                (
                Utf8String::Hash("environment-matrix"), ElementID::ENVIRONMENT_MATRIX,
                Utf8String::Hash("model-matrix"), ElementID::MODEL_MATRIX,
                Utf8String::Hash("inverse-model-matrix"), ElementID::INVERSE_MODEL_MATRIX,
                Utf8String::Hash("inverse-transpose-model-matrix"), ElementID::INVERSE_TRANSPOSE_MODEL_MATRIX,
                Utf8String::Hash("view-matrix"), ElementID::VIEW_MATRIX,
                Utf8String::Hash("inverse-view-matrix"), ElementID::INVERSE_VIEW_MATRIX,
                Utf8String::Hash("inverse-transpose-view-matrix"), ElementID::INVERSE_TRANSPOSE_VIEW_MATRIX,
                Utf8String::Hash("projection-matrix"), ElementID::PROJECTION_MATRIX,
                Utf8String::Hash("inverse-projection-matrix"), ElementID::INVERSE_PROJECTION_MATRIX,
                Utf8String::Hash("view-projection-matrix"), ElementID::VIEW_PROJECTION_MATRIX,
                Utf8String::Hash("inverse-view-projection-matrix"), ElementID::INVERSE_VIEW_PROJECTION_MATRIX,
                Utf8String::Hash("eye-position"), ElementID::EYE_POSITION,
                Utf8String::Hash("render-target-size"), ElementID::RENDER_TARGET_SIZE,
                Utf8String::Hash("inverse-render-target-size"), ElementID::INVERSE_RENDER_TARGET_SIZE,
                Utf8String::Hash("z-near"), ElementID::Z_NEAR,
                Utf8String::Hash("z-far"), ElementID::Z_FAR,
                Utf8String::Hash("total-elapsed-time-seconds"), ElementID::TOTAL_ELAPSED_TIME_SECONDS,
                Utf8String::Hash("frame-elapsed-time-seconds"), ElementID::FRAME_ELAPSED_TIME_SECONDS,
                Utf8String::Hash("ambient-light-color"), ElementID::AMBIENT_LIGHT_COLOR,
                Utf8String::Hash("light-color"), ElementID::LIGHT_COLOR,
                Utf8String::Hash("light-range"), ElementID::LIGHT_RANGE,
                Utf8String::Hash("light-direction"), ElementID::LIGHT_DIRECTION,
                Utf8String::Hash("light-position"), ElementID::LIGHT_POSITION,
                Utf8String::Hash("light-index"), ElementID::LIGHT_INDEX,
                Utf8String::Hash("light-power"), ElementID::LIGHT_POWER,
                Utf8String::Hash("light-cone-range"), ElementID::LIGHT_CONE_RANGE,
                Utf8String::Hash("light-sin-cone-range"), ElementID::LIGHT_SIN_CONE_RANGE,
                Utf8String::Hash("material-index"), ElementID::MATERIAL_INDEX,
                Utf8String::Hash("material-ambient-color"), ElementID::MATERIAL_AMBIENT_COLOR,
                Utf8String::Hash("material-diffuse-color"), ElementID::MATERIAL_DIFFUSE_COLOR,
                Utf8String::Hash("material-specular-color"), ElementID::MATERIAL_SPECULAR_COLOR,
                Utf8String::Hash("material-self-illumination-color"), ElementID::MATERIAL_SELF_ILLUMINATION_COLOR,
                Utf8String::Hash("material-shininess"), ElementID::MATERIAL_SHININESS,
                Utf8String::Hash("material-texture-coordinate-matrix"), ElementID::MATERIAL_TEXTURE_COORDINATE_MATRIX,
                Utf8String::Hash("material-opacity"), ElementID::MATERIAL_OPACITY,
                Utf8String::Hash("map-diffuse-texture-index"), ElementID::MAP_DIFFUSE_TEXTURE_INDEX,
                Utf8String::Hash("map-specular-texture-index"), ElementID::MAP_SPECULAR_TEXTURE_INDEX,
                Utf8String::Hash("map-reflection-texture-index"), ElementID::MAP_REFLECTION_TEXTURE_INDEX,
                Utf8String::Hash("map-refraction-texture-index"), ElementID::MAP_REFRACTION_TEXTURE_INDEX,
                Utf8String::Hash("map-bump-texture-index"), ElementID::MAP_BUMP_TEXTURE_INDEX,
                Utf8String::Hash("map-height-texture-index"), ElementID::MAP_HEIGHT_TEXTURE_INDEX,
                Utf8String::Hash("map-self-illumination-texture-index"), ElementID::MAP_SELF_ILLUMINATION_TEXTURE_INDEX,
                Utf8String::Hash("map-opacity-texture-index"), ElementID::MAP_OPACITY_TEXTURE_INDEX,
                Utf8String::Hash("map-environment-texture-index"), ElementID::MAP_ENVIRONMENT_TEXTURE_INDEX,
                Utf8String::Hash("map-shininess-texture-index"), ElementID::MAP_SHININESS_TEXTURE_INDEX,
                Utf8String::Hash("map-diffuse-amount"), ElementID::MAP_DIFFUSE_AMOUNT,
                Utf8String::Hash("map-specular-amount"), ElementID::MAP_SPECULAR_AMOUNT,
                Utf8String::Hash("map-reflection-amount"), ElementID::MAP_REFLECTION_AMOUNT,
                Utf8String::Hash("map-refraction-amount"), ElementID::MAP_REFRACTION_AMOUNT,
                Utf8String::Hash("map-bump-amount"), ElementID::MAP_BUMP_AMOUNT,
                Utf8String::Hash("map-height-amount"), ElementID::MAP_HEIGHT_AMOUNT,
                Utf8String::Hash("map-self-illumination-amount"), ElementID::MAP_SELF_ILLUMINATION_AMOUNT,
                Utf8String::Hash("map-opacity-amount"), ElementID::MAP_OPACITY_AMOUNT,
                Utf8String::Hash("map-environment-amount"), ElementID::MAP_ENVIRONMENT_AMOUNT,
                Utf8String::Hash("map-shininess-amount"), ElementID::MAP_SHININESS_AMOUNT
                );

            Utf8StringHash hash;
            auto foundAt = lookup.find(hash(value));
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

            const size_t extra = 2; //"color" and "tex-coord" are extra/redundant
            static StaticUnorderedMap<size_t, ElementID, (size_t)ElementID::COUNT + extra, FINJIN_OVERSIZE_FULL_STATIC_MAP_BUCKET_COUNT((size_t)ElementID::COUNT + extra), MapPairConstructNone<size_t, ElementID>, PassthroughHash> lookup
                (
                Utf8String::Hash("binormal"), ElementID::BINORMAL,
                /*Utf8String::Hash("blend-indices-0"), ElementID::BLEND_INDICES_0,
                Utf8String::Hash("blend-indices-1"), ElementID::BLEND_INDICES_1,
                Utf8String::Hash("blend-indices-2"), ElementID::BLEND_INDICES_2,
                Utf8String::Hash("blend-indices-3"), ElementID::BLEND_INDICES_3,
                Utf8String::Hash("blend-weight-0"), ElementID::BLEND_WEIGHT_0,
                Utf8String::Hash("blend-weight-1"), ElementID::BLEND_WEIGHT_1,
                Utf8String::Hash("blend-weight-2"), ElementID::BLEND_WEIGHT_2,
                Utf8String::Hash("blend-weight-3"), ElementID::BLEND_WEIGHT_3,*/
                Utf8String::Hash("color"), ElementID::COLOR_0,
                Utf8String::Hash("color-0"), ElementID::COLOR_0,
                Utf8String::Hash("color-1"), ElementID::COLOR_1,
                Utf8String::Hash("color-2"), ElementID::COLOR_2,
                Utf8String::Hash("color-3"), ElementID::COLOR_3,
                Utf8String::Hash("normal"), ElementID::NORMAL,
                Utf8String::Hash("position"), ElementID::POSITION,
                Utf8String::Hash("position-transformed"), ElementID::POSITION_TRANSFORMED,
                Utf8String::Hash("point-size"), ElementID::POINT_SIZE,
                Utf8String::Hash("tangent"), ElementID::TANGENT,
                Utf8String::Hash("tex-coord"), ElementID::TEX_COORD_0,
                Utf8String::Hash("tex-coord-0"), ElementID::TEX_COORD_0,
                Utf8String::Hash("tex-coord-1"), ElementID::TEX_COORD_1,
                Utf8String::Hash("tex-coord-2"), ElementID::TEX_COORD_2,
                Utf8String::Hash("tex-coord-3"), ElementID::TEX_COORD_3,
                Utf8String::Hash("tex-coord-4"), ElementID::TEX_COORD_4,
                Utf8String::Hash("tex-coord-5"), ElementID::TEX_COORD_5,
                Utf8String::Hash("tex-coord-6"), ElementID::TEX_COORD_6,
                Utf8String::Hash("tex-coord-7"), ElementID::TEX_COORD_7
                );

            Utf8StringHash hash;
            auto foundAt = lookup.find(hash(value));
            if (foundAt != lookup.end())
                result = foundAt->second;
            else
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse vertex element ID '%1%'.", value));
        }
    };
    using GpuInputFormatStruct = NumericStruct<GpuInputFormatStructMetadata>;
    using GpuInputFormatStructInstance = NumericStructInstance<GpuInputFormatStruct>;

} }
