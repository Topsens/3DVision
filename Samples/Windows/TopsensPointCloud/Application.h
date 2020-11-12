#pragma once

#include "MainWindow.h"
#include "StatusBar.h"
#include "CloudView.h"
#include "Panel.h"
#include <TopsensSensor.hpp>
#include <thread>

class Application : public MainWindow
{
public:
    Application(HINSTANCE instance);

protected:
    bool OnCreated() override;
    void OnDestroy() override;
    void OnCommand() override;
    void OnSize() override;

private:
    void Start();
    void Stop();

    static std::wstring GetError(Topsens::Error err);

private:
    StatusBar status;
    CloudView cview;
    Panel     panel;

    bool stop;
    std::thread thread;

    Topsens::Sensor sensor;
};