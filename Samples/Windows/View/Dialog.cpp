#include "Dialog.h"

Dialog::Dialog(UINT dialogID, HINSTANCE instance, UINT acceleratorID)
  : View(instance), dialogID(dialogID), accelID(acceleratorID), modal(false)
{
}

bool Dialog::Create(View* parent)
{
    if (!this->hwnd)
    {
        this->owner  = parent;
        this->parent = parent;

        HWND hWndParent = nullptr;
        if (parent)
        {
            hWndParent = parent->Handle();
        }

        this->hwnd = CreateDialogParamW(this->instance,
                                        MAKEINTRESOURCEW(this->DialogID()),
                                        hWndParent,
                                        (DLGPROC)MessageRouter,
                                        (LPARAM)this);

        if (this->hwnd)
        {
            this->parent  = parent;
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

    return (nullptr != this->hwnd);
}

int Dialog::Modal(View* owner)
{
    if (owner)
    {
        owner->Disable();
    }

    this->Owner(owner);
    this->Show();

    HACCEL haccel = nullptr;
    if (this->accelID)
    {
        haccel = LoadAcceleratorsW(this->instance, MAKEINTRESOURCEW(this->accelID));
    }

    this->modal = true;

    auto ret = (int)View::MessageLoop(this->Handle(), haccel);

    if (owner)
    {
        owner->Enable();
    }

    return ret;
}

UINT Dialog::DialogID() const
{
    return this->dialogID;
}

LRESULT Dialog::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        this->c2scr = {0};
        ClientToScreen(this->Handle(), &this->c2scr);
        GetWindowRect(this->Handle(), &this->wrect);
        GetClientRect(this->Handle(), &this->crect);
        this->OnInitDialog();
        break;

    default:
        break;
    }

    return View::WindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT Dialog::DefaultProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

void Dialog::OnDestroy()
{
    if (this->modal)
    {
        PostQuitMessage(0);
        this->modal = false;
    }

    View::OnDestroy();
}

void Dialog::OnInitDialog()
{
}