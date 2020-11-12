#pragma once

#include "MainWindow.h"
#include "DepthView.h"
#include "Panel.h"
#include "StatusBar.h"
#include <mutex>
#include <string>
#include <thread>

class Application : public MainWindow
{
public:
    Application(HINSTANCE);

protected:
    bool OnCreated() override;
    void OnDestroy() override;
    void OnSize() override;

private:
    void OnStart();
    void OnStop();
    void OnRefresh();
    void Refresh();
    void Status(const std::wstring&);

    static std::wstring GetError(Topsens::Error);

private:
    Panel     panel;
    StatusBar status;
    DepthView dview;

    bool stop;
    std::thread thread;

    std::mutex   lock;
    std::wstring info;
};