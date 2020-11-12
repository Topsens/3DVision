#include "GDIRenderer.h"
#include "Cleanup.h"

HPEN   NullPen   = (HPEN)GetStockObject(NULL_PEN);
HBRUSH NullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);

GDIRenderer::GDIRenderer()
  : hdc(nullptr), mem(nullptr), bmp(nullptr), old(nullptr), hWnd(nullptr), font(nullptr), pen(NullPen), brush(NullBrush)
{
}

GDIRenderer::~GDIRenderer()
{
    if (this->pen && this->pen != NullPen)
    {
        DeleteObject(this->pen);
    }

    if (this->brush && this->brush != NullBrush)
    {
        DeleteObject(this->brush);
    }

    if (this->font)
    {
        DeleteObject(this->font);
    }
}

bool GDIRenderer::BeginPaint(HWND hwnd, bool copyBackground)
{
    if (!hwnd)
    {
        return false;
    }

    this->hWnd = hwnd;

    this->hdc = ::BeginPaint(hwnd, &this->ps);
    ONCLEANUP(hdc, [this]{ if (this->hdc) ::EndPaint(this->hWnd, &this->ps); this->hdc = nullptr; });

    if (!this->hdc)
    {
        return false;
    }

    this->mem = CreateCompatibleDC(this->hdc);
    ONCLEANUP(mem, [this]{ if (this->mem) DeleteDC(this->mem); this->mem = nullptr; });

    if (!this->mem)
    {
        return false;
    }

    RECT client;
    GetClientRect(hwnd, &client);

    int cx = client.right - client.left;
    int cy = client.bottom - client.top;

    this->bmp = CreateCompatibleBitmap(this->hdc, cx, cy);
    ONCLEANUP(bmp, [this]{ if (this->bmp) DeleteObject(this->bmp); this->bmp = nullptr; });

    if (!this->bmp)
    {
        return false;
    }

    this->old = SelectObject(this->mem, this->bmp);
    ONCLEANUP(old, [this]{ if (this->old) DeleteObject(this->old); this->old = nullptr; });

    if (!this->old)
    {
        return false;
    }

    if (copyBackground)
    {
        auto x = this->ps.rcPaint.left;
        auto y = this->ps.rcPaint.top;
        auto w = this->ps.rcPaint.right - x;
        auto h = this->ps.rcPaint.bottom - y;

        BitBlt(this->mem, x, y, w, h, this->hdc, x, y, SRCCOPY);
    }

    RESETCLEANUP(old, nullptr);
    RESETCLEANUP(bmp, nullptr);
    RESETCLEANUP(mem, nullptr);
    RESETCLEANUP(hdc, nullptr);

    return true;
}

void GDIRenderer::EndPaint()
{
    if (this->hdc)
    {
        auto x = this->ps.rcPaint.left;
        auto y = this->ps.rcPaint.top;
        auto w = this->ps.rcPaint.right - x;
        auto h = this->ps.rcPaint.bottom - y;

        BitBlt(this->hdc, x, y, w, h, this->mem, x, y, SRCCOPY);
        ::EndPaint(this->hWnd, &this->ps);

        if (this->old)
        {
            SelectObject(this->mem, this->old);
        }

        DeleteObject(this->bmp);
        DeleteDC(this->mem);

        this->hdc = nullptr;
        this->mem = nullptr;
        this->bmp = nullptr;
        this->hWnd = nullptr;
    }
}

bool GDIRenderer::Pen(HPEN pen)
{
    if (pen)
    {
        LOGPEN logPen;
        if (!GetObjectW(pen, sizeof(logPen), &logPen))
        {
            return false;
        }

        if (this->pen && this->pen != NullPen)
        {
            DeleteObject(this->pen);
        }

        return nullptr != (this->pen = CreatePenIndirect(&logPen));
    }
    else
    {
        if (this->pen && this->pen != NullPen)
        {
            DeleteObject(this->pen);
        }

        return nullptr != (this->pen = NullPen);
    }
}

bool GDIRenderer::Pen(int style, int width, COLORREF color)
{
    if (this->hdc)
    {
        if (this->pen && this->pen != NullPen)
        {
            DeleteObject(this->pen);
        }

        this->pen = CreatePen(style, width, color);

        return nullptr != this->pen;
    }

    return false;
}

