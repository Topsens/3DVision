#include "GLScene.h"
#include <cmath>
#include <cstring>
#include <stdexcept>

using namespace std;

GLScene::GLScene()
  : enableDepth(true)
{
    memset(this->lights, 0, sizeof(this->lights));
}

GLScene::~GLScene()
{
    for (auto i = 0; i < COUNTOF(this->lights); i++)
    {
        delete this->lights[i];
    }
}

GLCamera& GLScene::Camera()
{
    return this->camera;
}

void GLScene::DepthTest(bool enable)
{
    this->enableDepth = enable;
}

GLLight& GLScene::EnableLight(int index)
{
    if (index < 0 || index > GL_LIGHT7)
    {
        throw out_of_range("Light index can be only between 0 and 7");
    }

    if (!this->lights[index])
    {
        this->lights[index] = new GLLight(index);
    }

    return *this->lights[index];
}

void GLScene::DisableLight(int index)
{
    if (index < 0 || index > GL_LIGHT7)
    {
        throw out_of_range("Light index can be only between 0 and 7");
    }

    delete this->lights[index];
    this->lights[index] = nullptr;
}

void GLScene::Begin(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    this->camera.SetProject(width, height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    this->camera.SetLookAt();

    if (this->enableDepth)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
    }

    bool lighting = false;
    for (auto i = 0; i < COUNTOF(this->lights); i++)
    {
        if (this->lights[i])
        {
            this->lights[i]->Enable();
            lighting = true;
        }
    }

    if (lighting)
    {
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }
}

void GLScene::End()
{
    for (auto i = 0; i < COUNTOF(this->lights); i++)
    {
        if (this->lights[i])
        {
            this->lights[i]->Disable();
        }
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glFlush();
}