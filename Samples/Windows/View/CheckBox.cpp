#include "CheckBox.h"
#include <CommCtrl.h>

using namespace std;

CheckBox CheckBox::Create(HWND parent, UINT id, const wstring& text, DWORD type, HINSTANCE instance)
{
    DialogItem di;

    if (parent)
    {
        auto hwnd = CreateWindowExW(0, WC_BUTTONW, text.c_str(), type | BS_CHECKBOX | WS_CHILD | WS_TABSTOP, 0, 0, 0, 0, parent, (HMENU)id, instance, nullptr);
        if (hwnd)
        {
            di = DialogItem(parent, hwnd, id);
        }
    }

    return (CheckBox&)di;
}

void CheckBox::Check() const
{
    this->Send(BM_SETCHECK, BST_CHECKED);
}

bool CheckBox::IsChecked() const
{
    return BST_CHECKED == this->Send(BM_GETCHECK);
}

void CheckBox::Uncheck() const
{
    this->Send(BM_SETCHECK, BST_UNCHECKED);
}

bool CheckBox::IsUnchecked() const
{
    return BST_UNCHECKED == this->Send(BM_GETCHECK);
}

void CheckBox::Indeterminate() const
{
    this->Send(BM_SETCHECK, BST_INDETERMINATE, 0);
}

bool CheckBox::IsIndeterminate() const
{
    return BST_INDETERMINATE == this->Send(BM_GETCHECK);
}