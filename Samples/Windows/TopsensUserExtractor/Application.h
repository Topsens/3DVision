#pragma once

#include "MainWindow.h"
#include <TopsensSensor.hpp>
#include <TopsensUserExtractor.hpp>
#include <mutex>
#include <thread>
#include <vector>

class Application : public MainWindow
{
public:
    Application(HINSTANCE instance);

protected:
    bool OnCreated() override;
    void OnDestroy() override;
    void OnPaint() override;

private:
    bool LoadBackground();

    static std::wstring GetError(Topsens::Error err);

private:
    Topsens::Sensor sensor;
    Topsens::Toolkit::UserExtractor extractor;

    uint32_t imgW, imgH, bkgW, bkgH;
    std::vector<uint8_t>  bkgPixels;
    std::vector<uint32_t> extracted;

    bool stop;
    std::mutex lock;
    std::thread thread;
};