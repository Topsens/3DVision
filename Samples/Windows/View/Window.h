#pragma once

#include "View.h"

class Window : public View
{
public:
    Window(HINSTANCE instance = nullptr);

    virtual bool Create(View* parent = nullptr) override;

protected:
    virtual LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM) override;
    virtual LRESULT DefaultProc(HWND, UINT, WPARAM, LPARAM) override;
    virtual bool OnNcCreate();
    virtual void OnCreate();

private:
    static bool Register();

private:
    static bool registered;
};