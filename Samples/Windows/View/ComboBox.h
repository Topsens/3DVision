#pragma once

#include "DialogItem.h"

class ComboBox : public DialogItem
{
public:
    static ComboBox Create(HWND parent, UINT id, DWORD type = ComboBox::DropdownList, HINSTANCE instance = nullptr);

    int  Add(const wchar_t* item);
    int  Add(const std::wstring& item);
    int  Insert(const wchar_t* item, int index);
    int  Insert(const std::wstring& item, int index);
    int  Remove(int index);
    int  Count() const;
    void Clear();

    int  FindExact(const wchar_t* item, int first = 0) const;
    int  FindExact(const std::wstring& item, int first = 0) const;
    int  FindBeginWith(const wchar_t* item, int first = 0) const;
    int  FindBeginWith(const std::wstring& item, int first = 0) const;

    int  TextLength(int index) const;
    bool GetText(int index, std::wstring& text) const;

    int  Selection() const;
    bool Select(int index) const;
    void ClearSelection() const;

    std::wstring Text() const;

    static const DWORD Simple = CBS_SIMPLE;
    static const DWORD Dropdown = CBS_DROPDOWN;
    static const DWORD DropdownList = CBS_DROPDOWNLIST;
};