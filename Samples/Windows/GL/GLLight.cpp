#include "GLLight.h"

int Lights[] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7 };

GLLight::GLLight(int index)
  : index(index)
{
    this->Ambient(0.f, 0.f, 0.f, 0.f);
    this->Diffuse(0.f, 0.f, 0.f, 0.f);
    this->Specular(0.f, 0.f, 0.f, 0.f);
    this->Position(0.f, 0.f, 0.f, 0.f);
}

GLLight::~GLLight()
{
    this->Disable();
}

void GLLight::Ambient(float r, float g, float b, float a)
{
    this->ambient = { r, g, b, a };
}

void GLLight::Ambient(const Vector<float, 4>& rgba)
{
    this->ambient = rgba;
}

void GLLight::Diffuse(float r, float g, float b, float a)
{
    this->diffuse = { r, g, b, a };
}

void GLLight::Diffuse(const Vector<float, 4>& rgba)
{
    this->diffuse = rgba;
}

void GLLight::Specular(float r, float g, float b, float a)
{
    this->specular = { r, g, b, a };
}

void GLLight::Specular(const Vector<float, 4>& rgba)
{
    this->specular = rgba;
}

void GLLight::Position(float x, float y, float z, float w)
{
    this->position = { x, y, z, w };
}

void GLLight::Position(const Vector<float, 4>& position)
{
    this->position = position;
}

void GLLight::Position(const Vector<float, 3>& position)
{
    this->Position(position[0], position[1], position[2], 1.f);
}

void GLLight::Enable()
{
    glLightfv(Lights[this->index], GL_AMBIENT,  this->ambient);
    glLightfv(Lights[this->index], GL_DIFFUSE,  this->diffuse);
    glLightfv(Lights[this->index], GL_SPECULAR, this->specular);
    glLightfv(Lights[this->index], GL_POSITION, this->position);

    glEnable(Lights[this->index]);
}

void GLLight::Disable()
{
    glDisable(Lights[this->index]);
}