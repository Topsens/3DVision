#include "MainWindow.h"

MainWindow::MainWindow(HINSTANCE instance, UINT acceleratorID)
  : Window(instance), accelID(acceleratorID)
{
}

int MainWindow::Run(int nCmdShow)
{
    if (!this->Create())
    {
        return -1;
    }

    ShowWindow(this->hwnd, nCmdShow);

    HACCEL haccel = nullptr;
    if (this->accelID)
    {
        haccel = LoadAcceleratorsW(this->instance, MAKEINTRESOURCEW(this->accelID));
    }

    return (int)Window::MessageLoop(this->Handle(), haccel);
}

void MainWindow::OnDestroy()
{
    PostQuitMessage(0);
    Window::OnDestroy();
}