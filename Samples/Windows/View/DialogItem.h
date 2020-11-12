#pragma once

#include <Windows.h>
#include <string>

class DialogItem
{
public:
    DialogItem();
    DialogItem(HWND hWnd);
    DialogItem(HWND parent, int id);
    DialogItem(HWND parent, HWND hWnd, int id);

    void Destroy();

    DWORD Style() const;
    DWORD StyleEx() const;
    void  Style(DWORD style);
    void  StyleEx(DWORD style);

    void Show() const;
    void Hide() const;
    bool MoveTo(int x, int y, bool repaint = false) const;
    bool Resize(int width, int height, bool repaint = true) const;

    void Enable() const;
    void Disable() const;
    bool IsEnabled() const;
    bool IsVisible() const;
    bool HasFocus() const;
    HWND SetFocus() const;

    void NextTabStop() const;

    LRESULT Send(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const;
    bool    Post(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const;

    int   X() const;
    int   Y() const;
    int   Width() const;
    int   Height() const;
    RECT  Rect() const;

    void Font(HFONT font) const;
    void Text(const wchar_t* text) const;
    void Text(const std::wstring& text) const;
    std::wstring Text() const;

    int  ID() const;
    HWND Handle() const;

    operator bool() const;

    static HFONT CreateFont(const wchar_t* family, int size, int weight = FW_DONTCARE, bool italic = false, bool underline = false, bool strikeOut = false, DWORD charSet = DEFAULT_CHARSET, DWORD outPrecision = OUT_DEFAULT_PRECIS, DWORD clipPrecision = CLIP_DEFAULT_PRECIS, DWORD quality = DEFAULT_QUALITY, DWORD pitchAndFamity = DEFAULT_PITCH | FF_DONTCARE, int escapement = 0, int orientation = 0);
    static void  DestroyFont(HFONT font);

protected:
    HWND parent;
    HWND hwnd;
    int  id;
};