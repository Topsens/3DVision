#pragma once

#include <QPainter>
#include <mutex>
#include <vector>
#include <topsens/UsersFrame.h>

class UserPainter
{
public:
    void Draw(const topsens::UsersFrame::ConstPtr&);
    void OnPaint(QPainter&, float scale);

private:
    static void PaintJoint(const QPointF& joint, QPainter&, float scale);
    static void PaintBone(const QPointF& beg, const QPointF& end, QPainter&);
    
private:
    std::mutex lock;
    std::vector<std::vector<QPointF>> users;
};