#include "GLMaterial.h"

const Vector<float, 4> DefaultAmbient  = { .2f, .2f, .2f, 1.f };
const Vector<float, 4> DefaultDiffuse  = { .8f, .8f, .8f, 1.f };
const Vector<float, 4> DefaultSpecular = { .0f, .0f, .0f, 1.f };
const Vector<float, 4> DefaultEmission = { .0f, .0f, .0f, 1.f };
const float DefaultShininess = 0.f;

GLMaterial::GLMaterial()
{
    this->Ambient(DefaultAmbient);
    this->Diffuse(DefaultDiffuse);
    this->Specular(DefaultSpecular);
    this->Emission(DefaultEmission);
    this->Shininess(DefaultShininess);
}

void GLMaterial::Ambient(float r, float g, float b, float a)
{
    this->ambient = { r, g, b, a };
}

void GLMaterial::Ambient(const Vector<float, 4>& rgba)
{
    this->ambient = rgba;
}

void GLMaterial::Diffuse(float r, float g, float b, float a)
{
    this->diffuse = { r, g, b, a};
}

void GLMaterial::Diffuse(const Vector<float, 4>& rgba)
{
    this->diffuse = rgba;
}

void GLMaterial::Specular(float r, float g, float b, float a)
{
    this->specular = { r, g, b, a };
}

void GLMaterial::Specular(const Vector<float, 4>& rgba)
{
    this->specular = rgba;
}

void GLMaterial::Emission(float r, float g, float b, float a)
{
    this->emission = { r, g, b, a };
}

void GLMaterial::Emission(const Vector<float, 4>& rgba)
{
    this->emission = rgba;
}

void GLMaterial::Shininess(float shininess)
{
    this->shininess = shininess;
}

void GLMaterial::Apply()
{
    glMaterialfv(GL_FRONT, GL_AMBIENT, this->ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, this->diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, this->specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, this->emission);
    glMaterialf(GL_FRONT, GL_SHININESS, this->shininess);
}

void GLMaterial::Revoke()
{
    glMaterialfv(GL_FRONT, GL_AMBIENT, DefaultAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, DefaultDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, DefaultSpecular);
    glMaterialfv(GL_FRONT, GL_EMISSION, DefaultEmission);
    glMaterialf(GL_FRONT, GL_SHININESS, DefaultShininess);
}