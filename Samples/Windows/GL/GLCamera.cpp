#include "GLCamera.h"
#include <gl/glew.h>

GLCamera::GLCamera()
  : perspective(true), rotate(0.f), hfov(90.f), orthWidth(1.f)
{
    this->clip = { 0.f, 0.f };
    this->lookAt = { 0.f, 0.f, 0.f };
    this->position = { 0.f, 0.f, 0.f };
}

bool GLCamera::IsPerspective() const
{
    return this->perspective;
}

void GLCamera::IsPerspective(bool isPerspective)
{
    this->perspective = isPerspective;
}

void GLCamera::HorizontalFov(float hfov)
{
    this->hfov = hfov;
}

void GLCamera::OrthogonalWidth(float width)
{
    this->orthWidth = width;
}

void GLCamera::Position(float x, float y, float z)
{
    this->position = { x, y, z };
}

void GLCamera::Position(const Vector<float, 3>& position)
{
    this->position = position;
}

void GLCamera::LookAt(float x, float y, float z)
{
    this->lookAt = { x, y, z };
}

void GLCamera::LookAt(const Vector<float, 3>& lookAt)
{
    this->lookAt = lookAt;
}

void GLCamera::Rotate(float rotate)
{
    this->rotate = rotate;
}

void GLCamera::Clip(float near, float far)
{
    this->clip = { near, far };
}

void GLCamera::Clip(const Vector<float, 2>& clip)
{
    this->clip = clip;
}

Vector<float, 3> GLCamera::Position() const
{
    return this->position;
}

Vector<float, 3> GLCamera::LookAt() const
{
    return this->lookAt;
}

Vector<float, 2> GLCamera::Clip() const
{
    return this->clip;
}

float GLCamera::Rotate() const
{
    return this->rotate;
}

void GLCamera::SetProject(int width, int height)
{
    glViewport(0, 0, width, height);

    if (this->perspective)
    {
        auto focal = ((float)width / 2.f) / tanf(ToRadian(this->hfov / 2.f));
        auto vfov  = ToDegree(atanf((float)height / focal / 2.f)) * 2.f;

        gluPerspective(vfov, (double)width / (double)height, this->clip[0], this->clip[1]);
    }
    else
    {
        auto l = Length(this->position - this->lookAt);
        auto orthHeight = orthWidth / width * height;
        glOrtho(-.5f * orthWidth, .5f * orthWidth, -.5f * orthHeight, .5f * orthHeight, this->clip[0] - l, this->clip[1] - l);
    }
}

void GLCamera::SetLookAt()
{
    Vector<float, 3> v;
    v = this->position - this->lookAt;

    if (this->perspective)
    {
        Quaternion<float> q;

        if (0.f != v[0] || 0.f != v[2])
        {
            q = Quaternion<float>::From2Vectors(Vector3<float>::ZAxis, { v[0], 0.f, v[2] });
            q = Quaternion<float>::From2Vectors({ v[0], 0.f, v[2] }, v) * q;
        }
        else
        {
            q = Quaternion<float>::From2Vectors(Vector3<float>::ZAxis, v);
        }

        q = Quaternion<float>::FromAxisAngle(v, this->rotate);

        Vector<float, 3> yAxis;
        yAxis = q.Rotate(Vector3<float>::YAxis);

        gluLookAt(this->position[0], this->position[1], this->position[2], this->lookAt[0], this->lookAt[1], this->lookAt[2], yAxis[0], yAxis[1], yAxis[2]);
    }
    else
    {
        if (0.f != v[0] || 0.f != v[2])
        {
            float a = ToDegree(acosf(CosOfVectors<float>(v, { v[0], 0.f, v[2] }))) * (v[1] > 0.f ? 1.f : -1.f);
            glRotatef(a, 1.f, 0.f, 0.f);

            a = ToDegree(acosf(CosOfVectors<float>({ v[0], 0.f, v[2] }, Vector3<float>::ZAxis))) * (v[0] > 0.f ? -1.f : 1.f);
            glRotatef(a, 0.f, 1.f, 0.f);
        }
        else
        {
            float a = 90.f * (v[1] > 0.f ? 1.f : -1.f);
            glRotatef(a, 1.f, 0.f, 0.f);
        }

        glRotatef(this->rotate, v[0], v[1], v[2]);
        glTranslatef(-this->lookAt[0], -this->lookAt[1], -this->lookAt[2]);
    }
}