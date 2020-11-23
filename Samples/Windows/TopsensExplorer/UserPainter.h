#pragma once

#include "GDIRenderer.h"
#include <TopsensSensor.hpp>
#include <mutex>
#include <vector>

class UserPainter
{
public:
    void Draw(const Topsens::UsersFrame& users, Topsens::Orientation orientation, int width, int height);
    void Paint(GDIRenderer& renderer, float scale);

private:
    void Paint(const Topsens::Vector2& joint, GDIRenderer& renderer);
    void Paint(const Topsens::Vector2& beg, const Topsens::Vector2& end, GDIRenderer& renderer);

private:
    float scale;
    std::mutex lock;
    std::vector<std::vector<Topsens::Vector2>> skeletons;
};

