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
#include "finjin/common/AllocatedVector.hpp"
#include "finjin/common/AllocatedUnorderedMap.hpp"
#include "finjin/common/Error.hpp"
#include "finjin/engine/GenericGpuNumericStructs.hpp"
#include "D3D12Includes.hpp"


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {

    class D3D12InputFormat
    {    
    public:
        D3D12InputFormat();

        void Create(const GpuInputFormatStruct& inputFormat, Allocator* allocator, Error& error);

        template <typename Dest, typename Source>
        static void CreateVector(Dest& dest, Allocator* allocator, Source& src, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (!dest.Create(src.size(), allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate outer D3D input format collection.");
                return;
            }

            for (size_t i = 0; i < src.size(); i++)
            {
                dest[i].Create(src[i], allocator, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create the '%1%' input format'.", src[i].GetTypeName()));
                    return;
                }
            }
        }

        template <typename Dest, typename Source>
        static void CreateUnorderedMap(Dest& dest, Allocator* allocator, Source& src, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (!dest.Create(src.size(), src.size(), allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to allocate outer D3D input format collection.");
                return;
            }

            for (auto& srcItem : src)
            {
                auto destItem = dest.GetOrAdd(srcItem.GetTypeName().GetHash());
                if (destItem.HasErrorOrValue(nullptr))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add element '%1%'.", srcItem.GetTypeName()));
                    return;
                }
                destItem.value->Create(srcItem, allocator, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create the '%1%' input format'.", srcItem.GetTypeName()));
                    return;
                }
            }
        }

    public:
        size_t inputFormatHash;
        AllocatedVector<D3D12_INPUT_ELEMENT_DESC> elements;
    };

} }
