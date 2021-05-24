#include "ulabel.h"
#include <QPainter>

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

ULabel::ULabel(QWidget* parent) : QLabel(parent)
{
}

void ULabel::Draw(const UsersFrame::ConstPtr& users, int32_t orientation)
{
    lock_guard<mutex> guard(this->lock);

    this->users.resize(users->UserCount);

    for (uint32_t i = 0; i < users->UserCount; i++)
    {
        auto& skeleton = users->Skeletons[i];

        vector<QPointF> joints(skeleton.Joints.size());

        for (size_t j = 0; j < skeleton.Joints.size(); j++)
        {
            auto& joint = joints[j];
            joint = Map(skeleton.Joints[j].Position, users->Projection);

            switch (orientation)
            {
                case 1:
                {
                    joint = QPointF(this->width() - joint.y(), joint.x());
                    break;
                }

                case 2:
                {
                    joint = QPointF(joint.y(), this->height() - joint.x());
                    break;
                }

                default: break;
            }
        }

        this->users[i].swap(joints);
    }
}

void ULabel::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    lock_guard<mutex> guard(this->lock);
    for (auto& user : this->users)
    {
        PaintUser(user, painter);
    }
}

void ULabel::PaintUser(const vector<QPointF>& user, QPainter& painter)
{
    painter.setPen(QPen(Qt::white, 4));
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
    PaintJoint(user[HEAD],      painter);
    PaintJoint(user[NECK],      painter);
    PaintJoint(user[LSHOULDER], painter);
    PaintJoint(user[RSHOULDER], painter);
    PaintJoint(user[LELBOW],    painter);
    PaintJoint(user[RELBOW],    painter);
    PaintJoint(user[LHAND],     painter);
    PaintJoint(user[RHAND],     painter);
    PaintJoint(user[LWAIST],    painter);
    PaintJoint(user[RWAIST],    painter);
    PaintJoint(user[LKNEE],     painter);
    PaintJoint(user[RKNEE],     painter);
    PaintJoint(user[LFOOT],     painter);
    PaintJoint(user[RFOOT],     painter);
}

void ULabel::PaintBone(const QPointF& beg, const QPointF& end, QPainter& painter)
{
    if (isnan(beg.x()) || isnan(beg.y()) ||
        isnan(end.x()) || isnan(end.y()))
    {
        return;
    }

    painter.drawLine(beg, end);
}

void ULabel::PaintJoint(const QPointF& joint, QPainter& painter)
{
    if (isnan(joint.x()) || isnan(joint.y()))
    {
        return;
    }

    painter.drawEllipse(joint, 5, 5);
}

QPointF ULabel::Map(const Vector3& joint, const Vector4& projection)
{
    float x =  joint.X / joint.Z * projection.X + projection.Z;
    float y = -joint.Y / joint.Z * projection.Y + projection.W;
    return QPointF(x, y);
}