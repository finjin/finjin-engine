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
#include "finjin/engine/Camera.hpp"

using namespace Finjin::Engine;


//Implementation----------------------------------------------------------------
Camera::Camera()
{
    this->position = MathVector3(0.0f, 0.0f, 0.0f);
    this->right = MathVector3(1.0f, 0.0f, 0.0f);
    this->up = MathVector3(0.0f, 1.0f, 0.0f);
    this->look = MathVector3(0.0f, 0.0f, 1.0f);

    this->nearZ = 0.0f;
    this->farZ = 0.0f;
    this->aspect = 0.0f;
    this->fovY = 0.0f;
    this->nearWindowHeight = 0.0f;
    this->farWindowHeight = 0.0f;

    this->isViewDirty = true;

    this->viewMatrix.setIdentity();
    this->projectionMatrix.setIdentity();

    SetLens(Radians(FINJIN_PI * 0.5f), 1.0f, 1.0f, 500.0f);

    SetPosition(0, 1, 10);
}

Camera::~Camera()
{
}

const MathVector3& Camera::GetPosition() const
{
    return this->position;
}

void Camera::SetPosition(float x, float y, float z)
{
    this->position = MathVector3(x, y, z);

    this->isViewDirty = true;
}

void Camera::Set(const CameraState& cameraState)
{
    MathVector4 position = cameraState.worldMatrix * MathVector4(0, 0, 0, 1);
    MathVector4 right = cameraState.worldMatrix * MathVector4(1, 0, 0, 0);
    MathVector4 up = cameraState.worldMatrix * MathVector4(0, 0, 1, 0);
    MathVector4 forward = cameraState.worldMatrix * MathVector4(0, 1, 0, 0);

    this->position = MathVector3(position(0), position(1), position(2));

    SetBases(right.data(), up.data(), forward.data());

    SetLens(cameraState.fovY, this->aspect, cameraState.range[0], cameraState.range[1]);
}

void Camera::SetBases(const float* right, const float* up, const float* forward)
{
    this->right = MathVector3(right[0], right[1], right[2]);
    this->up = MathVector3(up[0], up[1], up[2]);
    this->look = MathVector3(forward[0], forward[1], forward[2]);

    this->isViewDirty = true;
}

const MathVector3& Camera::GetRight() const
{
    return this->right;
}

const MathVector3& Camera::GetUp() const
{
    return this->up;
}

const MathVector3& Camera::GetLook() const
{
    return this->look;
}

float Camera::GetNearZ() const
{
    return this->nearZ;
}

void Camera::SetNearZ(float value)
{
    SetLens(Radians(this->fovY), this->aspect, value, this->farZ);
}

float Camera::GetFarZ() const
{
    return this->farZ;
}

void Camera::SetFarZ(float value)
{
    SetLens(Radians(this->fovY), this->aspect, this->nearZ, value);
}

float Camera::GetAspect() const
{
    return this->aspect;
}

void Camera::SetAspect(float value)
{
    SetLens(Radians(this->fovY), value, this->nearZ, this->farZ);
}

Angle Camera::GetFovX() const
{
    return Radians(2.0f * atan((GetNearWindowWidth() / 2.0f) / this->nearZ));
}

Angle Camera::GetFovY() const
{
    return Radians(this->fovY);
}

void Camera::SetFovY(Angle angle)
{
    SetLens(angle, this->aspect, this->nearZ, this->farZ);
}

float Camera::GetNearWindowWidth() const
{
    return this->aspect * this->nearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
    return this->nearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
    return this->aspect * this->farWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
    return this->farWindowHeight;
}