bool GDIRenderer::Brush(HBRUSH brush)
{
    if (brush)
    {
        LOGBRUSH logBrush;
        if (!GetObjectW(brush, sizeof(logBrush), &logBrush))
        {
            return false;
        }

        if (this->brush && this->brush != NullBrush)
        {
            DeleteObject(this->brush);
        }

        return nullptr != (this->brush = CreateBrushIndirect(&logBrush));
    }
    else
    {
        if (this->brush && this->brush != NullBrush)
        {
            DeleteObject(this->brush);
        }

        return nullptr != (this->brush = NullBrush);
    }
}

bool GDIRenderer::SolidBrush(COLORREF color)
{
    if (this->hdc)
    {
        if (this->brush && this->brush != NullBrush)
        {
            DeleteObject(this->brush);
        }

        this->brush = CreateSolidBrush(color);

        return nullptr != this->brush;
    }

    return false;
}

bool GDIRenderer::Pixel(int x, int y, COLORREF color)
{
    if (!this->hdc)
    {
        return false;
    }

    return -1 != SetPixel(this->mem, x, y, color);
}

bool GDIRenderer::PixelsRGB(const BYTE* pixels, int width, int height, int x, int y, int cx, int cy)
{
    if (!pixels || width <= 0 || height <= 0 || cx <= 0 || cy <= 0)
    {
        return false;
    }

    if (!this->hdc)
    {
        return false;
    }

    auto hdc = CreateCompatibleDC(this->hdc);
    ONCLEANUP(hdc, [&]{ if (hdc) DeleteDC(hdc); });

    if (!hdc)
    {
        return false;
    }

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0; // ignored for BI_RGB
    bmi.bmiHeader.biXPelsPerMeter = 3780;
    bmi.bmiHeader.biYPelsPerMeter = 3780;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;

    auto bmp = CreateDIBitmap(this->hdc, &bmi.bmiHeader, CBM_INIT, pixels, &bmi, DIB_RGB_COLORS);
    ONCLEANUP(bmp, [&]{ if (bmp) DeleteObject(bmp); });

    if (!bmp)
    {
        return false;
    }

    auto old = SelectObject(hdc, bmp);
    ONCLEANUP(old, [&]{ if (old) SelectObject(hdc, old); });

    if (!old)
    {
        return false;
    }

    SetStretchBltMode(hdc, COLORONCOLOR);
    return StretchBlt(this->mem, x, y, cx, cy, hdc, 0, 0, width, height, SRCCOPY) ? true : false;
}

bool GDIRenderer::PixelsARGB(const BYTE* pixels, int width, int height, int x, int y, int cx, int cy, bool useAlpha)
{
    if (!pixels || width <= 0 || height <= 0 || cx <= 0 || cy <= 0)
    {
        return false;
    }

    if (!this->hdc)
    {
        return false;
    }

    auto hdc = CreateCompatibleDC(this->hdc);
    ONCLEANUP(hdc, [&]{ if (hdc) DeleteDC(hdc); });

    if (!hdc)
    {
        return false;
    }

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0; // ignored for BI_RGB
    bmi.bmiHeader.biXPelsPerMeter = 3780;
    bmi.bmiHeader.biYPelsPerMeter = 3780;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;

    HBITMAP bmp = nullptr;
    ONCLEANUP(bmp, [&]{ if (bmp) DeleteObject(bmp); });

    if (useAlpha)
    {
        void* bits;
        bmp = CreateDIBSection(this->hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);

        if (bmp)
        {
            for (int i = 0; i < width * height; i++)
            {
                auto bgra = &pixels[i * 4];

                if (bgra[3] != 255)
                {
                    auto a =  bgra[3] + 1;
                    auto b = (bgra[0] * a) >> 8; // approximate to (color = color * alpha / 255) but probably faster
                    auto g = (bgra[1] * a) >> 8 << 8;
                    auto r = (bgra[2] * a) >> 8 << 16;
                    ((int*)bits)[i] = b | g | r | (bgra[3] << 24);
                }
                else
                {
                    ((int*)bits)[i] = *(int*)bgra;
                }
            }
        }
    }
    else
    {
        bmp = CreateDIBitmap(this->hdc, &bmi.bmiHeader, CBM_INIT, pixels, &bmi, DIB_RGB_COLORS);
    }
    
    if (!bmp)
    {
        return false;
    }

    auto old = SelectObject(hdc, bmp);
    ONCLEANUP(old, [&]{ if (old) SelectObject(hdc, old); });

    if (!old)
    {
        return false;
    }

    if (useAlpha)
    {
        BLENDFUNCTION bf = {0};
        bf.BlendOp = AC_SRC_OVER;
        bf.AlphaFormat = AC_SRC_ALPHA;
        bf.SourceConstantAlpha = 255;

        if (!AlphaBlend(this->mem, x, y, cx, cy, hdc, 0, 0, width, height, bf))
        {
            return false;
        }
    }
    else
    {
        SetStretchBltMode(this->mem, COLORONCOLOR);
        if (!StretchBlt(this->mem, x, y, cx, cy, hdc, 0, 0, width, height, SRCCOPY))
        {
            return false;
        }
    }

    return true;
}

