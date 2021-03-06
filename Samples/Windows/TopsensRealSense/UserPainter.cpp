#include "UserPainter.h"

using namespace std;
using namespace Topsens;

void UserPainter::Draw(const UsersFrame& users, Orientation orientation, int width, int height)
{
    if (users.UserCount)
    {
        this->skeletons.resize(users.UserCount);

        for (uint32_t i = 0; i < users.UserCount; i++)
        {
            this->skeletons[i].resize(JointIndex::Count);

            for (int j = 0; j < JointIndex::Count; j++)
            {
                this->skeletons[i][j] = users.MapTo2D(users.Skeletons[i].Joints[j].Position);
            }
        }
    }
    else
    {
        this->skeletons.clear();
    }

    for (auto& skeleton : this->skeletons)
    {
        for (auto& joint : skeleton)
        {
            auto j = joint;

            if (Orientation::PortraitClockwise == orientation)
            {
                joint.X = width - j.Y - 1;
                joint.Y = j.X;
            }
            else if (Orientation::PortraitAntiClockwise == orientation)
            {
                joint.X = j.Y;
                joint.Y = height - j.X - 1;
            }
        }
    }
    
    this->orient = orientation;
}

void UserPainter::Paint(GdiRenderer& renderer, int x0, int y0, float scale)
{
    this->x0 = x0;
    this->y0 = y0;
    this->scale = scale;

    for (auto& skeleton : this->skeletons)
    {
        if (renderer.Pen(PS_SOLID, 5, RGB(255, 255, 255)))
        {
            this->Paint(skeleton[JointIndex::LShoulder], skeleton[JointIndex::LElbow],    renderer);
            this->Paint(skeleton[JointIndex::RShoulder], skeleton[JointIndex::RElbow],    renderer);
            this->Paint(skeleton[JointIndex::LElbow],    skeleton[JointIndex::LHand],     renderer);
            this->Paint(skeleton[JointIndex::RElbow],    skeleton[JointIndex::RHand],     renderer);

            if (Orientation::Aerial != this->orient)
            {
                this->Paint(skeleton[JointIndex::Head],      skeleton[JointIndex::Neck],      renderer);
                this->Paint(skeleton[JointIndex::Neck],      skeleton[JointIndex::LShoulder], renderer);
                this->Paint(skeleton[JointIndex::Neck],      skeleton[JointIndex::RShoulder], renderer);
                this->Paint(skeleton[JointIndex::LShoulder], skeleton[JointIndex::RWaist],    renderer);
                this->Paint(skeleton[JointIndex::RShoulder], skeleton[JointIndex::LWaist],    renderer);
                this->Paint(skeleton[JointIndex::LWaist],    skeleton[JointIndex::LKnee],     renderer);
                this->Paint(skeleton[JointIndex::RWaist],    skeleton[JointIndex::RKnee],     renderer);
                this->Paint(skeleton[JointIndex::LKnee],     skeleton[JointIndex::LFoot],     renderer);
                this->Paint(skeleton[JointIndex::RKnee],     skeleton[JointIndex::RFoot],     renderer);
                this->Paint(skeleton[JointIndex::LWaist],    skeleton[JointIndex::RWaist],    renderer);
            }
        }

        if (renderer.Pen(nullptr) && renderer.SolidBrush(RGB(255, 255, 255)))
        {
            this->Paint(skeleton[JointIndex::Head],      renderer);
            this->Paint(skeleton[JointIndex::LShoulder], renderer);
            this->Paint(skeleton[JointIndex::RShoulder], renderer);
            this->Paint(skeleton[JointIndex::LElbow],    renderer);
            this->Paint(skeleton[JointIndex::RElbow],    renderer);
            this->Paint(skeleton[JointIndex::LHand],     renderer);
            this->Paint(skeleton[JointIndex::RHand],     renderer);

            if (Orientation::Aerial != this->orient)
            {
                this->Paint(skeleton[JointIndex::Neck],      renderer);
                this->Paint(skeleton[JointIndex::LWaist],    renderer);
                this->Paint(skeleton[JointIndex::RWaist],    renderer);
                this->Paint(skeleton[JointIndex::LKnee],     renderer);
                this->Paint(skeleton[JointIndex::RKnee],     renderer);
                this->Paint(skeleton[JointIndex::LFoot],     renderer);
                this->Paint(skeleton[JointIndex::RFoot],     renderer);
            }
        }
    }
}

void UserPainter::Paint(const Vector2& joint, GdiRenderer& renderer)
{
    if (isnan(joint.X) || isnan(joint.Y))
    {
        return;
    }

    renderer.Ellipse((int)(joint.X * this->scale) - 7 + this->x0, (int)(joint.Y * this->scale) - 7 + this->y0, (int)(joint.X * this->scale) + 7 + this->x0, (int)(joint.Y * this->scale) + 7 + this->y0);
}

void UserPainter::Paint(const Vector2& beg, const Vector2& end, GdiRenderer& renderer)
{
    if (isnan(beg.X) || isnan(beg.Y) || isnan(end.X) || isnan(end.Y))
    {
        return;
    }

    renderer.From((int)(beg.X * this->scale) + this->x0, (int)(beg.Y * this->scale) + this->y0);
    renderer.LineTo((int)(end.X * this->scale) + this->x0, (int)(end.Y * this->scale) + this->y0);
}