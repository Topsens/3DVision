#pragma once

#include "Window.h"
#include <mutex>
#include <string>
#include <vector>
#include <TopsensSensor.hpp>

class ColorView : public Window
{
public:
    void Draw(const Topsens::ColorFrame& frame);
    void Error(const wchar_t* error);
    void Orientation(Topsens::Orientation orient);

protected:
    virtual void OnPaint() override;

private:
    std::mutex   mutex;
    std::wstring error;

    int width;
    int height;
    std::vector<uint32_t> pixels;

    Topsens::Orientation orient;
};