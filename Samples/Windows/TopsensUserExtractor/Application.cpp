#include "Application.h"
#include "GDIRenderer.h"
#include "resource.h"
#include <gdiplus.h>

using namespace std;
using namespace Gdiplus;
using namespace Topsens;
using namespace Topsens::Toolkit;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    ULONG_PTR token;
    GdiplusStartupInput input;
    GdiplusStartupOutput output;
    GdiplusStartup(&token, &input, &output);

    auto ret = Application(hInstance).Run(nCmdShow);

    GdiplusShutdown(token);
    return ret;
}

Application::Application(HINSTANCE instance) : MainWindow(instance)
{
}

bool Application::OnCreated()
{
    if (!MainWindow::OnCreated())
    {
        return false;
    }

    if (!this->LoadBackground())
    {
        MessageBoxW(this->Handle(), L"Failed to load background image", nullptr, MB_OK);
        return false;
    }

    Resolution resolution = Resolution::VGA;

    auto err = this->extractor.Initialize(resolution);
    if (Error::Ok != err)
    {
        MessageBoxW(this->Handle(), (L"Failed to initialize extractor: " + GetError(err)).c_str(), nullptr, MB_OK);
        return false;
    }

    err = this->sensor.Open(0);
    if (Error::Ok != err)
    {
        MessageBoxW(this->Handle(), (L"Failed to open first sensor: " + GetError(err)).c_str(), nullptr, MB_OK);
        return false;
    }

    err = this->sensor.SetDepthAligned(true);
    if (Error::Ok != err)
    {
        MessageBoxW(this->Handle(), (L"Failed to set depth alignment: " + GetError(err)).c_str(), nullptr, MB_OK);
        return false;
    }

    err = this->sensor.SetMaskEnabled(true);
    if (Error::Ok != err)
    {
        MessageBoxW(this->Handle(), (L"Failed to enable mask: " + GetError(err)).c_str(), nullptr, MB_OK);
        return false;
    }

    err = this->sensor.Start(resolution, resolution, true);
    if (Error::Ok != err)
    {
        MessageBoxW(this->Handle(), (L"Failed to start sensor" + GetError(err)).c_str(), nullptr, MB_OK);
        return false;
    }

    this->Caption(L"Topsens User Extractor");
    this->Icon(IDI_APP);
    this->Style(this->Style() &~ WS_SIZEBOX &~ WS_MAXIMIZEBOX);
    this->ResizeClient(640, 480);

    this->stop = false;
    this->thread = std::thread([=]
    {
        ColorFrame cframe = {};
        UsersFrame uframe = {};

        while (!this->stop)
        {
            ColorFrame cf;

            auto err = this->sensor.WaitColor(cf, 1);
            if (Error::Ok == err)
            {
                cframe = cf;
            }

            err = this->sensor.WaitUsers(uframe, 1);
            if (Error::Ok == err && cframe.Width && cframe.Height)
            {
                lock_guard<mutex> lock(this->lock);

                err = this->extractor.Extract(cframe, uframe, this->extracted);
                if (Error::Ok == err)
                {
                    this->imgW = cframe.Width;
                    this->imgH = cframe.Height;
                    this->Invalidate();
                }
            }
        }
    });

    return true;
}

void Application::OnDestroy()
{
    this->stop = true;
    if (this->thread.joinable())
    {
        this->thread.join();
    }

    MainWindow::OnDestroy();
}

void Application::OnPaint()
{
    GDIRenderer renderer;
    if (renderer.BeginPaint(this->Handle()))
    {
        renderer.PixelsRGB(this->bkgPixels.data(), this->bkgW, this->bkgH, 0, 0, this->ClientWidth(), this->ClientHeight());

        if (!this->extracted.empty())
        {
            renderer.PixelsARGB((BYTE*)this->extracted.data(), this->imgW, this->imgH, 0, 0, this->ClientWidth(), this->ClientHeight(), true);
        }

        renderer.EndPaint();
    }
    else
    {
        MainWindow::OnPaint();
    }
}

bool Application::LoadBackground()
{
    auto bmp = Bitmap::FromFile(L"Background.jpg");
    if (!bmp)
    {
        return false;
    }

    this->bkgW = bmp->GetWidth();
    this->bkgH = bmp->GetHeight();

    auto data = new BitmapData();
    bmp->LockBits(nullptr, ImageLockModeRead, PixelFormat24bppRGB, data);

    this->bkgPixels.resize(this->bkgW * this->bkgH * 3);
    for (UINT i = 0; i < this->bkgH; i++)
    {
        memcpy_s(&this->bkgPixels[i * this->bkgW * 3], this->bkgW * 3, (uint8_t*)data->Scan0 + i * data->Stride, this->bkgW * 3);
    }

    bmp->UnlockBits(data);

    delete data;
    delete bmp;
    return true;
}

wstring Application::GetError(Error error)
{
    switch (error)
    {
        case Error::Ok:
            return L"No error";

        case Error::Timeout:
            return L"Timeout";

        case Error::InvalidParameter:
            return L"Invalid parameter";

        case Error::InvalidOperation:
            return L"Invalid operation";

        case Error::Initialization:
            return L"Not initialized/Initialization failed";

        case Error::NotSupported:
            return L"Not supported";

        case Error::NotFound:
            return L"Target not found";

        case Error::Closed:
            return L"Object is closed";

        case Error::DeviceClosed:
            return L"Device closed";

        case Error::DevicePermission:
            return L"Device permission not granted";

        case Error::StreamDisabled:
            return L"Stream disabled";

        case Error::StreamContent:
            return L"Stream content error";

        case Error::StreamCodec:
            return L"Stream encoding error";

        case Error::StreamEnd:
            return L"Stream end reached";

        case Error::StreamIO:
            return L"Stream IO failure";

        case Error::Resource:
            return L"Failed to load resource";

        default:
            return to_wstring((int)error);
    }
}