#pragma once

#include "DialogItem.h"

class CheckBox : public DialogItem
{
public:
    static CheckBox Create(HWND parent, UINT id, const std::wstring& text, DWORD type = CheckBox::Auto, HINSTANCE instance = nullptr);

    void Check() const;
    bool IsChecked() const;
    void Uncheck() const;
    bool IsUnchecked() const;
    void Indeterminate() const;
    bool IsIndeterminate() const;

    static const DWORD Auto = BS_AUTOCHECKBOX;
    static const DWORD Auto3 = BS_AUTO3STATE;
    static const DWORD AutoRadio = BS_AUTORADIOBUTTON;
    static const DWORD AutoRadio1st = BS_AUTORADIOBUTTON | WS_GROUP;
    static const DWORD TriState = BS_3STATE;
};