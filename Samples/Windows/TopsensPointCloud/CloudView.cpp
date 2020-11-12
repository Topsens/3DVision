#include "CloudView.h"
#include <cassert>

using namespace std;

void CloudView::ApplyColor(const Topsens::ColorFrame& frame)
{
    lock_guard<mutex> lock(this->lock);
    this->cw = frame.Width;
    this->ch = frame.Height;
    this->color.resize(frame.Width * frame.Height);
    memcpy(&this->color[0], frame.Pixels, this->color.size() * sizeof(this->color[0]));
}

void CloudView::Draw(const Topsens::DepthFrame& frame, bool mapColor)
{
    lock_guard<mutex> lock(this->lock);

    Vertex wc;
    this->GetVertices(frame, wc);

    if (mapColor)
    {
        this->GetCoordinates(frame);
    }
    else
    {
        this->coords.clear();
    }

    this->scene.Camera().LookAt(wc);
    this->cloud.Position = wc;
    this->Invalidate();
}

void CloudView::Clear()
{
    this->AttachContext();
    this->cloud.Texture().Release();
    this->cloud.Release();
    this->DetachContext();
}

LRESULT CloudView::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_MOUSEMOVE:
        {
            if (MK_LBUTTON & wParam)
            {
                POINT pos;
                if (GetCursorPos(&pos))
                {
                    auto x = pos.x - this->cursor.x;
                    auto y = this->cursor.y - pos.y;
                    this->cursor = pos;

                    auto qx = Quaternion<float>::FromAxisAngle(Vertex::XAxis, ToRadian(y * .2f));
                    auto qy = Quaternion<float>::FromAxisAngle(Vertex::YAxis, ToRadian(x * .2f));
                    auto qr = Quaternion<float>::FromRotation(this->cloud.Rotation);

                    this->cloud.Rotation = (qx * qy * qr).ToRotation();
                }
            }
            break;
        }

        case WM_LBUTTONDOWN:
        {
            SetCapture(this->Handle());
            GetCursorPos(&this->cursor);
            break;
        }

        case WM_LBUTTONUP:
        {
            ReleaseCapture();
            break;
        }

        case WM_LBUTTONDBLCLK:
        {
            this->cloud.Rotation = { 0.f, 0.f, 0.f, 0.f };
        }

        case WM_MOUSEWHEEL:
        {
            auto delta = 1.f + (float)((short)HIWORD(wParam) / WHEEL_DELTA) * 0.05f;

            auto position = this->scene.Camera().Position();
            auto lookAt   = this->scene.Camera().LookAt();
            this->scene.Camera().Position((position - lookAt) * delta + lookAt);

            break;
        }
    }

    return GLWindow::WindowProc(hWnd, uMsg, wParam, lParam);
}

bool CloudView::OnCreated()
{
    if (!GLWindow::OnCreated())
    {
        return false;
    }

    this->scene.Camera().Clip(0.01f, 100.f);
    return true;
}

void CloudView::OnDestroy()
{
    this->Clear();
}

void CloudView::OnPaint()
{
    this->AttachContext();

    lock_guard<mutex> lock(this->lock);

    if (this->vertices.size())
    {
        this->cloud.Vertices(this->vertices.data(), (int)this->vertices.size());

        if (this->color.size() && this->coords.size())
        {
            this->cloud.TexCoords(this->coords.data(), (int)this->coords.size());
            this->cloud.Texture().Set((uint8_t*)this->color.data(), this->cw, this->ch, (int)(this->color.size() * sizeof(this->color[0])), GL_BGRA);
        }
        else
        {
            this->cloud.ClearTexCoords();
            this->cloud.Texture().Clear();
        }
    }
    else
    {
        this->cloud.ClearVertices();
        this->cloud.ClearTexCoords();
        this->cloud.Texture().Clear();
    }

    this->scene.Begin(this->ClientWidth(), this->ClientHeight());
    this->cloud.Render();
    this->scene.End();

    this->DetachContext();

    GLWindow::OnPaint();
}

void CloudView::GetVertices(const Topsens::DepthFrame& frame, Vertex& weightCenter)
{
    vector<Vertex> vertices;
    if (Topsens::Error::Ok == frame.ToPointCloud((vector<Topsens::Vector3>&)vertices, true) && vertices.size())
    {
        auto avg = vertices;

        while (avg.size() > 1)
        {
            for (size_t i = 0; i < avg.size() / 2; i++)
            {
                auto k = i * 2;
                avg[i] = avg[k] + avg[k + 1];
            }

            if (avg.size() & 1)
            {
                avg[0] += avg[avg.size() - 1];
            }

            avg.resize(avg.size() / 2);
        }

        avg[0] /= (float)vertices.size();
        avg[0][0] = -avg[0][0];
        weightCenter = avg[0];

        for (auto& v : vertices)
        {
            v[0] = -v[0];
            v -= avg[0];
        }

        this->vertices.swap(vertices);
    }
}

void CloudView::GetCoordinates(const Topsens::DepthFrame& frame)
{
    if (this->coordTable.size() != frame.Width * frame.Height)
    {
        this->coordTable.clear();
        this->coordTable.reserve(frame.Width * frame.Height);

        vector<float> hcoords(frame.Width);
        for (uint32_t i = 0; i < frame.Width; i++)
        {
            hcoords[i] = (float)i / frame.Width;
        }

        Coordinate c;
        for (uint32_t i = 0; i < frame.Height; i++)
        {
            c[1] = (float)i / frame.Height;

            for (uint32_t j = 0; j < frame.Width; j++)
            {
                c[0] = hcoords[j];
                this->coordTable.push_back(c);
            }
        }
    }

    this->coords.clear();
    this->coords.reserve(frame.Width * frame.Height);

    for (uint32_t i = 0, index = 0; i < frame.Height; i++)
    {
        for (uint32_t j = 0; j < frame.Width; j++, index++)
        {
            if (frame.Pixels[index])
            {
                this->coords.push_back(this->coordTable[index]);
            }
        }
    }
}