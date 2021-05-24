#include "userpainter.h"

#define HEAD      (0)  // 头
#define NECK      (1)  // 脖子
#define LSHOULDER (2)  // 左肩
#define RSHOULDER (3)  // 右肩
#define LELBOW    (4)  // 左肘
#define RELBOW    (5)  // 右肘
#define LHAND     (6)  // 左手
#define RHAND     (7)  // 右手
#define LCHEST    (8)  // 左胸
#define RCHEST    (9)  // 右胸
#define LWAIST    (10) // 左腰
#define RWAIST    (11) // 右腰
#define LKNEE     (12) // 左膝
#define RKNEE     (13) // 右膝
#define LFOOT     (14) // 左脚
#define RFOOT     (15) // 右脚
#define TORSO     (16) // 躯干

using namespace std;
using namespace topsens;

void UserPainter::Draw(const UsersFrame::ConstPtr& frame)
{
    lock_guard<mutex> guard(this->lock);
    this->users.resize(frame->UserCount);

    for (uint32_t i = 0; i < frame->UserCount; i++)
    {
        vector<QPointF> joints;

        for (auto& joint : frame->Skeletons[i].Joints)
        {
            float x =  joint.Position.X / joint.Position.Z * frame->Projection.X + frame->Projection.Z;
            float y = -joint.Position.Y / joint.Position.Z * frame->Projection.Y + frame->Projection.W;
            joints.push_back(QPointF(x, y));
        }

        this->users[i].swap(joints);
    }
}

void UserPainter::OnPaint(QPainter& painter, float scale)
{
    lock_guard<mutex> guard(this->lock);

    for (auto& user : this->users)
    {
        painter.setPen(QPen(Qt::white, (int)(5 * scale)));
        PaintBone(user[HEAD],      user[NECK],   painter);
        PaintBone(user[LSHOULDER], user[NECK],   painter);
        PaintBone(user[RSHOULDER], user[NECK],   painter);
        PaintBone(user[LSHOULDER], user[LELBOW], painter);
        PaintBone(user[RSHOULDER], user[RELBOW], painter);
        PaintBone(user[LSHOULDER], user[RWAIST], painter);
        PaintBone(user[RSHOULDER], user[LWAIST], painter);
        PaintBone(user[LELBOW],    user[LHAND],  painter);
        PaintBone(user[RELBOW],    user[RHAND],  painter);
        PaintBone(user[LWAIST],    user[LKNEE],  painter);
        PaintBone(user[RWAIST],    user[RKNEE],  painter);
        PaintBone(user[LKNEE],     user[LFOOT],  painter);
        PaintBone(user[RKNEE],     user[RFOOT],  painter);
        PaintBone(user[LWAIST],    user[RWAIST], painter);

        painter.setBrush(Qt::white);
        PaintJoint(user[HEAD],      painter, scale);
        PaintJoint(user[NECK],      painter, scale);
        PaintJoint(user[LSHOULDER], painter, scale);
        PaintJoint(user[RSHOULDER], painter, scale);
        PaintJoint(user[LELBOW],    painter, scale);
        PaintJoint(user[RELBOW],    painter, scale);
        PaintJoint(user[LHAND],     painter, scale);
        PaintJoint(user[RHAND],     painter, scale);
        PaintJoint(user[LWAIST],    painter, scale);
        PaintJoint(user[RWAIST],    painter, scale);
        PaintJoint(user[LKNEE],     painter, scale);
        PaintJoint(user[RKNEE],     painter, scale);
        PaintJoint(user[LFOOT],     painter, scale);
        PaintJoint(user[RFOOT],     painter, scale);
    }
}

void UserPainter::PaintJoint(const QPointF& joint, QPainter& painter, float scale)
{
    if (isnan(joint.x()) || isnan(joint.y()))
    {
        return;
    }

    auto r = (int)(5 * scale);
    painter.drawEllipse(joint, r, r);
}

void UserPainter::PaintBone(const QPointF& beg, const QPointF& end, QPainter& painter)
{
    if (isnan(beg.x()) || isnan(beg.y()) || isnan(end.x()) || isnan(end.y()))
    {
        return;
    }

    painter.drawLine(beg, end);
}
