#pragma once

#include "GLWindow.h"
#include "GLScene.h"
#include "GLCloud.h"
#include <TopsensSensor.hpp>
#include <mutex>

class CloudView : public GLWindow
{
public:
    void ApplyColor(const Topsens::ColorFrame& frame);
    void Draw(const Topsens::DepthFrame& frame, bool mapColor);
    void Clear();

protected:
    LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM) override;
    bool OnCreated() override;
    void OnDestroy() override;
    void OnPaint() override;

private:
    void GetVertices(const Topsens::DepthFrame& frame, Vertex& weightCenter);
    void GetCoordinates(const Topsens::DepthFrame& frame);

private:
    POINT   cursor;
    GLScene scene;
    GLCloud cloud;

    int cw, ch;
    std::vector<uint32_t>   color;
    std::vector<Vertex>     vertices;
    std::vector<Coordinate> coords;
    std::vector<Coordinate> coordTable;

    std::mutex lock;
};