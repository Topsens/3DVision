#include "ColorView.h"
#include "GDIRenderer.h"

using namespace std;
using namespace Topsens;

void ColorView::Draw(const ColorFrame& frame)
{
    this->width  = frame.Width;
    this->height = frame.Height;

    auto len = this->width * this->height;

    lock_guard<std::mutex> lock(this->mutex);

    this->pixels.resize(len * 4);
    memcpy(&this->pixels[0], frame.Pixels, this->pixels.size());

    this->Invalidate();
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

    this->Invalidate();
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