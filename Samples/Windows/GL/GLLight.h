#pragma once

#include "GLCommon.h"
#include <gl/glew.h>

class GLLight
{
public:
    GLLight(int index);
   ~GLLight();

    void Ambient(float r, float g, float b, float a);
    void Ambient(const Vector<float, 4>& rgba);
    void Diffuse(float r, float g, float b, float a);
    void Diffuse(const Vector<float, 4>& rgba);
    void Specular(float r, float g, float b, float a);
    void Specular(const Vector<float, 4>& rgba);
    void Position(float x, float y, float z, float w);
    void Position(const Vector<float, 4>& position);
    void Position(const Vector<float, 3>& position);

    void Enable();
    void Disable();

private:
    int index;

    Vector<float, 4> ambient;
    Vector<float, 4> diffuse;
    Vector<float, 4> specular;
    Vector<float, 4> position;
};