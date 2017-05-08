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
#include "finjin/engine/TextureDimension.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
TextureDimension TextureDimensionUtilities::Get(size_t height, size_t depth, size_t faceCount, size_t arrayElementCount)
{
    return Get(height, depth > 0, faceCount > 1, arrayElementCount > 0);
}

TextureDimension TextureDimensionUtilities::Get(size_t height, bool is3D, bool isCube, bool isArray)
{
    if (isCube)
    {
        if (isArray)
            return TextureDimension::DIMENSION_CUBE_ARRAY;
        else
            return TextureDimension::DIMENSION_CUBE;
    }
    else if (is3D)
    {
        return TextureDimension::DIMENSION_3D;
    }
    else if (height == 0)
    {
        if (isArray)
            return TextureDimension::DIMENSION_1D_ARRAY;
        else
            return TextureDimension::DIMENSION_1D;
    }
    else
    {
        if (isArray)
            return TextureDimension::DIMENSION_2D_ARRAY;
        else
            return TextureDimension::DIMENSION_2D;
    }
}

TextureDimension TextureDimensionUtilities::GetNonArrayDimension(TextureDimension value)
{
    switch (value)
    {
        case TextureDimension::DIMENSION_1D_ARRAY: return TextureDimension::DIMENSION_1D;
        case TextureDimension::DIMENSION_2D_ARRAY: return TextureDimension::DIMENSION_2D;
        case TextureDimension::DIMENSION_CUBE_ARRAY: return TextureDimension::DIMENSION_CUBE;
        default: return value;
    }
}

bool TextureDimensionUtilities::IsArray(TextureDimension value)
{
    return
        value == TextureDimension::DIMENSION_1D_ARRAY ||
        value == TextureDimension::DIMENSION_2D_ARRAY ||
        value == TextureDimension::DIMENSION_CUBE_ARRAY
        ;
}
