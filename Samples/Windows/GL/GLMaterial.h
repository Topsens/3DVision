#pragma once

#include "GLCommon.h"

class GLMaterial
{
public:
    GLMaterial();

    void Ambient(float r, float g, float b, float a);
    void Ambient(const Vector<float, 4>& rgba);
    void Diffuse(float r, float g, float b, float a);
    void Diffuse(const Vector<float, 4>& rgba);
    void Specular(float r, float g, float b, float a);
    void Specular(const Vector<float, 4>& rgba);
    void Emission(float r, float g, float b, float a);
    void Emission(const Vector<float, 4>& rgba);
    void Shininess(float shininess);

    void Apply();
    void Revoke();

private:
    Vector<float, 4> ambient;
    Vector<float, 4> diffuse;
    Vector<float, 4> specular;
    Vector<float, 4> emission;

    float shininess;
};