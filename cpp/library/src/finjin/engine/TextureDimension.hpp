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


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    enum class TextureDimension
    {
        DIMENSION_1D,
        DIMENSION_2D,
        DIMENSION_3D,
        DIMENSION_1D_ARRAY,
        DIMENSION_2D_ARRAY,
        DIMENSION_CUBE_ARRAY,
        DIMENSION_CUBE,

        COUNT
    };

    struct TextureDimensionUtilities
    {
        static TextureDimension Get(size_t height, size_t depth, size_t faceCount, size_t arrayElementCount);
        static TextureDimension Get(size_t height, bool is3D, bool isCube, bool isArray);

        static TextureDimension GetNonArrayDimension(TextureDimension value);

        static bool IsArray(TextureDimension value);
    };

} }
