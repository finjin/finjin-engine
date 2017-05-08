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
#include "InputCoordinate.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
InputCoordinate::InputCoordinate()
{
    Reset();
}

InputCoordinate::InputCoordinate(float value, Type type, float screenDensity)
{
    Set(value, type, screenDensity, 1);
}

InputCoordinate::InputCoordinate(float value, Type type, float screenDensity, float uiScale)
{
    Set(value, type, screenDensity, uiScale);
}

const InputCoordinate& InputCoordinate::GetZero()
{
    static const InputCoordinate value(0, InputCoordinate::Type::DIPS, 1.0f, 1.0f);
    return value;
}

void InputCoordinate::Reset()
{
    Set(0, Type::DIPS, 1.0f, 1.0f);
}

bool InputCoordinate::IsZero() const
{
    return this->value == 0;
}

bool InputCoordinate::IsNonZero() const
{
    return this->value != 0;
}

bool InputCoordinate::IsNegative() const
{
    return this->value < 0;
}

bool InputCoordinate::IsPositive() const
{
    return this->value > 0;
}

InputCoordinate InputCoordinate::Abs() const
{
    return InputCoordinate(std::abs(value), this->type, this->screenDensity, this->uiScale);
}

float InputCoordinate::ToType(Type type) const
{
    switch (type)
    {
        case Type::PIXELS: return ToPixelsValue();
        case Type::UIPIXELS: return ToUIPixelsValue();
        case Type::DIPS: return ToDipsValue();
    }
    return 0;
}

void InputCoordinate::Set(float value, Type type, float screenDensity)
{
    this->value = value;
    this->type = type;
    this->screenDensity = screenDensity;
    this->uiScale = 1;
}

void InputCoordinate::Set(float value, Type type, float screenDensity, float uiScale)
{
    this->value = value;
    this->type = type;
    this->screenDensity = screenDensity;
    this->uiScale = uiScale;
}

float InputCoordinate::GetScreenDensity() const
{
    return this->screenDensity;
}

void InputCoordinate::SetScreenDensity(float screenDensity)
{
    this->screenDensity = screenDensity;
}

float InputCoordinate::GetUIScale() const
{
    return this->uiScale;
}

void InputCoordinate::SetUIScale(float scale)
{
    this->uiScale = scale;
}

InputCoordinate InputCoordinate::ToPixels() const
{
    return InputCoordinate(ToPixelsValue(), Type::PIXELS, this->screenDensity, this->uiScale);
}

float InputCoordinate::ToPixelsValue() const
{
    switch (this->type)
    {
        case Type::PIXELS: return this->value;
        case Type::UIPIXELS: return this->value / GetUIScale();
        case Type::DIPS: return this->value * this->screenDensity;
    }
    return 0;
}

void InputCoordinate::SetPixels(float value)
{
    Set(value, Type::PIXELS, this->screenDensity, this->uiScale);
}

InputCoordinate InputCoordinate::ToUIPixels() const
{
    return InputCoordinate(ToUIPixelsValue(), Type::UIPIXELS, this->screenDensity, this->uiScale);
}

float InputCoordinate::ToUIPixelsValue() const
{
    switch (this->type)
    {
        case Type::PIXELS: return this->value * GetUIScale();
        case Type::UIPIXELS: return this->value;
        case Type::DIPS: return this->value * GetUIScale() * this->screenDensity;
    }
    return 0;
}

void InputCoordinate::SetUIPixels(float value)
{
    Set(value, Type::UIPIXELS, this->screenDensity, this->uiScale);
}

InputCoordinate InputCoordinate::ToDips() const
{
    return InputCoordinate(ToDipsValue(), Type::DIPS, this->screenDensity, this->uiScale);
}

float InputCoordinate::ToDipsValue() const
{
    switch (this->type)
    {
        case Type::PIXELS: return this->value / this->screenDensity;
        case Type::UIPIXELS: return this->value / GetUIScale() / this->screenDensity;
        case Type::DIPS: return this->value;
    }
    return 0;
}

void InputCoordinate::SetDips(float value)
{
    Set(value, Type::DIPS, this->screenDensity, this->uiScale);
}

bool InputCoordinate::operator == (const InputCoordinate& other) const
{
    return ToDipsValue() == other.ToDipsValue();
}

bool InputCoordinate::operator != (const InputCoordinate& other) const
{
    return ToDipsValue() != other.ToDipsValue();
}

bool InputCoordinate::operator < (const InputCoordinate& other) const
{
    return ToDipsValue() < other.ToDipsValue();
}

bool InputCoordinate::operator <= (const InputCoordinate& other) const
{
    return ToDipsValue() <= other.ToDipsValue();
}

bool InputCoordinate::operator > (const InputCoordinate& other) const
{
    return ToDipsValue() > other.ToDipsValue();
}

bool InputCoordinate::operator >= (const InputCoordinate& other) const
{
    return ToDipsValue() >= other.ToDipsValue();
}

InputCoordinate& InputCoordinate::operator += (const InputCoordinate& other)
{
    this->value += other.ToType(this->type);
    return *this;
}

InputCoordinate InputCoordinate::operator + (const InputCoordinate& other) const
{
    return InputCoordinate(this->value + other.ToType(this->type), this->type, this->screenDensity, this->uiScale);
}

InputCoordinate& InputCoordinate::operator -= (const InputCoordinate& other)
{
    this->value -= other.ToType(this->type);
    return *this;
}

InputCoordinate InputCoordinate::operator - (const InputCoordinate& other) const
{
    return InputCoordinate(this->value - other.ToType(this->type), this->type, this->screenDensity, this->uiScale);
}

InputCoordinate InputCoordinate::operator - () const
{
    return InputCoordinate(-this->value, this->type, this->screenDensity, this->uiScale);
}

InputCoordinate& InputCoordinate::operator *= (float scale)
{
    this->value *= scale;
    return *this;
}

InputCoordinate InputCoordinate::operator * (float scale) const
{
    return InputCoordinate(this->value * scale, this->type, this->screenDensity, this->uiScale);
}

InputCoordinate& InputCoordinate::operator /= (float scale)
{
    this->value /= scale;
    return *this;
}

InputCoordinate InputCoordinate::operator / (float scale) const
{
    return InputCoordinate(this->value / scale, this->type, this->screenDensity, this->uiScale);
}