void GDIRenderer::From(int x, int y)
{
    this->fromX = x;
    this->fromY = y;
}

bool GDIRenderer::LineTo(int x, int y)
{
    if (!this->hdc)
    {
        return false;
    }

    HGDIOBJ old = nullptr;
    ONCLEANUP(old, [&]{ if (old) SelectObject(this->mem, this->pen); });

    if (this->pen)
    {
        old = SelectObject(this->mem, this->pen);
        if (!old)
        {
            return false;
        }
    }

    MoveToEx(this->mem, this->fromX, this->fromY, nullptr);
    this->fromX = x;
    this->fromY = y;

    return ::LineTo(this->mem, x, y) ? true : false;
}

bool GDIRenderer::Rectangle(int left, int top, int right, int bottom)
{
    if (!this->hdc)
    {
        return false;
    }

    HGDIOBJ pobj = nullptr;
    if (this->pen && !(pobj = SelectObject(this->mem, this->pen)))
    {
        return false;
    }

    HGDIOBJ bobj = nullptr;
    if (this->brush && !(bobj = SelectObject(this->mem, this->brush)))
    {
        if (pobj)
        {
            SelectObject(this->mem, pobj);
        }

        return false;
    }

    auto ret = ::Rectangle(this->mem, left, top, right, bottom);

    if (bobj)
    {
        SelectObject(this->mem, bobj);
    }

    if (pobj)
    {
        SelectObject(this->mem, pobj);
    }

    return FALSE != ret;
}

bool GDIRenderer::Rectangle(int left, int top, int right, int bottom, BYTE opacity)
{
    auto w = right - left;
    auto h = bottom - top;

    auto hdc = CreateDC(w, h);
    ONCLEANUP(hdc, [&]{ DestroyDC(hdc); });

    if (!hdc)
    {
        return false;
    }

    auto oldp = SelectObject(hdc, this->pen);
    ONCLEANUP(oldp, [&]{ if (oldp) SelectObject(hdc, oldp); });

    if (!oldp)
    {
        return false;
    }

    auto oldb = SelectObject(hdc, this->brush);
    ONCLEANUP(oldb, [&]{ if (oldb) SelectObject(hdc, oldb); });

    if (!oldb)
    {
        return false;
    }

    if (!::Rectangle(hdc, 0, 0, w, h))
    {
        return false;
    }
    
    BLENDFUNCTION bf = {0};
    bf.BlendOp = AC_SRC_OVER;
    bf.AlphaFormat = 0;
    bf.SourceConstantAlpha = opacity;

    if (!AlphaBlend(this->mem, left, top, w, h, hdc, 0, 0, w, h, bf))
    {
        return false;
    }

    return true;
}

bool GDIRenderer::Ellipse(int left, int top, int right, int bottom)
{
    if (!this->hdc || !this->pen)
    {
        return false;
    }

    HGDIOBJ pobj = nullptr;
    if (this->pen && !(pobj = SelectObject(this->mem, this->pen)))
    {
        return false;
    }

    HGDIOBJ bobj = nullptr;
    if (this->brush && !(bobj = SelectObject(this->mem, this->brush)))
    {
        if (pobj)
        {
            SelectObject(this->mem, pobj);
        }

        return false;
    }

    auto ret = ::Ellipse(this->mem, left, top, right, bottom);

    if (bobj)
    {
        SelectObject(this->mem, bobj);
    }

    if (pobj)
    {
        SelectObject(this->mem, pobj);
    }

    return FALSE != ret;
}

bool GDIRenderer::Ellipse(int left, int top, int right, int bottom, BYTE opacity)
{
    auto w = right - left;
    auto h = bottom - top;

    auto hdc = CreateDC(w, h, this->mem, left, top);
    ONCLEANUP(hdc, [&]{ DestroyDC(hdc); });

    if (!hdc)
    {
        return false;
    }

    auto oldp = SelectObject(hdc, this->pen);
    ONCLEANUP(oldp, [&]{ if (oldp) SelectObject(hdc, oldp); });

    if (!oldp)
    {
        return false;
    }

    auto oldb = SelectObject(hdc, this->brush);
    ONCLEANUP(oldb, [&]{ if (oldb) SelectObject(hdc, oldb); });

    if (!oldb)
    {
        return false;
    }

    if (!::Ellipse(hdc, 0, 0, w, h))
    {
        return false;
    }
    
    BLENDFUNCTION bf = {0};
    bf.BlendOp = AC_SRC_OVER;
    bf.AlphaFormat = 0;
    bf.SourceConstantAlpha = opacity;

    if (!AlphaBlend(this->mem, left, top, w, h, hdc, 0, 0, w, h, bf))
    {
        return false;
    }

    return true;
}

