#include "ColorView.h"
#include "GDIRenderer.h"

using namespace std;
using namespace Topsens;

void ColorView::Draw(const ColorFrame& frame, Orientation orientation)
{
    if (Orientation::Landscape == orientation)
    {
        this->width  = frame.Width;
        this->height = frame.Height;
    }
    else
    {
        this->height = frame.Width;
        this->width  = frame.Height;
    }

    lock_guard<std::mutex> lock(this->mutex);

    this->pixels.resize(this->width * this->height);

    if (Orientation::Landscape == orientation)
    {
        memcpy(&this->pixels[0], frame.Pixels, this->pixels.size() * sizeof(this->pixels[0]));
    }
    else
    {
        auto p = frame.Pixels;

        vector<int> off(this->height);
        for (int i = 0; i < this->height; i++)
        {
            off[i] = i * this->width;
        }

        if (Orientation::PortraitClockwise == orientation)
        {
            for (int i = this->width - 1; i >= 0; i--)
            {
                for (int j = 0; j < this->height; j++)
                {
                    this->pixels[off[j] + i] = *p++;
                }
            }
        }
        else
        {
            for (int i = 0; i < this->width; i++)
            {
                for (int j = this->height - 1; j >= 0; j--)
                {
                    this->pixels[off[j] + i] = *p++;
                }
            }
        }
    }
}

void ColorView::Error(const wchar_t* error)
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
}

void ColorView::OnPaint()
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