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


//Classes----------------------------------------------------------------------
namespace Finjin { namespace Engine {
    
    class InputCoordinate
    {
    public:
        enum class Type
        {
            PIXELS, //Raw pixels
            UIPIXELS, //User interface coordinates
            DIPS, //Device-independent points. Think of how OS X/iOS use the same coordinates regardless of screen density
        };
        
        InputCoordinate();
        InputCoordinate(float value, Type type, float screenDensity);
        InputCoordinate(float value, Type type, float screenDensity, float uiScale);

        static const InputCoordinate ZERO;

        void Reset();

        bool IsZero() const;
        bool IsNonZero() const;

        bool IsNegative() const;
        bool IsPositive() const;

        InputCoordinate Abs() const;

        float ToType(Type type) const;
        void Set(float value, Type type, float screenDensity);
        void Set(float value, Type type, float screenDensity, float uiScale);
        
        float GetScreenDensity() const;
        void SetScreenDensity(float screenDensity);
        
        float GetUIScale() const;
        void SetUIScale(float scale);
        
        InputCoordinate ToPixels() const;
        float ToPixelsValue() const;
        void SetPixels(float value);

        InputCoordinate ToUIPixels() const;
        float ToUIPixelsValue() const;
        void SetUIPixels(float value);
        
        InputCoordinate ToDips() const;
        float ToDipsValue() const;
        void SetDips(float value);

        bool operator == (const InputCoordinate& other) const;
        bool operator != (const InputCoordinate& other) const;

        bool operator < (const InputCoordinate& other) const;
        bool operator <= (const InputCoordinate& other) const;

        bool operator > (const InputCoordinate& other) const;
        bool operator >= (const InputCoordinate& other) const;

        InputCoordinate& operator += (const InputCoordinate& other);
        InputCoordinate operator + (const InputCoordinate& other) const;

        InputCoordinate& operator -= (const InputCoordinate& other);
        InputCoordinate operator - (const InputCoordinate& other) const;
        InputCoordinate operator - () const;

        InputCoordinate& operator *= (float scale);
        InputCoordinate operator * (float scale) const;

        InputCoordinate& operator /= (float scale);
        InputCoordinate operator / (float scale) const;
        
    private:
        float value;
        float screenDensity;
        float uiScale;
        Type type;
    };

} }
