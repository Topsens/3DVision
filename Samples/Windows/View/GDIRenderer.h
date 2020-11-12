#pragma once
#pragma comment(lib, "Msimg32.lib")

#include <Windows.h>

class GDIRenderer
{
public:
    GDIRenderer();
   ~GDIRenderer();

    bool BeginPaint(HWND hwnd, bool copyBackground = false);
    void EndPaint();

    bool Pen(HPEN pen);
    bool Pen(int style, int width, COLORREF color);
    bool Brush(HBRUSH brush);
    bool SolidBrush(COLORREF color);

    bool Pixel(int x, int y, COLORREF color);
    bool PixelsRGB(const BYTE* pixels, int width, int height, int x, int y, int cx, int cy);
    bool PixelsARGB(const BYTE* pixels, int width, int height, int x, int y, int cx, int cy, bool useAlpha = false);

    void From(int x, int y);
    bool LineTo(int x, int y);

    bool Rectangle(int left, int top, int right, int bottom);
    bool Rectangle(int left, int top, int right, int bottom, BYTE opacity);
    bool Ellipse(int left, int top, int right, int bottom);
    bool Ellipse(int left, int top, int right, int bottom, BYTE opacity);

    bool Font(const wchar_t* family, int size, int weight = FW_NORMAL, bool italic = false, bool underline = false, bool strikeOut = false, DWORD charSet = DEFAULT_CHARSET, DWORD outPrecision = OUT_DEFAULT_PRECIS, DWORD clipPrecision = CLIP_DEFAULT_PRECIS, DWORD quality = DEFAULT_QUALITY, DWORD pitchAndFamity = DEFAULT_PITCH | FF_DONTCARE, int escapement = 0, int orientation = 0);
    bool Text(const wchar_t* text, int x, int y, int w, int h, UINT align = DT_LEFT | DT_TOP, COLORREF color = RGB(255, 255, 255), BYTE opacity = 255);

    bool Clear(COLORREF color = RGB(0, 0, 0));

private:
    HDC  CreateDC(int width, int height, HDC src = nullptr, int x = 0, int y = 0);
    void DestroyDC(HDC hdc);

private:
    HDC     hdc;
    HDC     mem;
    HBITMAP bmp;

    HPEN    pen;
    HBRUSH  brush;
    HFONT   font;

    HWND hWnd;
    PAINTSTRUCT ps;

    HGDIOBJ old;

    int fromX, fromY;
};