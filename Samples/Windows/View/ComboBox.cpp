#include "ComboBox.h"
#include <CommCtrl.h>

using namespace std;

ComboBox ComboBox::Create(HWND parent, UINT id, DWORD type, HINSTANCE instance)
{
    DialogItem di;

    if (parent)
    {
        DWORD style = WS_CHILD | WS_TABSTOP | WS_VSCROLL;

        if (ComboBox::Simple != type)
        {
            style |= WS_OVERLAPPED;
        }
        
        auto hwnd = CreateWindowExW(0, WC_COMBOBOXW, nullptr, type | style, 0, 0, 0, 0, parent, (HMENU)id, instance, nullptr);
        if (hwnd)
        {
            di = DialogItem(parent, hwnd, id);
        }
    }

    return (ComboBox&)di;
}

int ComboBox::Add(const wchar_t* item)
{
    return (int)this->Send(CB_ADDSTRING, 0, (LPARAM)item);
}

int ComboBox::Add(const wstring& item)
{
    return this->Add(item.c_str());
}

int ComboBox::Insert(const wchar_t* item, int index)
{
    return (int)this->Send(CB_INSERTSTRING, (WPARAM)index, (LPARAM)item);
}

int ComboBox::Insert(const wstring& item, int index)
{
    return this->Insert(item.c_str(), index);
}

int ComboBox::Remove(int index)
{
    return (int)this->Send(CB_DELETESTRING, (WPARAM)index);
}

int ComboBox::Count() const
{
    return (int)this->Send(CB_GETCOUNT);
}

void ComboBox::Clear()
{
    this->Send(CB_RESETCONTENT);
}

int ComboBox::FindExact(const wchar_t* item, int first) const
{
    return (int)this->Send(CB_FINDSTRINGEXACT, (WPARAM)(first - 1), (LPARAM)item);
}

int ComboBox::FindExact(const wstring& item, int first) const
{
    return this->FindExact(item.c_str(), first);
}

int ComboBox::FindBeginWith(const wchar_t* item, int first) const
{
    return (int)this->Send(CB_FINDSTRING, (WPARAM)(first - 1), (LPARAM)item);
}

int ComboBox::FindBeginWith(const wstring& item, int first) const
{
    return this->FindBeginWith(item.c_str(), first);
}

int ComboBox::TextLength(int index) const
{
    return (int)this->Send(CB_GETLBTEXTLEN, (WPARAM)index);
}

bool ComboBox::GetText(int index, wstring& text) const
{
    text.clear();

    auto cnt = this->TextLength(index);
    if (CB_ERR == cnt)
    {
        return false;
    }

    text.resize(cnt + 1);
    if (CB_ERR == this->Send(CB_GETLBTEXT, (WPARAM)index, (LPARAM)&text[0]))
    {
        text.clear();
        return false;
    }

    text.resize(cnt);
    return true;
}

int ComboBox::Selection() const
{
    return (int)this->Send(CB_GETCURSEL);
}

bool ComboBox::Select(int index) const
{
    if (index < 0 && index >= this->Count())
    {
        return false;
    }

    return CB_ERR != this->Send(CB_SETCURSEL, (WPARAM)index);
}

void ComboBox::ClearSelection() const
{
    this->Send(CB_SETCURSEL, (WPARAM)-1);
}

wstring ComboBox::Text() const
{
    wstring text;

    auto index = this->Selection();
    if (index >= 0)
    {
        this->GetText(index, text);
    }

    return text;
}