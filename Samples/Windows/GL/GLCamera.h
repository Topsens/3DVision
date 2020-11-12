#pragma once

#include "GLCommon.h"

class GLCamera
{
    friend class GLScene;

public:
    GLCamera();

    bool IsPerspective() const;
    void IsPerspective(bool isPerspective);
    void HorizontalFov(float hfov);
    void OrthogonalWidth(float width);
    void Position(float x, float y, float z);
    void Position(const Vector<float, 3>& position);
    void LookAt(float x, float y, float z);
    void LookAt(const Vector<float, 3>& lookAt);
    void Rotate(float rotate);
    void Clip(float near, float far);
    void Clip(const Vector<float, 2>& clip);

    Vector<float, 3> Position() const;
    Vector<float, 3> LookAt() const;
    Vector<float, 2> Clip() const;

    float Rotate() const;

private:
    void SetProject(int width, int height);
    void SetLookAt();

private:
    Vector<float, 2> clip;
    Vector<float, 3> lookAt;
    Vector<float, 3> position;

    float rotate;
    float hfov;
    float orthWidth;
    bool  perspective;
};