void Camera::SetLens(Angle fovY, float aspect, float zn, float zf)
{
    this->fovY = fovY.ToRadiansValue();
    this->aspect = aspect;
    this->nearZ = zn;
    this->farZ = zf;

    auto halfFovY = this->fovY / 2.0f;

    this->nearWindowHeight = 2.0f * this->nearZ * tanf(halfFovY);
    this->farWindowHeight = 2.0f * this->farZ * tanf(halfFovY);

    float sinFov = sinf(halfFovY);
    float cosFov = cosf(halfFovY);

    float height = cosFov / sinFov;
    float width = height / this->aspect;
    float fRange = this->farZ / (this->nearZ - this->farZ);

    this->projectionMatrix(0, 0) = width;
    this->projectionMatrix(0, 1) = 0.0f;
    this->projectionMatrix(0, 2) = 0.0f;
    this->projectionMatrix(0, 3) = 0.0f;

    this->projectionMatrix(1, 0) = 0.0f;
    this->projectionMatrix(1, 1) = height;
    this->projectionMatrix(1, 2) = 0.0f;
    this->projectionMatrix(1, 3) = 0.0f;

    this->projectionMatrix(2, 0) = 0.0f;
    this->projectionMatrix(2, 1) = 0.0f;
    this->projectionMatrix(2, 2) = fRange;
    this->projectionMatrix(2, 3) = -1.0f;

    this->projectionMatrix(3, 0) = 0.0f;
    this->projectionMatrix(3, 1) = 0.0f;
    this->projectionMatrix(3, 2) = fRange * this->nearZ;
    this->projectionMatrix(3, 3) = 0.0f;
}

void Camera::LookAt(const MathVector3& pos, const MathVector3& target, const MathVector3& worldUp)
{
    auto L = target - pos;
    auto R = worldUp - L;
    auto U = L.cross(R);

    this->position = pos;
    this->look = L;
    this->right = R;
    this->up = U;

    this->isViewDirty = true;
}

const MathMatrix4& Camera::GetViewMatrix() const
{
    return this->viewMatrix;
}

const MathMatrix4& Camera::GetProjectionMatrix() const
{
    return this->projectionMatrix;
}

void Camera::Strafe(float d)
{
    this->position += d * this->right;

    this->isViewDirty = true;
}

void Camera::Pan(float dx, float dy)
{
    this->position += dx * this->right;
    this->position += dy * MathVector3::UnitY();

    this->isViewDirty = true;
}

void Camera::Walk(float d)
{
    this->position += -d * this->look;

    this->isViewDirty = true;
}

void Camera::Pitch(Angle angle)
{
    //Rotate up and look vector about the right vector
    MathAngleAxis R(angle.ToRadiansValue(), this->right);
    auto m = R.toRotationMatrix();

    this->up = m * this->up;
    this->look = m * this->look;

    this->isViewDirty = true;
}

void Camera::RotateY(Angle angle)
{
    //Rotate the basis vectors about the world y-axis
    MathAngleAxis R(angle.ToRadiansValue(), MathVector3::UnitY());
    auto m = R.toRotationMatrix();

    this->right = m * this->right;
    this->up = m * this->up;
    this->look = m * this->look;

    this->isViewDirty = true;
}

void Camera::Update()
{
    if (this->isViewDirty)
    {
        auto R = this->right;
        auto U = this->up;
        auto L = this->look;
        auto P = this->position;

        //Keep camera's axes orthogonal to each other and of unit length
        L.normalize();
        U = L.cross(R).normalized();

        //U, L already ortho-normal, so no need to normalize cross product
        R = U.cross(L);

        //Fill in the view matrix entries.
        auto x = -P.dot(R);
        auto y = -P.dot(U);
        auto z = -P.dot(L);

        this->right = R;
        this->up = U;
        this->look = L;

        this->viewMatrix(0, 0) = this->right(0);
        this->viewMatrix(0, 1) = this->right(1);
        this->viewMatrix(0, 2) = this->right(2);
        this->viewMatrix(0, 3) = x;

        this->viewMatrix(1, 0) = this->up(0);
        this->viewMatrix(1, 1) = this->up(1);
        this->viewMatrix(1, 2) = this->up(2);
        this->viewMatrix(1, 3) = y;

        this->viewMatrix(2, 0) = this->look(0);
        this->viewMatrix(2, 1) = this->look(1);
        this->viewMatrix(2, 2) = this->look(2);
        this->viewMatrix(2, 3) = z;

        this->viewMatrix(3, 0) = 0.0f;
        this->viewMatrix(3, 1) = 0.0f;
        this->viewMatrix(3, 2) = 0.0f;
        this->viewMatrix(3, 3) = 1.0f;

        this->isViewDirty = false;
    }
}
