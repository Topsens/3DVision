#include "Application.h"
#include "CheckBox.h"
#include "resource.h"

using namespace std;
using namespace Topsens;

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int nCmdShow)
{
    Application(instance).Run(nCmdShow);
}

Application::Application(HINSTANCE instance) : MainWindow(instance), status(IDC_STATUS)
{
}

bool Application::OnCreated()
{
    if (!this->cview.Create(this) || !this->panel.Create(this) || !this->status.Create(this, true))
    {
        return false;
    }
    this->cview.Show();
    this->panel.Show();
    this->status.Show();

    this->Caption(L"Topsens Point Cloud");
    this->Icon(IDI_APP);
    this->Start();

    return MainWindow::OnCreated();
}

void Application::OnDestroy()
{
    this->Stop();
    MainWindow::OnDestroy();
}

void Application::OnCommand()
{
    switch (this->command)
    {
        case IDC_COLOR:
        case IDC_FLIP:
        {
            this->Start();
            break;
        }

        default:
            MainWindow::OnCommand();
    }
}

void Application::OnSize()
{
    this->status.AutoResize();

    this->cview.MoveTo(0, 0);
    this->cview.Resize(this->ClientWidth(), this->ClientHeight() - this->panel.Height() - this->status.Height());

    this->panel.MoveTo(0, this->cview.Height());
    this->panel.Resize(this->cview.Width(), this->panel.Height());

    MainWindow::OnSize();
}

void Application::Start()
{
    this->Stop();

    uint32_t count;

    auto err = Sensor::Count(count);
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to get sensor count: " + GetError(err));
        return;
    }

    if (!count)
    {
        this->status.Text(L"No sensor found");
        return;
    }

    err = this->sensor.Open(0);
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to open sensor: " + GetError(err));
        return;
    }

    Resolution cres;
    if (((CheckBox&)this->panel.Item(IDC_COLOR)).IsChecked())
    {
        cres = Resolution::VGA;
    }
    else
    {
        cres = Resolution::Disabled;
    }

    err = this->sensor.SetDepthAligned(Resolution::Disabled != cres);
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to align depth: " + GetError(err));
        return;
    }

    err = this->sensor.SetImageFlipped(((CheckBox&)this->panel.Item(IDC_FLIP)).IsChecked());
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to set image flipped: " + GetError(err));
        return;
    }

    err = this->sensor.Start(cres, Resolution::VGA, false);
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to start sensor: " + GetError(err));
        return;
    }

    this->cview.Clear();
    this->status.Text(nullptr);

    this->stop = false;
    this->thread = std::thread([=]
    {
        ColorFrame cframe;
        DepthFrame dframe;

        while (!this->stop)
        {
            if (Resolution::Disabled != cres)
            {
                auto err = this->sensor.WaitColor(cframe, 1);
                if (Error::Ok == err)
                {
                    this->cview.ApplyColor(cframe);
                }
            }

            auto err = this->sensor.WaitDepth(dframe, 1);
            if (Error::Ok == err)
            {
                this->cview.Draw(dframe, Resolution::Disabled != cres);
            }
        }
    });
}

void Application::Stop()
{
    this->stop = true;

    if (this->thread.joinable())
    {
        this->thread.join();
    }

    this->sensor.Stop();
    this->sensor.Close();
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