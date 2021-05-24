#ifndef USERPAINTER_H
#define USERPAINTER_H

#include <QPainter>
#include <TopsensSensor.hpp>

class UserPainter
{
public:
    static void Paint(const Topsens::UsersFrame& frame, QPainter& painter, Topsens::Orientation orientation, float thickness);

private:
    static void PaintJoint(const Topsens::Vector2& joint, QPainter& painter, float thickness);
    static void PaintBone(const Topsens::Vector2& beg, const Topsens::Vector2& end, QPainter& painter);
};

#endif // USERPAINTER_H
