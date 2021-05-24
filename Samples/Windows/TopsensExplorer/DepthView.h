#pragma once

#include "Window.h"
#include "UserPainter.h"
#include <TopsensSensor.hpp>
#include <mutex>
#include <string>
#include <vector>

class DepthView : public Window
{
public:
    DepthView();

    void Draw(const Topsens::DepthFrame& frame);
    void Draw(const Topsens::UsersFrame& frame);
    void Ground(const Topsens::Vector4* groundPlane);
    void Error(const wchar_t* error);
    void Orientation(Topsens::Orientation orient);

protected:
    virtual void OnPaint() override;

private:
    void DrawDepth(const Topsens::DepthFrame& frame);
    void DrawGround(const Topsens::DepthFrame& frame);
    void PaintSkeletons(GdiRenderer& renderer);
    void PaintSkeleton(const std::vector<Topsens::Vector2>& skeleton, GdiRenderer& renderer);
    void PaintJoint(const Topsens::Vector2& joint, GdiRenderer& renderer);
    void PaintBone(const Topsens::Vector2& beg, const Topsens::Vector2& end, GdiRenderer& renderer);

    static inline bool IsGround(Topsens::Vector3& point, const Topsens::Vector4& groundPlane, float thresh)
    {
        return abs(point.X * groundPlane.X + point.Y * groundPlane.Y + point.Z * groundPlane.Z + groundPlane.W) < thresh;
    }

private:
    std::mutex   mutex;
    std::wstring error;

    int width;
    int height;
    std::vector<uint32_t> pixels;
    std::vector<uint32_t> palette;
    Topsens::Vector4      ground;
    Topsens::Orientation  orient;

    UserPainter users;
};