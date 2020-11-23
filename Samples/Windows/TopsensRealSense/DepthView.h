#pragma once

#include "Window.h"
#include "UserPainter.h"
#include <Topsens.hpp>
#include <mutex>
#include <string>
#include <vector>

class DepthView : public Window
{
public:
    DepthView();

    void Draw(const uint16_t* depth, uint32_t w, uint32_t h, Topsens::Orientation orientation);
    void Draw(const Topsens::UsersFrame& users, Topsens::Orientation orientation);
    void Error(const std::wstring& error);

protected:
    void OnPaint() override;

private:
    std::mutex   mutex;
    std::wstring error;

    int width;
    int height;
    std::vector<int> pixels;
    std::vector<int> palette;

    UserPainter users;
};