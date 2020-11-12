#pragma once

#include "View.h"
#include "DialogItem.h"

class Dialog : public View
{
public:
    Dialog(UINT dialogID, HINSTANCE instance = nullptr, UINT acceleratorID = 0);

    virtual bool Create(View* parent = nullptr) override;

    int Modal(View* owner = nullptr);

    UINT DialogID() const;

protected:
    virtual LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM) override;
    virtual LRESULT DefaultProc(HWND, UINT, WPARAM, LPARAM) override;
    virtual void OnDestroy() override;
    virtual void OnInitDialog();

protected:
    UINT dialogID;
    UINT accelID;
    bool modal;
};