bool GDIRenderer::Font(const wchar_t* family, int size, int weight, bool italic, bool underline, bool strikeOut, DWORD charSet, DWORD outPrecision, DWORD clipPrecision, DWORD quality, DWORD pitchAndFamity, int escapement, int orientation)
{
    if (this->hdc)
    {
        if (this->font)
        {
            DeleteObject(this->font);
        }

        this->font = CreateFontW(size, 0, escapement, orientation, weight, italic ? TRUE : FALSE, underline ? TRUE : FALSE, strikeOut ? TRUE : FALSE, charSet, outPrecision, clipPrecision, quality, pitchAndFamity, family);
        return nullptr != this->font;
    }

    return false;
}

bool GDIRenderer::Text(const wchar_t* text, int x, int y, int w, int h, UINT align, COLORREF color, BYTE opacity)
{
    if (!this->hdc)
    {
        return false;
    }

    auto hdc = 255 == opacity ? this->mem : CreateDC(w, h, this->mem, x, y);
    ONCLEANUP(hdc, [&]{ if (hdc != this->mem) DestroyDC(hdc); });

    if (!hdc)
    {
        return false;
    }

    HGDIOBJ old = nullptr;
    ONCLEANUP(old, [&]{ if(old) SelectObject(hdc, old); });

    if (this->font && !(old = SelectObject(hdc, this->font)))
    {
        return false;
    }

    SetBkMode(hdc, TRANSPARENT);
    SetBkColor(hdc, RGB(0, 0, 0));
    SetTextColor(hdc, color);

    RECT rect = 255 == opacity ? RECT{ x, y, x + w, y + h } : RECT{ 0, 0, w, h };
    if (!DrawTextW(hdc, text, -1, &rect, align))
    {
        return false;
    }
    
    if (255 != opacity)
    {
        BLENDFUNCTION bf = {0};
        bf.BlendOp = AC_SRC_OVER;
        bf.AlphaFormat = 0;
        bf.SourceConstantAlpha = opacity;

        if (!AlphaBlend(this->mem, x, y, w, h, hdc, 0, 0, w, h, bf))
        {
            return false;
        }
    }

    return true;
}

bool GDIRenderer::Clear(COLORREF color)
{
    if (this->hdc)
    {
        RECT client;
        if (GetClientRect(this->hWnd, &client) && this->SolidBrush(color) && this->Pen(PS_SOLID, 1, color))
        {
            return this->Rectangle(client.left, client.top, client.right, client.bottom);
        }
    }

    return false;
}

HDC GDIRenderer::CreateDC(int width, int height, HDC src, int x, int y)
{
    if (!this->hdc || width <= 0 || height <= 0 || x < 0 || y < 0)
    {
        return nullptr;
    }

    HDC hdc = CreateCompatibleDC(this->hdc);
    if (hdc)
    {
        BITMAPINFO bmi = {0};
        bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 24;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = 0; // ignored for BI_RGB
        bmi.bmiHeader.biXPelsPerMeter = 3780;
        bmi.bmiHeader.biYPelsPerMeter = 3780;
        bmi.bmiHeader.biClrUsed = 0;
        bmi.bmiHeader.biClrImportant = 0;

        auto bmp = CreateDIBitmap(this->hdc, &bmi.bmiHeader, CBM_INIT, nullptr, &bmi, DIB_RGB_COLORS);
        if (bmp)
        {
            DeleteObject(SelectObject(hdc, bmp));
        }
        else
        {
            DeleteDC(hdc);
            hdc = nullptr;
        }
    }

    if (hdc)
    {
        BitBlt(hdc, 0, 0, width, height, src, x, y, SRCCOPY);
    }
    else
    {
        DestroyDC(hdc);
        hdc = nullptr;
    }

    return hdc;
}

void GDIRenderer::DestroyDC(HDC hdc)
{
    if (hdc)
    {
        DeleteObject(GetCurrentObject(hdc, OBJ_BITMAP));
        DeleteDC(hdc);
    }
}