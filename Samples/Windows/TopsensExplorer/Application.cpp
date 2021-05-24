#include "Application.h"
#include "CheckBox.h"
#include "ComboBox.h"
#include "resource.h"

using namespace std;
using namespace Topsens;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    return Application(hInstance).Run(nCmdShow);
}

Application::Application(HINSTANCE instance) : MainWindow(instance), status(IDC_STATUS)
{
}

Application::~Application()
{
}

bool Application::OnCreated()
{
    if (!MainWindow::OnCreated())
    {
        return false;
    }

    if (!this->cview.Create(this) || !this->dview.Create(this) || !this->panel.Create(this) || !this->status.Create(this))
    {
        return false;
    }

    this->Caption(L"Topsens Explorer");
    this->Icon(IDI_APP);
    this->Style(this->Style() &~ WS_SIZEBOX &~ WS_MAXIMIZEBOX);

    this->cview.Show();
    this->dview.Show();
    this->panel.Show();
    this->status.Show();

    this->Arrange(Orientation::Landscape);
    this->OnRefresh();

    // Preload resources to minimize sensor start delay.
    // If skeleton is not needed, this can be omitted.
    Sensor::Preload({ 0, 1 });

    return true;
}

void Application::OnDestroy()
{
    this->OnStop();
    MainWindow::OnDestroy();
}

void Application::OnCommand()
{
    switch (this->command)
    {
        case IDC_REFRESH:
            this->OnRefresh();
            break;

        case IDC_START:
            this->OnStart();
            break;

        case IDC_STOP:
            this->OnStop();
            break;

        case IDC_USERS_NO:
            if (((CheckBox&)this->panel.Item(IDC_USERS_NO)).IsChecked())
            {
                ((CheckBox&)this->panel.Item(IDC_GROUND_YES)).Uncheck();
                ((CheckBox&)this->panel.Item(IDC_GROUND_NO)).Check();
            }

        case IDC_GROUND_YES:
            if (((CheckBox&)this->panel.Item(IDC_GROUND_YES)).IsChecked())
            {
                ((CheckBox&)this->panel.Item(IDC_USERS_YES)).Check();
                ((CheckBox&)this->panel.Item(IDC_USERS_NO)).Uncheck();
            }
            break;

        default:
            MainWindow::OnCommand();
    }
}

void Application::OnSize()
{
    this->status.AutoResize();
    MainWindow::OnSize();
}

void Application::OnRefresh()
{
    uint32_t count;
    auto err = Sensor::Count(count);

    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to get sensor count: " + GetError(err));
        return;
    }

    this->panel.Count(count);
    this->status.Text(count ? L"" : L"No sensor found");
}

void Application::OnStart()
{
    auto index = ((ComboBox&)this->panel.Item(IDC_SENSOR)).Selection();

    auto err = this->sensor.Open(index);
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to open sensor. Error: " + GetError(err));
        return;
    }

    auto orient = (Orientation)this->panel.Orientation();
    err = this->sensor.SetOrientation(orient);
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to set image orientation. Error: " + GetError(err));
        return;
    }

    this->cview.Orientation(orient);
    this->dview.Orientation(orient);

    err = this->sensor.SetDepthAligned(this->panel.Align());
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to set depth alignment. Error: " + GetError(err));
        return;
    }

    err = this->sensor.SetImageFlipped(this->panel.Flip());
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to set image flipped. Error: " + GetError(err));
        return;
    }

    err = this->sensor.SetRecording(this->panel.Record());
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to set stream recording. Error: " + GetError(err));
        return;
    }

    auto generateUsers = this->panel.GenerateUsers();
    auto paintGround   = this->panel.PaintGround();

    err = this->sensor.Start((Resolution)this->panel.ColorResolution(), (Resolution)this->panel.DepthResolution(), generateUsers);
    if (Error::Ok != err)
    {
        this->status.Text(L"Failed to open sensor. Error: " + GetError(err));
        return;
    }

    this->status.Text(nullptr);
    this->panel.Disable();
    this->Arrange(orient);

    this->stop = false;
    this->thread = std::thread([=]
    {
        ColorFrame cframe;
        DepthFrame dframe;
        UsersFrame uframe;

        while (!this->stop)
        {
            auto err = this->sensor.WaitColor(cframe, 1);
            switch (err)
            {
                case Error::Ok:
                {
                    this->cview.Error(nullptr);
                    this->cview.Draw(cframe);
                    this->cview.Invalidate();
                }

                case Error::Timeout:
                    break;

                default:
                {
                    this->cview.Error((L"Color: " + GetError(err)).c_str());
                    this->cview.Invalidate();
                }
            }

            err = this->sensor.WaitDepth(dframe, 1);
            switch (err)
            {
                case Error::Ok:
                {
                    this->dview.Error(nullptr);
                    this->dview.Ground(nullptr);

                    if (generateUsers)
                    {
                        err = this->sensor.WaitUsers(uframe, 0);
                        switch (err)
                        {
                            case Error::Ok:
                            {
                                this->dview.Draw(uframe);

                                if (paintGround)
                                {
                                    this->dview.Ground(&uframe.GroundPlane);
                                }
                                break;
                            }

                            case Error::Timeout:
                            {
                                this->dview.Error(L"Users/Depth lost sync");
                                break;
                            }

                            default:
                            {
                                this->dview.Error((L"Users: " + GetError(err)).c_str());
                            }
                        }
                    }

                    this->dview.Draw(dframe);
                    this->dview.Invalidate();
                }

                case Error::Timeout:
                    break;

                default:
                {
                    this->dview.Error((L"Depth: " + GetError(err)).c_str());
                    this->dview.Invalidate();
                }
            }
        }
    });
}

void Application::OnStop()
{
    this->stop = true;
    if (this->thread.joinable())
    {
        this->thread.join();
    }

    this->sensor.Stop();
    this->sensor.Close();
    this->panel.Enable();
}

void Application::Arrange(Orientation o)
{
    if (Orientation::Landscape == o || Orientation::Aerial == o)
    {
        this->cview.Resize(640, 480);
        this->dview.Resize(640, 480);
    }
    else
    {
        this->cview.Resize(640, 854);
        this->dview.Resize(640, 854);
    }
    this->cview.MoveTo(0, 0);
    this->dview.MoveTo(this->cview.Width(), 0);

    this->panel.MoveTo(0, max(this->cview.Height(), this->dview.Height()));
    this->panel.Resize(this->cview.Width() + this->dview.Width(), this->panel.Height());

    this->ResizeClient(this->cview.Width() + this->dview.Width(), this->cview.Height() + this->panel.Height() + this->status.Height());
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