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
#include "finjin/common/Angle.hpp"
#include "finjin/common/Math.hpp"
#include "finjin/engine/CameraState.hpp"


//Types-------------------------------------------------------------------------
namespace Finjin { namespace Engine {

    using namespace Finjin::Common;

    class Camera
    {
    public:
        Camera();
        ~Camera();

        CameraProjectionType GetProjectionType() const;

        const MathVector3& GetPosition() const;
        void SetPosition(const MathVector3& value);

        const MathVector3& GetRight() const;
        const MathVector3& GetUp() const;
        const MathVector3& GetForward() const;

        void Set(const CameraState& cameraState);

        void SetOrientationFromColumns(const MathMatrix3& orientationMatrix);

        float GetNearZ() const;
        void SetNearZ(float value);

        float GetFarZ() const;
        void SetFarZ(float value);

        float GetAspect() const;
        void SetAspect(float value);

        Angle GetFovX() const;

        Angle GetFovY() const;
        void SetFovY(Angle angle);

        float GetNearWindowWidth() const;
        float GetNearWindowHeight() const;
        float GetFarWindowWidth() const;
        float GetFarWindowHeight() const;

        void SetLens(Angle fovY, float aspect, float nearZ, float farZ, float orthoWidth, float orthoHeight, CameraProjectionType projectionType);

        void LookAt(const MathVector3& pos, const MathVector3& target, const MathVector3& up);

        const MathMatrix4& GetViewMatrix() const;
        const MathMatrix4& GetProjectionMatrix() const;

        void Strafe(float d);
        void Pan(float dx, float dy);
        void Walk(float d);

        void Pitch(Angle angle);
        void RotateY(Angle angle);

        //After modifying, call to rebuild the view matrix
        void Update();

    private:
        CameraProjectionType projectionType;

        //Coordinate system relative to world space
        MathVector3 right;
        MathVector3 up;
        MathVector3 forward;
        MathVector3 position;

        //Cached frustum properties
        float nearZ;
        float farZ;
        float aspect;
        float fovY;
        float nearWindowHeight;
        float farWindowHeight;
        float orthoSize[2];

        bool isViewDirty;

        MathMatrix4 viewMatrix;
        MathMatrix4 projectionMatrix;
    };

} }
