#pragma once

#include "Window.h"

class MainWindow : public Window
{
public:
    MainWindow(HINSTANCE instance = nullptr, UINT acceleratorID = 0);

    int Run(int nCmdShow);

protected:
    virtual void OnDestroy() override;

    UINT accelID;
};