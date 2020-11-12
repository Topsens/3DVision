#pragma once
#include "GLObject.h"
#include "GLTexture.h"
#include "GLMaterial.h"
#include <vector>

class GLShape : public GLObject
{
public:
    GLShape();
   ~GLShape();

    bool Vertices(const Vertex* vertices, int count);
    //bool Vertices(const Vertex* vertices, const GLuint* indices, int indexCount);
    bool Normals(const Normal* normals, int count);
    //bool Normals(const Normal* normals, const GLuint* indices, int indexCount);
    bool TexCoords(const Coordinate* coords, int count);
    //void TexCoords(const Coordinate* coords, const GLuint* indices, int indexCount);

    GLTexture&  Texture();
    GLMaterial& Material();

    virtual void Render();
    virtual void Release();

protected:
    GLint ApplyVertices();
    GLint ApplyNormals();
    GLint ApplyCoordinates();

    int textureMode;
    GLuint vbo, cbo, nbo;
    GLTexture  texture;
    GLMaterial material;
};
