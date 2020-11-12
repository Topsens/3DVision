#include "DepthView.h"

#define COLOR_FAR       (0xFFFF0000)
#define COLOR_GROUND    (0xFF808080)

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
}

void DepthView::Draw(const uint16_t* depth, uint32_t w, uint32_t h)
{
    this->width  = w;
    this->height = h;

    lock_guard<std::mutex> lock(this->mutex);
    this->pixels.resize(this->width * this->height);

    for (size_t i = 0; i < w * h; i++)
    {
        auto d = depth[i];

        if (d < this->palette.size())
        {
            this->pixels[i] = this->palette[d];
        }
        else
        {
            this->pixels[i] = COLOR_FAR;
        }
    }
}

void DepthView::Draw(const UsersFrame& frame)
{
    lock_guard<std::mutex> lock(this->mutex);
    this->users.Draw(frame);
}

void DepthView::Error(const wstring& error)
{
    lock_guard<std::mutex> lock(this->mutex);
    this->error = error;
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
            renderer.PixelsARGB((BYTE*)this->pixels.data(), this->width, this->height, (this->ClientWidth() - this->width) / 2, 0, this->width, this->height);
        }

        this->users.Paint(renderer, (this->ClientWidth() - this->width) / 2, 0, this->ClientHeight() / (float)this->height);

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