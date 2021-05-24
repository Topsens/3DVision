#include "Application.h"
#include "resource.h"
#include "CheckBox.h"
#include "ComboBox.h"
#include <TopsensPeople.hpp>

#define noexcept
#include <librealsense2\rs.hpp>

#define WM_STATUS   (WM_APP + 1)

using namespace std;
using namespace rs2;
using namespace Topsens;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    return Application(hInstance).Run(nCmdShow);
}

Application::Application(HINSTANCE instance) : MainWindow(instance), status(1000)
{
}

bool Application::OnCreated()
{
    if (!MainWindow::OnCreated())
    {
        return false;
    }

    if (!this->dview.Create(this) || !this->panel.Create(this) || !this->status.Create(this))
    {
        return false;
    }

    this->MinClient(640, this->panel.Height() + this->status.Height());

    this->RegisterCommand(IDC_START, [this]{ this->OnStart(); });
    this->RegisterCommand(IDC_STOP, [this]{ this->OnStop(); });

    this->RegisterMessage(WM_STATUS, [this]
    {
        lock_guard<mutex> lock(this->lock);
        this->status.Text(this->info);
        return 0;
    });

    this->panel.Item(IDC_STOP).Disable();

    this->dview.Show();
    this->panel.Show();
    this->status.Show();

    this->Style(this->Style() &~ WS_SIZEBOX &~ WS_MAXIMIZEBOX &~ WS_MINIMIZEBOX);
    this->ResizeClient(640, 480 + this->panel.Height() + this->status.Height());
    this->Caption(L"Topsens on RealSense");
    this->Icon(IDI_ICON);

    return true;
}

void Application::OnDestroy()
{
    this->OnStop();
    MainWindow::OnDestroy();
}

void Application::OnSize()
{
    this->status.AutoResize();

    auto h = this->ClientHeight() - this->status.Height() - this->panel.Height();

    this->panel.MoveTo(0, h);
    this->panel.Resize(this->ClientWidth(), this->panel.Height());
    this->dview.Resize(this->ClientWidth(), h);
}

void Application::OnStart()
{
    if (!this->panel.Count())
    {
        this->Status(L"No RealSense camera connected");
        return;
    }

    auto flip   = this->panel.Flip();
    auto serial = this->panel.Serial();
    auto orient = this->panel.Orientation();

    uint32_t w = this->panel.XRes();
    uint32_t h = this->panel.YRes();

    if (Orientation::Landscape == orient || Orientation::Aerial == orient)
    {
        this->ResizeClient(w, h + this->panel.Height() + this->status.Height());
    }
    else
    {
        this->ResizeClient(h, w + this->panel.Height() + this->status.Height());
    }

    this->stop = false;
    this->thread = std::thread([=]
    {
        config cfg;
        cfg.enable_stream(rs2_stream::RS2_STREAM_DEPTH, w, h, rs2_format::RS2_FORMAT_Z16, 30);
        cfg.enable_device(serial);

        auto pipe = rs2::pipeline();

        pipeline_profile prof;
        try
        {
            prof = pipe.start(cfg);

        } catch (...)
        {
            this->Status(L"Failed to start RealSense");
            return;
        }

        People people;
        auto err = people.SetOrientation(orient);
        if (Error::Ok != err)
        {
            this->Status(L"Failed to set image orientation: " + GetError(err));
            pipe.stop();
            return;
        }

        err = people.SetImageFlipped(flip);
        if (Error::Ok != err)
        {
            this->Status(L"Failed to set image flip: " + GetError(err));
            pipe.stop();
            return;
        }

        auto intr = prof.get_stream(rs2_stream::RS2_STREAM_DEPTH).as<rs2::video_stream_profile>().get_intrinsics();
        err = people.Initialize(w, h, intr.fx, intr.fy, intr.ppx, intr.ppy);
        if (Error::Ok != err)
        {
            this->Status(L"Failed to initialize people: " + GetError(err));
            pipe.stop();
            return;
        }

        vector<uint16_t> flipped;

        while (!this->stop)
        {
            auto frame = pipe.wait_for_frames().first(rs2_stream::RS2_STREAM_DEPTH);
            if (frame)
            {
                auto depth = (uint16_t*)frame.get_data();

                if (flip)
                {
                    flipped.resize(w * h);

                    for (uint32_t i = 0; i < h; i++)
                    {
                        auto off0 = i * w;
                        auto off1 = off0 + w - 1;

                        for (uint32_t j = 0; j < w; j++)
                        {
                            flipped[off1 - j] = depth[off0 + j];
                        }
                    }

                    depth = flipped.data();
                }

                this->dview.Draw(depth, w, h, orient);

                UsersFrame users;
                auto err = people.Detect(depth, w, h, (int64_t)frame.get_timestamp(), users);
                if (Error::Ok == err)
                {
                    this->dview.Draw(users, orient);
                    this->dview.Error(L"");
                }
                else
                {
                    this->dview.Error(L"Failed to detect people: " + GetError(err));
                }

                this->dview.Invalidate();
            }
        }

        people.Uninitialize();
        pipe.stop();
    });

    this->panel.Disable();

    this->Status(L"");
}

void Application::OnStop()
{
    this->stop = true;

    if (this->thread.joinable())
    {
        this->thread.join();
    }

    this->panel.Enable();
}

void Application::Status(const wstring& info)
{
    lock_guard<mutex> lock(this->lock);
    this->info = info;
    this->Post(WM_STATUS);
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