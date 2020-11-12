#pragma once

#include "DialogItem.h"

class Button : public DialogItem
{
public:
    static Button Create(HWND parent, UINT id, const std::wstring& text, HINSTANCE instance = nullptr);
};