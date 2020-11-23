#include "View.h"
#include <vector>

using namespace std;

View::View(HINSTANCE instance)
  : hwnd(nullptr), owner(nullptr), parent(nullptr), style(0), styleEx(0), instance(instance), cursor(nullptr)
{
    this->c2scr = {0};
    this->wrect = {0};
    this->crect = {0};
    this->min   = {0};
    this->max   = {0};
}

View::~View()
{
}

WPARAM View::MessageLoop(HWND hwnd, HACCEL haccel)
{
    MSG  msg = {0};
    BOOL ret;

    while (0 != (ret = GetMessageW(&msg, nullptr, 0, 0)))
    {
        if (-1 == ret)
        {
            break;
        }

        if (hwnd)
        {
            if (haccel && TranslateAcceleratorW(hwnd, haccel, &msg))
            {
                continue;
            }

            if (IsDialogMessageW(hwnd, &msg))
            {
                continue;
            }
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return msg.wParam;
}

LRESULT View::MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    View* pThis = nullptr;

    if (WM_NCCREATE == uMsg)
    {
        pThis = reinterpret_cast<View*>(((LPCREATESTRUCTW)lParam)->lpCreateParams);
        pThis->hwnd = hWnd;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
    }
    else if (WM_INITDIALOG == uMsg)
    {
        pThis = reinterpret_cast<View*>(lParam);
        pThis->hwnd = hWnd;
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, lParam);
    }
    else
    {
        pThis = reinterpret_cast<View*>(::GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    }

    if (pThis)
    {
        pThis->umsg   = uMsg;
        pThis->wparam = wParam;
        pThis->lparam = lParam;

        return (pThis->MessageHandler(hWnd, uMsg, wParam, lParam));
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT View::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        {
            auto res = this->WindowProc(hWnd, uMsg, wParam, lParam);
            this->c2scr  = {0};
            this->wrect  = {0};
            this->crect  = {0};
            this->hwnd   = nullptr;
            this->owner  = nullptr;
            this->parent = nullptr;
            return res;
        }

        case WM_MOVE:
        {
            this->c2scr = {0};
            ClientToScreen(this->hwnd, &this->c2scr);
            GetWindowRect(this->hwnd, &this->wrect);

            if (this->parent)
            {
                this->wrect.left   -= this->parent->c2scr.x;
                this->wrect.right  -= this->parent->c2scr.x;
                this->wrect.top    -= this->parent->c2scr.y;
                this->wrect.bottom -= this->parent->c2scr.y;
            }
            break;
        }

        case WM_SIZE:
        {
            this->c2scr = {0};
            ClientToScreen(this->hwnd, &this->c2scr);
            GetWindowRect(this->hwnd, &this->wrect);
            GetClientRect(this->hwnd, &this->crect);

            if (this->parent)
            {
                this->wrect.left   -= this->parent->c2scr.x;
                this->wrect.right  -= this->parent->c2scr.x;
                this->wrect.top    -= this->parent->c2scr.y;
                this->wrect.bottom -= this->parent->c2scr.y;
            }
            break;
        }

        default:
            break;
    }

    return this->WindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT View::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_PAINT:
        {
            this->OnPaint();
            break;
        }

        case WM_TIMER:
        {
            this->OnTimer();
            break;
        }

        case WM_COMMAND:
        {
            this->command = LOWORD(wParam);
            this->OnCommand();
            break;
        }

        case WM_GETMINMAXINFO:
        {
            if (this->min.x || this->min.y)
            {
                ((MINMAXINFO*)lParam)->ptMinTrackSize = this->min;
            }
            if (this->max.x || this->max.y)
            {
                ((MINMAXINFO*)lParam)->ptMaxTrackSize = this->max;
            }
            break;
        }

        case WM_SIZE:
        {
            this->OnSize();
            break;
        }

        case WM_MOVE:
        {
            this->OnMove();
            break;
        }

        case WM_SHOWWINDOW:
        {
            if (wParam)
            {
                this->OnShow();
            }
            else
            {
                this->OnHide();
            }
            break;
        }

        case WM_CLOSE:
        {
            this->OnClose();
            break;
        }

        case WM_DESTROY:
        {
            this->OnDestroy();
            break;
        }

        case WM_SETCURSOR:
        {
            if (this->OnSetCursor())
            {
                return TRUE;
            }
            else
            {
                return this->DefaultProc(hWnd, uMsg, wParam, lParam);
            }
        }

        default:
        {
            auto it = this->messages.find(uMsg);
            if (this->messages.end() != it && !it->second.first)
            {
                it->second.first = true;
                auto result = it->second.second();
                it->second.first = false;
                return result;
            }

            return this->DefaultProc(hWnd, uMsg, wParam, lParam);
        }
    }

    return 0;
}

void View::Destroy()
{
    if (this->hwnd)
    {
        DestroyWindow(this->hwnd);
    }
}

