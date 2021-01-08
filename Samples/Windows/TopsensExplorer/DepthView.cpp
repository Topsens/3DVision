#include "DepthView.h"

#define COLOR_FAR       (0xFFFF0000)
#define COLOR_GND    (0xFF808080)

using namespace std;
using namespace Topsens;

DepthView::DepthView()
{
    this->palette.resize(0x1000);

    for (auto d = 0; d < 0x190; d++)
    {
        this->palette[d] = 0xFF000000;
    }

    for (auto d = 0x190; d < 0x400; d++)
    {
        int r = 0xFF - 0xFF * (d - 0x190) / (0x400 - 0x190);
        int g = 0;
        int b = 0xFF;

        this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
    }

    for (auto d = 0x400; d < 0x600; d++)
    {
        int r = 0;
        int g = 0xFF * (d - 0x400) / (0x600 - 0x400);
        int b = 0xFF;

        this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
    }

    for (auto d = 0x600; d < 0x800; d++)
    {
        int r = 0;
        int g = 0xFF;
        int b = 0xFF - 0xFF * (d - 0x600) / (0x800 - 0x600);

        this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
    }

    for (auto d = 0x800; d < 0xC00; d++)
    {
        int r = 0xFF * (d - 0x800) / (0xC00 - 0x800);
        int g = 0xFF;
        int b = 0;

        this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
    }

    for (auto d = 0xC00; d < 0x1000; d++)
    {
        int r = 0xFF;
        int g = 0xFF - 0xFF * (d - 0xC00) / (0x1000 - 0xC00);
        int b = 0;

        this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
    }

    this->Ground(nullptr);
}

void DepthView::Draw(const DepthFrame& frame, Orientation orientation)
{
    if (!isnan(this->ground.X) && !isnan(this->ground.Y) && !isnan(this->ground.Z) && !isnan(this->ground.W))
    {
        this->DrawGround(frame, orientation);
    }
    else
    {
        this->DrawDepth(frame, orientation);
    }
}

void DepthView::Draw(const UsersFrame& frame, Orientation orientation)
{
    this->users.Draw(frame, orientation, this->width, this->height);
}

void DepthView::Ground(const Vector4* ground)
{
    if (ground)
    {
        this->ground = *ground;
    }
    else
    {
        this->ground.X = NAN;
        this->ground.Y = NAN;
        this->ground.W = NAN;
        this->ground.Z = NAN;;
    }
}

void DepthView::Error(const wchar_t* error)
{
    lock_guard<std::mutex> lock(this->mutex);

    if (error)
    {
        this->error = error;
    }
    else
    {
        this->error.clear();
    }

    this->Invalidate();
}

void DepthView::OnPaint()
{
    GDIRenderer renderer;

    if (renderer.BeginPaint(this->Handle()))
    {
        lock_guard<std::mutex> lock(this->mutex);

        if (this->pixels.empty())
        {
            if (renderer.Pen(PS_SOLID, 1, RGB(0, 0, 0)) && renderer.SolidBrush(RGB(255, 255, 255)))
            {
                renderer.Rectangle(0, 0, this->ClientWidth(), this->ClientHeight());
            }
        }
        else
        {
            renderer.PixelsARGB((BYTE*)this->pixels.data(), this->width, this->height, 0, 0, this->ClientWidth(), this->ClientHeight());
        }

        this->users.Paint(renderer, this->ClientWidth() / (float)this->width);

        if (!this->error.empty() && renderer.Font(L"Segou UI", 40))
        {
            renderer.Text(this->error.c_str(), 0, 0, this->ClientWidth(), this->ClientHeight(), DT_CENTER | DT_VCENTER | DT_SINGLELINE, RGB(255, 0, 0));
        }

        renderer.EndPaint();
    }
    else
    {
        Window::OnPaint();
    }
}

void DepthView::DrawDepth(const DepthFrame& frame, Orientation o)
{
    if (Orientation::Landscape == o)
    {
        this->width  = frame.Width;
        this->height = frame.Height;
    }
    else
    {
        this->width  = frame.Height;
        this->height = frame.Width;
    }

    lock_guard<std::mutex> lock(this->mutex);
    this->pixels.resize(this->width * this->height);

    if (Orientation::Landscape == o)
    {
        for (size_t i = 0; i < this->pixels.size(); i++)
        {
            auto d = frame.Pixels[i];
            this->pixels[i] = d < this->palette.size() ? this->palette[d] : COLOR_FAR;
        }
    }
    else
    {
        auto p = frame.Pixels;

        vector<int> off(this->height);
        for (int i = 0; i < this->height; i++)
        {
            off[i] = i * this->width;
        }

        if (Orientation::PortraitClockwise == o)
        {
            for (int i = this->width - 1; i >= 0; i--)
            {
                for (int j = 0; j < this->height; j++)
                {
                    auto d = *p++;
                    this->pixels[off[j] + i] = d < this->palette.size() ? this->palette[d] : COLOR_FAR;
                }
            }
        }
        else
        {
            for (int i = 0; i < this->width; i++)
            {
                for (int j = this->height - 1; j >= 0; j--)
                {
                    auto d = *p++;
                    this->pixels[off[j] + i] = d < this->palette.size() ? this->palette[d] : COLOR_FAR;
                }
            }
        }
    }
}

void DepthView::DrawGround(const DepthFrame& frame, Orientation o)
{
    if (Orientation::Landscape == o)
    {
        this->width  = frame.Width;
        this->height = frame.Height;
    }
    else
    {
        this->width  = frame.Height;
        this->height = frame.Width;
    }

    lock_guard<std::mutex> lock(this->mutex);

    vector<Vector3> cloud;
    if (Error::Ok == frame.ToPointCloud(cloud))
    {
        this->pixels.resize(this->width * this->height);

        if (Orientation::Landscape == o)
        {
            for (size_t i = 0; i < this->pixels.size(); i++)
            {
                auto d = frame.Pixels[i];

                if (d < this->palette.size())
                {
                    this->pixels[i] = IsGround(cloud[i], this->ground) ? COLOR_GND : this->palette[d];
                }
                else
                {
                    this->pixels[i] = COLOR_FAR;
                }
            }
        }
        else
        {
            auto p = frame.Pixels;

            vector<int> off(this->height);
            for (int i = 0; i < this->height; i++)
            {
                off[i] = i * this->width;
            }

            if (Orientation::PortraitClockwise == o)
            {
                int idx = 0;

                for (int i = this->width - 1; i >= 0; i--)
                {
                    for (int j = 0; j < this->height; j++)
                    {
                        auto d = frame.Pixels[idx];
                        if (d < this->palette.size())
                        {
                            this->pixels[off[j] + i] = IsGround(cloud[idx], this->ground) ? COLOR_GND : this->palette[d];
                        }
                        else
                        {
                            this->pixels[off[j] + i] = COLOR_FAR;
                        }

                        idx++;
                    }
                }
            }
            else
            {
                int idx = 0;

                for (int i = 0; i < this->width; i++)
                {
                    for (int j = this->height - 1; j >= 0; j--)
                    {
                        auto d = frame.Pixels[idx];
                        if (d < this->palette.size())
                        {
                            this->pixels[off[j] + i] = IsGround(cloud[idx], this->ground) ? COLOR_GND : this->palette[d];
                        }
                        else
                        {
                            this->pixels[off[j] + i] = COLOR_FAR;
                        }

                        idx++;
                    }
                }
            }
        }
    }
}