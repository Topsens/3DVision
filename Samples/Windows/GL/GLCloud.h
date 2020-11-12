#pragma once

#include "GLObject.h"
#include "GLTexture.h"
#include <vector>

class GLCloud : public GLObject
{
public:
    GLCloud();
    virtual ~GLCloud();

    bool Vertices(const Vertex* vertices, int count);
    bool TexCoords(const Coordinate* coordinates, int count);
    void ClearVertices();
    void ClearTexCoords();
    void Release();

    GLTexture& Texture();

    float PointSize() const;
    void  PointSize(float size);

    virtual void Render();

protected:
    GLint ApplyCloud();
    void  RevokeCloud();
    GLint ApplyCoordinates();
    void  RevokeCoordinates();

protected:
    float pointSize;
    GLuint vbo, cbo;
    GLTexture texture;
};