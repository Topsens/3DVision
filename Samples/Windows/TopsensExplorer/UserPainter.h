#pragma once

#include "GdiRenderer.h"
#include <TopsensSensor.hpp>
#include <mutex>
#include <vector>

class UserPainter
{
public:
    void Draw(const Topsens::UsersFrame& users, int width, int height);
    void Orientation(Topsens::Orientation orient);
    void Paint(GdiRenderer& renderer, float scale);

private:
    void Paint(const Topsens::Vector2& joint, GdiRenderer& renderer);
    void Paint(const Topsens::Vector2& beg, const Topsens::Vector2& end, GdiRenderer& renderer);

private:
    float scale;
    std::mutex lock;
    std::vector<std::vector<Topsens::Vector2>> skeletons;
    Topsens::Orientation orient;
};

