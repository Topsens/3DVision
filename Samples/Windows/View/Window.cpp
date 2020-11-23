#include "Window.h"

#define WNDCLASSNAME L"{317B2C5D-2708-458B-874A-93B07E3723D1}"

bool Window::registered = false;

Window::Window(HINSTANCE instance)
  : View(instance)
{
}

bool Window::Create(View* parent)
{
    if (!this->hwnd)
    {
        if (!Register())
        {
            return false;
        }

        this->owner  = parent;
        this->parent = parent;

        HWND hWndParent;

        if (parent)
        {
            hWndParent = parent->Handle();
            this->Style(this->Style() | WS_CHILD);
        }
        else
        {
            hWndParent = nullptr;
            if (this->Style())
            {
                this->Style(this->Style() &~ WS_CHILD);
            }
            else
            {
                this->Style(WS_OVERLAPPEDWINDOW);
            }
        }

        this->hwnd = CreateWindowExW(this->StyleEx(),
                                     WNDCLASSNAME,
                                     L"",
                                     this->Style(),
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     CW_USEDEFAULT,
                                     hWndParent,
                                     nullptr,
                                     this->instance,
                                     this);

        if (this->hwnd)
        {
            this->style   = (DWORD)GetWindowLongPtrW(this->hwnd, GWL_STYLE);
            this->styleEx = (DWORD)GetWindowLongPtrW(this->hwnd, GWL_EXSTYLE);
            
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
    }

    return nullptr != this->hwnd;
}

LRESULT Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_NCCREATE:
        if (!this->OnNcCreate())
        {
            return FALSE;
        }
        break;

    case WM_CREATE:
        this->c2scr = {0};
        ClientToScreen(this->Handle(), &this->c2scr);
        GetWindowRect(this->Handle(), &this->wrect);
        GetClientRect(this->Handle(), &this->crect);
        this->OnCreate();
        break;

    case WM_GETDLGCODE:
        return DLGC_WANTALLKEYS;

    default:
        break;
    }

    return View::WindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Window::DefaultProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

bool Window::OnNcCreate()
{
    return true;
}

void Window::OnCreate()
{
}

bool Window::Register()
{
    if (registered)
    {
        return true;
    }

    WNDCLASSEXW wcex   = {0};
    wcex.cbSize        = sizeof(wcex);
    wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc   = (WNDPROC)View::MessageRouter;
    wcex.hInstance     = GetModuleHandleW(nullptr);
    wcex.hIcon         = nullptr;
    wcex.hIconSm       = nullptr;
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName  = nullptr;
    wcex.lpszClassName = WNDCLASSNAME;

    auto atom = RegisterClassExW(&wcex);

    registered = (0 != atom);

    return registered;
}