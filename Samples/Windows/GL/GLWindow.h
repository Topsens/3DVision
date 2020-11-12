#pragma once

#include "Window.h"
#include <gl/glew.h>

class GLWindow : public Window
{
public:
    GLWindow(HINSTANCE instance = nullptr);
    virtual ~GLWindow();

protected:
    bool AttachContext();
    void DetachContext();

    bool OnCreated() override;
    void OnDestroy() override;

    virtual bool OnContextCreated();

protected:
    HDC   hdc;
    HGLRC hrc;
};