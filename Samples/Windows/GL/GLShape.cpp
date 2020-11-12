#include "GLShape.h"
#include <cstring>
#include <vector>

using namespace std;

GLShape::GLShape() : vbo(0), cbo(0), nbo(0)
{
    this->Position = { 0.f, 0.f, 0.f };
    this->Rotation = { 0.f, 0.f, 0.f, 0.f };
    this->Scaling  = { 1.f, 1.f, 1.f };
}

GLShape::~GLShape()
{
    this->Release();
}

bool GLShape::Vertices(const Vertex* vertices, int count)
{
    if (!vertices || count <= 0)
    {
        return false;
    }

    if (this->vbo)
    {
        glDeleteBuffers(1, &this->vbo);
    }

    if (!glGenBuffers)
    {
        int a = 0;
    }

    glGenBuffers(1, &this->vbo);
    if (!this->vbo)
    {
        return false;
    }

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(*vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

bool GLShape::Normals(const Normal* normals, int count)
{
    if (!normals || count <= 0)
    {
        return false;
    }

    if (this->nbo)
    {
        glDeleteBuffers(1, &this->nbo);
    }

    glGenBuffers(1, &this->nbo);
    if (!this->nbo)
    {
        return false;
    }

    glBindBuffer(GL_ARRAY_BUFFER, this->nbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(*normals), normals, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

bool GLShape::TexCoords(const Coordinate* coords, int count)
{
    if (!coords || count <= 0)
    {
        return false;
    }

    if (this->cbo)
    {
        glDeleteBuffers(1, &this->cbo);
    }

    glGenBuffers(1, &this->cbo);
    if (!this->cbo)
    {
        return false;
    }

    glBindBuffer(GL_ARRAY_BUFFER, this->cbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(*coords), coords, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

GLTexture& GLShape::Texture()
{
    return this->texture;
}

GLMaterial& GLShape::Material()
{
    return this->material;
}

void GLShape::Render()
{
    auto count = this->ApplyVertices() / 3;
    this->ApplyNormals();
    this->ApplyCoordinates();
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    this->texture.Apply();
    this->material.Apply();

    glPushMatrix();

    glTranslatef(this->Position[0], this->Position[1], this->Position[2]);
    glRotatef(this->Rotation[0], this->Rotation[1], this->Rotation[2], this->Rotation[3]);
    glScalef(this->Scaling[0], this->Scaling[1], this->Scaling[2]);

    glDrawArrays(GL_TRIANGLES, 0, count);

    glPopMatrix();

    this->material.Revoke();
    this->texture.Revoke();

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void GLShape::Release()
{
    if (this->nbo)
    {
        glDeleteBuffers(1, &this->nbo);
        this->nbo = 0;
    }

    if (this->cbo)
    {
        glDeleteBuffers(1, &this->cbo);
        this->cbo = 0;
    }

    if (this->vbo)
    {
        glDeleteBuffers(1, &this->vbo);
        this->vbo = 0;
    }
}

GLint GLShape::ApplyVertices()
{
    if (this->vbo)
    {
        GLint count;
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &count);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return count;
    }

    return 0;
}

GLint GLShape::ApplyNormals()
{
    if (this->nbo)
    {
        GLint count;
        glBindBuffer(GL_ARRAY_BUFFER, this->nbo);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &count);
        glNormalPointer(GL_FLOAT, 0, 0);
        glEnableClientState(GL_NORMAL_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return count;
    }

    return 0;
}

GLint GLShape::ApplyCoordinates()
{
    if (this->cbo)
    {
        GLint count;
        glBindBuffer(GL_ARRAY_BUFFER, this->cbo);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &count);
        glTexCoordPointer(2, GL_FLOAT, 0, 0);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return count;
    }

    return 0;
}