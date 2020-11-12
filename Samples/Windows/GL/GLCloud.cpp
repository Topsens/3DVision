#include "GLCloud.h"

GLCloud::GLCloud() : vbo(0), cbo(0), pointSize(1.f)
{
    this->Position = { 0.f, 0.f, 0.f };
    this->Rotation = { 0.f, 0.f, 0.f, 0.f };
    this->Scaling  = { 1.f, 1.f, 1.f };
}

GLCloud::~GLCloud()
{
    this->Release();
}

bool GLCloud::Vertices(const Vertex* vertices, int count)
{
    if (!vertices || count <= 0)
    {
        return false;
    }

    if (this->vbo)
    {
        glDeleteBuffers(1, &this->vbo);
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

bool GLCloud::TexCoords(const Coordinate* coordinates, int count)
{
    if (!coordinates || count <= 0)
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
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(*coordinates), coordinates, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return true;
}

void GLCloud::ClearVertices()
{
    if (this->vbo)
    {
        glDeleteBuffers(1, &this->vbo);
        this->vbo = 0;
    }
}

void GLCloud::ClearTexCoords()
{
    if (this->cbo)
    {
        glDeleteBuffers(1, &this->cbo);
        this->cbo = 0;
    }
}

void GLCloud::Release()
{
    this->ClearVertices();
    this->ClearTexCoords();
    this->texture.Release();
}

GLTexture& GLCloud::Texture()
{
    return this->texture;
}

float GLCloud::PointSize() const
{
    return this->pointSize;
}

void GLCloud::PointSize(float size)
{
    this->pointSize = size;
}

void GLCloud::Render()
{
    auto count = this->ApplyCloud();
    this->ApplyCoordinates();
    this->texture.Apply();

    glPushMatrix();

    glTranslatef(this->Position[0], this->Position[1], this->Position[2]);
    glRotatef(this->Rotation[0], this->Rotation[1], this->Rotation[2], this->Rotation[3]);
    glScalef(this->Scaling[0], this->Scaling[1], this->Scaling[2]);

    glPointSize(this->pointSize);
    glDrawArrays(GL_POINTS, 0, count);

    glPopMatrix();

    this->texture.Revoke();
    this->RevokeCoordinates();
    this->RevokeCloud();
}

GLint GLCloud::ApplyCloud()
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
    else
    {
        return 0;
    }
}

void GLCloud::RevokeCloud()
{
    if (this->vbo)
    {
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glDisableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

GLint GLCloud::ApplyCoordinates()
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
    else
    {
        return 0;
    }
}

void GLCloud::RevokeCoordinates()
{
    if (this->cbo)
    {
        glBindBuffer(GL_ARRAY_BUFFER, this->cbo);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}