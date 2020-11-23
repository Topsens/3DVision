#pragma once

#include "GDIRenderer.h"
#include <Topsens.hpp>
#include <vector>

class UserPainter
{
public:
    void Draw(const Topsens::UsersFrame& users, Topsens::Orientation orientation, int width, int height);
    void Paint(GDIRenderer& renderer, int x0, int y0, float scale);

private:
    void Paint(const Topsens::Vector2& joint, GDIRenderer& renderer);
    void Paint(const Topsens::Vector2& beg, const Topsens::Vector2& end, GDIRenderer& renderer);

private:
    int x0, y0;
    float scale;
    std::vector<std::vector<Topsens::Vector2>> skeletons;
};