DWORD View::Style() const
{
    return this->style;
}

DWORD View::StyleEx() const
{
    return this->styleEx;
}

void View::Style(DWORD style)
{
    if (this->hwnd)
    {
        SetWindowLongPtrW(this->hwnd, GWL_STYLE, style);
        this->style = (DWORD)GetWindowLongPtrW(this->hwnd, GWL_STYLE);
        SetWindowPos(this->hwnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
    }
    else
    {
        this->style = style;
    }
}

void View::StyleEx(DWORD style)
{
    if (this->hwnd)
    {
        SetWindowLongPtrW(this->hwnd, GWL_EXSTYLE, style);
        this->styleEx = (DWORD)GetWindowLongPtrW(this->hwnd, GWL_EXSTYLE);
        SetWindowPos(this->hwnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
    }
    else
    {
        this->styleEx = style;
    }
}

void View::ClipChildren(bool clip)
{
    if (clip)
    {
        this->Style(this->Style() | WS_CLIPCHILDREN);
    }
    else
    {
        this->Style(this->Style() &~ WS_CLIPCHILDREN);
    }
}

void View::Show() const
{
    if (this->hwnd)
    {
        ShowWindow(this->hwnd, SW_SHOW);
        UpdateWindow(this->hwnd);
    }
}

void View::Hide() const
{
    if (this->hwnd)
    {
        ShowWindow(this->hwnd, SW_HIDE);
    }
}

bool View::Update() const
{
    return FALSE != UpdateWindow(this->hwnd);
}

bool View::Invalidate(bool erase) const
{
    return FALSE != InvalidateRect(this->hwnd, nullptr, erase ? TRUE : FALSE);
}

bool View::Invalidate(const RECT& rect, bool erase) const
{
    return FALSE != InvalidateRect(this->hwnd, &rect,  erase ? TRUE : FALSE);
}

bool View::Move(int x, int y, bool repaint) const
{
    return this->MoveTo(this->X() + x, this->Y() + y, repaint);
}

bool View::MoveTo(int x, int y, bool repaint) const
{
    if (!this->hwnd)
    {
        return false;
    }

    return MoveWindow(this->hwnd, x, y, this->Width(), this->Height(), repaint ? TRUE : FALSE) ? true : false;
}

bool View::Resize(int width, int height, bool repaint) const
{
    if (!this->hwnd || width < 0 || height < 0)
    {
        return false;
    }

    return MoveWindow(this->hwnd, this->X(), this->Y(), width, height, repaint ? TRUE : FALSE) ? true : false;
}

bool View::ResizeClient(int width, int height, bool repaint) const
{
    return this->Resize(width + this->Width() - this->ClientWidth(), height + this->Height() - this->ClientHeight(), repaint);
}

void View::Enable() const
{
    if (this->hwnd)
    {
        EnableWindow(this->hwnd, TRUE);
    }
}

void View::Disable() const
{
    if (this->hwnd)
    {
        EnableWindow(this->hwnd, FALSE);
    }
}

bool View::IsEnabled() const
{
    if (this->hwnd)
    {
        return FALSE != IsWindowEnabled(this->hwnd);
    }

    return false;
}

bool View::IsVisible() const
{
    if (this->hwnd)
    {
        return FALSE != IsWindowVisible(this->hwnd);
    }

    return false;
}

bool View::HasFocus() const
{
    if (this->hwnd)
    {
        return GetFocus() == this->hwnd;
    }

    return false;
}

HWND View::SetFocus() const
{
    if (this->hwnd)
    {
        return ::SetFocus(this->hwnd);
    }

    return nullptr;
}

LRESULT View::Send(UINT uMsg, WPARAM wParam, LPARAM lParam) const
{
    return SendMessageW(this->hwnd, uMsg, wParam, lParam);
}

bool View::Post(UINT uMsg, WPARAM wParam, LPARAM lParam) const
{
    return FALSE != PostMessageW(this->hwnd, uMsg, wParam, lParam);
}

int View::X() const
{
    return this->wrect.left;
}

int View::Y() const
{
    return this->wrect.top;
}

int View::Width() const
{
    return this->wrect.right - this->wrect.left;
}

int View::Height() const
{
    return this->wrect.bottom - this->wrect.top;
}

RECT View::Rect() const
{
    return this->wrect;
}

int View::ClientWidth() const
{
    return crect.right - crect.left;
}

int View::ClientHeight() const
{
    return this->crect.bottom - this->crect.top;
}

RECT View::ClientRect() const
{
    return this->crect;
}

POINT View::Min() const
{
    return this->min;
}

POINT View::Max() const
{
    return this->max;
}

POINT View::MinClient() const
{
    return this->min;
}

void View::Min(int x, int y)
{
    this->min.x = x > 0 ? x : 0;
    this->min.y = y > 0 ? y : 0;
}

void View::Max(int x, int y)
{
    this->max.x = x > 0 ? x : 0;
    this->max.y = y > 0 ? y : 0;
}

POINT View::MaxClient() const
{
    return this->max;
}

void View::MinClient(int x, int y)
{
    this->min.x = (x > 0 ? x : 0) + this->Width()  - this->ClientWidth();
    this->min.y = (y > 0 ? y : 0) + this->Height() - this->ClientHeight();
}

void View::MaxClient(int x, int y)
{
    this->max.x = (x > 0 ? x : 0) + this->Width()  - this->ClientWidth();
    this->max.y = (y > 0 ? y : 0) + this->Height() - this->ClientHeight();
}

HWND View::Handle() const
{
    return this->hwnd;
}

bool View::Owner(View* parent)
{
    if (this->hwnd)
    {
        if (!parent)
        {
            SetWindowLongPtrW(this->hwnd, GWLP_HWNDPARENT, 0);
            SetWindowPos(this->hwnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
            this->owner = nullptr;
            return true;
        }
        else if (parent->hwnd)
        {
            SetWindowLongPtrW(this->hwnd, GWLP_HWNDPARENT, (LONG_PTR)parent->hwnd);
            SetWindowPos(this->hwnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_DRAWFRAME);
            this->owner = parent;
            return true;
        }
    }

    return false;
}

View* View::Owner() const
{
    return this->owner;
}

bool View::Parent(View* parent)
{
    if (this->hwnd)
    {
        if (!parent)
        {
            if (SetParent(this->hwnd, nullptr))
            {
                this->parent = nullptr;
                return true;
            }
        }
        else if (parent->hwnd)
        {
            if (SetParent(this->hwnd, parent->hwnd))
            {
                this->parent = parent;
                return true;
            }
        }
    }

    return false;
}

View* View::Parent() const
{
    return this->parent;
}

DialogItem View::Item(int id) const
{
    return DialogItem(this->hwnd, id);
}

void View::Cursor(LPWSTR name)
{
    if (name)
    {
        this->cursor = LoadCursorW(nullptr, name);
    }
    else
    {
        this->cursor = nullptr;
    }
}

void View::Caption(const wchar_t* caption) const
{
    if (this->hwnd)
    {
        SetWindowTextW(this->hwnd, caption);
    }
}

void View::Caption(const wstring& caption) const
{
    this->Caption(caption.c_str());
}

wstring View::Caption() const
{
    vector<wchar_t> buf;

    auto l = GetWindowTextLengthW(this->hwnd);
    if (l)
    {
        buf.resize(l + 1);

        if (GetWindowTextW(this->hwnd, &buf[0], (int)buf.size()))
        {
            return wstring(buf.data());
        }
    }

    return wstring();
}

bool View::Icon(UINT resourceId)
{
    HICON hIcon = LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCE(resourceId));
    if (hIcon)
    {
        SendMessageW(this->hwnd, WM_SETICON, ICON_BIG, LPARAM(hIcon));
        DestroyIcon(hIcon);
        return true;
    }

    return false;
}

void View::SetTimer(UINT_PTR id, UINT elapse)
{
    ::SetTimer(this->Handle(), id, elapse, nullptr);
}

void View::KillTimer(UINT_PTR id)
{
    ::KillTimer(this->Handle(), id);
}

void View::RegisterMessage(UINT message, const function<LRESULT()>& handler)
{
    this->messages[message] = make_pair(false, handler);
}

void View::RemoveMessage(UINT message)
{
    auto it = this->messages.find(message);
    if (this->messages.end() != it)
    {
        this->messages.erase(it);
    }
}

void View::RegisterCommand(UINT command, const function<void()>& handler)
{
    this->commands[command] = make_pair(false, handler);
}

void View::RemoveCommand(UINT command)
{
    auto it = this->commands.find(command);
    if (this->commands.end() != it)
    {
        this->commands.erase(it);
    }
}

bool View::OnSetCursor()
{
    if (this->cursor)
    {
        SetCursor(this->cursor);
        return true;
    }

    return false;
}

bool View::OnCreated()
{
    return true;
}

void View::OnDestroy()
{
}

void View::OnCommand()
{
    auto it = this->commands.find(this->command);
    if (this->commands.end() != it && !it->second.first)
    {
        it->second.first = true;
        it->second.second();
        it->second.first = false;
    }
    else if (this->parent)
    {
        this->parent->wparam  = this->wparam;
        this->parent->lparam  = this->lparam;
        this->parent->command = this->command;
        this->parent->OnCommand();
    }
}

void View::OnPaint()
{
    PAINTSTRUCT ps;

    auto hdc = BeginPaint(this->hwnd, &ps);
    EndPaint(this->hwnd, &ps);
}

void View::OnClose()
{
    this->Destroy();
}

void View::OnTimer()
{
}

void View::OnMove()
{
}

void View::OnSize()
{
}

void View::OnShow()
{
}

void View::OnHide()
{
}