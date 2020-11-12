#include "StatusBar.h"
#include <vector>

using namespace std;

StatusBar::StatusBar(UINT id, HINSTANCE instance) : View(instance), id(id)
{
    this->style = CCS_BOTTOM | WS_CHILD;
}

StatusBar::operator bool() const
{
    return this->hwnd ? true : false;
}

bool StatusBar::Create(View* parent)
{
    if (!parent)
    {
        return false;
    }

    if (this->hwnd)
    {
        this->Destroy();
    }

    this->owner  = parent;
    this->parent = parent;

    this->hwnd = CreateWindowExW(0, STATUSCLASSNAMEW, nullptr, this->style, 0, 0, 0, 0, parent->Handle(), (HMENU)this->id, this->instance, nullptr);
    if (this->hwnd)
    {
        SetWindowLongPtrW(this->hwnd, GWLP_USERDATA, (LONG_PTR)this);
        this->defaultProc = (WNDPROC)SetWindowLongPtrW(this->hwnd, GWLP_WNDPROC, (LONG_PTR)StatusBar::MessageRouter);

        if (!this->OnCreated())
        {
            this->Destroy();
        }
    }
    else
    {
        this->owner  = nullptr;
        this->parent = nullptr;
    }

    return this->hwnd ? true : false;
}

bool StatusBar::Create(View* parent, bool sizeGrip)
{
    if (sizeGrip)
    {
        this->Style(this->Style() | SBARS_SIZEGRIP);
    }
    else
    {
        this->Style(this->Style() &~ SBARS_SIZEGRIP);
    }

    return this->Create(parent);
}

void StatusBar::AutoResize()
{
    SendMessageW(this->hwnd, WM_SIZE, 0, 0);
}

bool StatusBar::SetParts(UINT parts, int* positions) const
{
    if (parts > 256)
    {
        return false;
    }

    if (!positions)
    {
        return false;
    }

    return SendMessageW(this->hwnd, SB_SETPARTS, parts, (LPARAM)positions) ? true : false;
}

UINT StatusBar::GetParts() const
{
    return (UINT)SendMessageW(this->hwnd, SB_GETPARTS, 0, 0);
}

UINT StatusBar::GetParts(std::vector<int>& positions) const
{
    positions.resize(this->GetParts());
    return (UINT)SendMessageW(this->hwnd, SB_GETPARTS, (WPARAM)positions.size(), (LPARAM)&positions[0]);
}

bool StatusBar::Text(const wchar_t* text, UINT part) const
{
    if (part > 256)
    {
        return false;
    }

    return SendMessageW(this->hwnd, SB_SETTEXTW, part, (LPARAM)text) ? true : false;
}

bool StatusBar::Text(const wstring& text, UINT part) const
{
    return this->Text(text.c_str(), part);
}

wstring StatusBar::Text(UINT part) const
{
    wstring text;

    if (part > 256)
    {
        return text;
    }

    auto l = SendMessageW(this->hwnd, SB_GETTEXTLENGTH, part, 0) & 0xFF;
    if (l)
    {
        text.resize(l);
        SendMessageW(this->hwnd, SB_GETTEXTW, part, (LPARAM)&text[0]);
    }

    return text;
}

LRESULT StatusBar::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
        {
            return View::WindowProc(hWnd, uMsg, wParam, lParam);
        }

        default:
            return this->DefaultProc(hWnd, uMsg, wParam, lParam);
    }
}

LRESULT StatusBar::DefaultProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return this->defaultProc(hWnd, uMsg, wParam, lParam);
}

bool StatusBar::OnCreated()
{
    if (!View::OnCreated())
    {
        return false;
    }

    this->AutoResize();
    return true;
}