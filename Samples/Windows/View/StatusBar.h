#pragma once

#include "View.h"
#include <functional>
#include <map>
#include <vector>
#include <CommCtrl.h>

class StatusBar : public View
{
public:
    StatusBar(UINT controlID, HINSTANCE instance = nullptr);

    operator bool() const;

    bool Create(View* parent) override;
    bool Create(View* parent, bool sizeGrip);
    void AutoResize();

    bool SetParts(UINT parts, int* rightEdgePositions) const;
    UINT GetParts() const;
    UINT GetParts(std::vector<int>& positions) const;

    bool Text(const wchar_t* text, UINT part = 0) const;
    bool Text(const std::wstring& text, UINT part = 0) const;
    std::wstring Text(UINT part = 0) const;

protected:
    LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM) override;
    LRESULT DefaultProc(HWND, UINT, WPARAM, LPARAM) override;

    bool OnCreated() override;

private:
    UINT id;
    WNDPROC defaultProc;
};