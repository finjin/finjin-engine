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
#include "finjin/common/Error.hpp"
#include "finjin/common/RequestedValue.hpp"
#include "finjin/common/WindowsUtilities.hpp"
#include "finjin/engine/GenericGpuNumericStructs.hpp"
#include "D3D12Includes.hpp"
#include "D3D12GpuDescription.hpp"
#include "D3D12InputFormat.hpp"


//Macros------------------------------------------------------------------------
#if FINJIN_DEBUG
    #define FINJIN_D3D12_SET_RESOURCE_NAME(resource, name) \
        { \
            Finjin::Common::Utf8StringToWideString _tempWide(name); \
            (resource)->SetName(_tempWide.c_str()); \
        }
#else
    #define FINJIN_D3D12_SET_RESOURCE_NAME(resource, name)
#endif

//The error message should not include a trailing period. Example: "This is an error message"
#define FINJIN_D3D12_SET_ERROR_BLOB(error, message, blob) \
    { \
        if (blob) \
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("%1%: '%2%'", message, Finjin::Common::WindowsUtilities::BlobToString(blob))); \
        else \
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("%1%.", message)); \
    }


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class D3D12Utilities
    {
    public:
        static bool GetBestColorFormat(RequestedValue<DXGI_FORMAT>& colorFormat, const D3D12_FEATURE_DATA_FORMAT_SUPPORT* formatSupportByFormat, size_t formatCount);
        static bool GetBestDepthStencilFormat(RequestedValue<DXGI_FORMAT>& depthStencilFormat, bool stencilRequired, const D3D12_FEATURE_DATA_FORMAT_SUPPORT* formatSupportByFormat, size_t formatCount);

        static D3D_SHADER_MODEL ParseShaderModel(const Utf8String& s, Error& error);
        static ValueOrError<void> ShaderModelToString(Utf8String& result, D3D_SHADER_MODEL model);

        static D3D_FEATURE_LEVEL ParseFeatureLevel(const Utf8String& s, Error& error);
        static ValueOrError<void> FeatureLevelToString(Utf8String& result, D3D_FEATURE_LEVEL level);

        static UINT GetConstantBufferAlignedSize(size_t paddedSize, size_t size, size_t count);

        static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBufferAndUploader
            (
            ID3D12Device* device,
            const void* initData,
            uint64_t byteSize,
            Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer,
            Error& error
            );
    };

} }
