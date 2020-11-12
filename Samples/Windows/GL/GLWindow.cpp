#include "GLWindow.h"

GLWindow::GLWindow(HINSTANCE instance) : Window(instance), hdc(nullptr), hrc(nullptr)
{
}

GLWindow::~GLWindow()
{
}

bool GLWindow::AttachContext()
{
    if (this->hrc)
    {
        wglMakeCurrent(this->hdc, this->hrc);
        return true;
    }

    return false;
}

void GLWindow::DetachContext()
{
    wglMakeCurrent(nullptr, nullptr);
}

bool GLWindow::OnCreated()
{
    this->hdc = GetDC(this->Handle());
    if (!this->hdc)
    {
        return false;
    }

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    auto pf = ChoosePixelFormat(this->hdc, &pfd);
    if (!pf)
    {
        return false;
    }

    if (!SetPixelFormat(this->hdc, pf, &pfd))
    {
        return false;
    }

    DescribePixelFormat(this->hdc, pf, sizeof(pfd), &pfd);

    this->hrc = wglCreateContext(this->hdc);

    this->AttachContext();
    if (GLEW_OK != glewInit() || !this->OnContextCreated())
    {
        this->DetachContext();
        return false;
    }
    this->DetachContext();

    return true;
}

void GLWindow::OnDestroy()
{
    if (this->hdc)
    {
        if (this->hrc)
        {
            wglDeleteContext(this->hrc);
            this->hrc = nullptr;
        }

        ReleaseDC(this->Handle(), this->hdc);
        this->hdc = nullptr;
    }

    Window::OnDestroy();
}

bool GLWindow::OnContextCreated()
{
    return true;
}