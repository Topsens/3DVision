#include "userpainter.h"
#include <cmath>
#include <vector>

using namespace std;
using namespace Topsens;

void UserPainter::Paint(const UsersFrame& frame, QPainter& painter, float thickness)
{
    for (uint32_t i = 0; i < frame.UserCount; i++)
    {
        auto& skeleton = frame.Skeletons[i];

        vector<Vector2> joints(JointIndex::Count);

        for (int j = 0; j < JointIndex::Count; j++)
        {
            joints[j] = frame.MapTo2D(skeleton.Joints[j].Position);
        }

        painter.setPen(QPen(Qt::lightGray, (int)(8 * thickness)));
        PaintBone(joints[JointIndex::Head],      joints[JointIndex::Neck],   painter);
        PaintBone(joints[JointIndex::LShoulder], joints[JointIndex::Neck],   painter);
        PaintBone(joints[JointIndex::RShoulder], joints[JointIndex::Neck],   painter);
        PaintBone(joints[JointIndex::LShoulder], joints[JointIndex::LElbow], painter);
        PaintBone(joints[JointIndex::RShoulder], joints[JointIndex::RElbow], painter);
        PaintBone(joints[JointIndex::LShoulder], joints[JointIndex::RWaist], painter);
        PaintBone(joints[JointIndex::RShoulder], joints[JointIndex::LWaist], painter);
        PaintBone(joints[JointIndex::LElbow],    joints[JointIndex::LHand],  painter);
        PaintBone(joints[JointIndex::RElbow],    joints[JointIndex::RHand],  painter);
        PaintBone(joints[JointIndex::LWaist],    joints[JointIndex::LKnee],  painter);
        PaintBone(joints[JointIndex::RWaist],    joints[JointIndex::RKnee],  painter);
        PaintBone(joints[JointIndex::LKnee],     joints[JointIndex::LFoot],  painter);
        PaintBone(joints[JointIndex::RKnee],     joints[JointIndex::RFoot],  painter);
        PaintBone(joints[JointIndex::LWaist],    joints[JointIndex::RWaist], painter);

        painter.setBrush(Qt::white);
        PaintJoint(joints[JointIndex::Head],      painter, thickness);
        PaintJoint(joints[JointIndex::Neck],      painter, thickness);
        PaintJoint(joints[JointIndex::LShoulder], painter, thickness);
        PaintJoint(joints[JointIndex::RShoulder], painter, thickness);
        PaintJoint(joints[JointIndex::LElbow],    painter, thickness);
        PaintJoint(joints[JointIndex::RElbow],    painter, thickness);
        PaintJoint(joints[JointIndex::LHand],     painter, thickness);
        PaintJoint(joints[JointIndex::RHand],     painter, thickness);
        PaintJoint(joints[JointIndex::LWaist],    painter, thickness);
        PaintJoint(joints[JointIndex::RWaist],    painter, thickness);
        PaintJoint(joints[JointIndex::LKnee],     painter, thickness);
        PaintJoint(joints[JointIndex::RKnee],     painter, thickness);
        PaintJoint(joints[JointIndex::LFoot],     painter, thickness);
        PaintJoint(joints[JointIndex::RFoot],     painter, thickness);
    }
}

void UserPainter::PaintJoint(const Vector2& joint, QPainter& painter, float thickness)
{
    if (isnan(joint.X) || isnan(joint.Y))
    {
        return;
    }

    auto r = (int)(5 * thickness);
    painter.drawEllipse((int)joint.X - r, (int)joint.Y - r, r * 2, r * 2);
}

void UserPainter::PaintBone(const Vector2& beg, const Vector2& end, QPainter& painter)
{
    if (isnan(beg.X) || isnan(beg.Y) || isnan(end.X) || isnan(end.Y))
    {
        return;
    }

    painter.drawLine((int)beg.X, (int)beg.Y, (int)end.X, (int)end.Y);
}
