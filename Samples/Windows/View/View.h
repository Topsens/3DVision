#pragma once

#include "DialogItem.h"
#include <functional>
#include <map>
#include <string>

class View
{
public:
    View(HINSTANCE);
    virtual ~View();

public:
    virtual bool Create(View* parent) = 0;

    void Destroy();

    DWORD Style() const;
    DWORD StyleEx() const;
    void  Style(DWORD style);
    void  StyleEx(DWORD style);
    void  ClipChildren(bool clip = true);

    void Show() const;
    void Hide() const;
    bool Update() const;
    bool Invalidate(bool erase = false) const;
    bool Invalidate(const RECT& rect, bool erase = false) const;

    bool Move(int x, int y, bool repaint = false) const;
    bool MoveTo(int x, int y, bool repaint = false) const;
    bool Resize(int width, int height, bool repaint = true) const;
    bool ResizeClient(int width, int height, bool repaint = true) const;

    void Enable() const;
    void Disable() const;
    bool IsEnabled() const;
    bool IsVisible() const;
    bool HasFocus() const;
    HWND SetFocus() const;

    bool    Post(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const;
    LRESULT Send(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0) const;

    int   X() const;
    int   Y() const;
    int   Width() const;
    int   Height() const;
    RECT  Rect() const;

    int   ClientWidth() const;
    int   ClientHeight() const;
    RECT  ClientRect() const;

    POINT Min() const;
    POINT Max() const;
    void  Min(int x, int y);
    void  Max(int x, int y);

    POINT MinClient() const;
    POINT MaxClient() const;
    void  MinClient(int x, int y);
    void  MaxClient(int x, int y);

    HWND  Handle() const;

    bool  Owner(View* parent);
    View* Owner() const;
    bool  Parent(View* parent);
    View* Parent() const;

    DialogItem Item(int id) const;

    void  Cursor(LPTSTR name);

    void Caption(const wchar_t* caption) const;
    void Caption(const std::wstring& caption) const;
    std::wstring Caption() const;

    void Icon(UINT);

    void SetTimer(UINT_PTR id, UINT elapse);
    void KillTimer(UINT_PTR id);

    void RegisterMessage(UINT message, const std::function<LRESULT()>& handler);
    void RemoveMessage(UINT message);
    void RegisterCommand(UINT command, const std::function<void()>& handler);
    void RemoveCommand(UINT command);

protected:
    virtual bool OnSetCursor();
    virtual bool OnCreated();
    virtual void OnDestroy();
    virtual void OnCommand();
    virtual void OnPaint();
    virtual void OnClose();
    virtual void OnTimer();
    virtual void OnMove();
    virtual void OnSize();
    virtual void OnShow();
    virtual void OnHide();

    static  WPARAM  MessageLoop(HWND hwnd, HACCEL haccel);
    static  LRESULT MessageRouter(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
            LRESULT MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual LRESULT WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT DefaultProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

protected:
    HWND  hwnd;
    UINT  umsg;
    UINT  dbgid;
    RECT  wrect;
    RECT  crect;
    POINT c2scr;
    POINT min;
    POINT max;
    View* owner;
    View* parent;
    WORD  command;
    DWORD style;
    DWORD styleEx;
    WPARAM wparam;
    LPARAM lparam;
    HCURSOR cursor;
    HINSTANCE instance;

    std::map<UINT, std::pair<bool, std::function<LRESULT()>>> messages;
    std::map<UINT, std::pair<bool, std::function<void()>>>    commands;
};