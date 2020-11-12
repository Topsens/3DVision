#pragma once

#include "MainWindow.h"
#include "StatusBar.h"
#include "ColorView.h"
#include "DepthView.h"
#include "Panel.h"
#include <TopsensSensor.hpp>
#include <string>
#include <thread>

class Application : public MainWindow
{
public:
    Application(HINSTANCE);
    ~Application();

protected:
    bool OnCreated() override;
    void OnDestroy() override;
    void OnCommand() override;
    void OnSize()    override;

private:
    void OnRefresh();
    void OnStart();
    void OnStop();

    static std::wstring GetError(Topsens::Error err);

private:
    bool stop;
    std::thread thread;

    ColorView cview;
    DepthView dview;
    StatusBar status;
    Panel     panel;
    Topsens::Sensor sensor;
};