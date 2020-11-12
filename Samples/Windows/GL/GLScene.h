#pragma once

#include <gl/glew.h>
#include "GLLight.h"
#include "GLCamera.h"

class GLScene
{
public:
    GLScene();
   ~GLScene();

    GLCamera& Camera();

    void DepthTest(bool enable);

    GLLight& EnableLight(int index);
    void DisableLight(int index);

    virtual void Begin(int width, int height);
    virtual void End();

protected:
    bool enableDepth;

    GLCamera camera;
    GLLight* lights[GL_LIGHT7 - GL_LIGHT0];
};