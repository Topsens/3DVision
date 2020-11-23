#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_depthview.h"
#include "ui_panel.h"
#include <librealsense2/rs.hpp>

#define MIN_WIDTH   (810)

using namespace std;
using namespace rs2;
using namespace Topsens;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->dview = new DepthView(this);
    this->panel = new Panel(this);

    this->connect(this->panel->ui->start, SIGNAL(clicked()), this, SLOT(OnStart()));
    this->connect(this->panel->ui->stop,  SIGNAL(clicked()), this, SLOT(OnStop()));
    this->connect(this, SIGNAL(UpdateStatus()), this, SLOT(OnStatus()));

    People::Preload({ 0, 1 });
}

MainWindow::~MainWindow()
{
    this->OnStop();

    delete this->panel;
    delete ui;
}

void MainWindow::showEvent(QShowEvent *e)
{
    QMainWindow::showEvent(e);
    this->Arrange(MIN_WIDTH, 480, Orientation::Landscape);
}

void MainWindow::OnStart()
{
    auto serial = this->panel->Serial();
    if (serial.empty())
    {
        this->Status("No RealSense camera connected");
        return;
    }

    auto w = this->panel->Width();
    auto h = this->panel->Height();

    auto flip = this->panel->Flip();
    auto orient = this->panel->Orientation();

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
            this->Status("Failed to start RealSense");
            return;
        }

        auto intr = prof.get_stream(rs2_stream::RS2_STREAM_DEPTH).as<rs2::video_stream_profile>().get_intrinsics();

        People people;
        auto err = people.SetOrientation(orient);
        if (Error::Ok != err)
        {
            this->Status("Failed to set image orientation: " + GetError(err));
            pipe.stop();
            return;
        }

        err = people.SetImageFlipped(flip);
        if (Error::Ok != err)
        {
            this->Status("Failed to set image flip: " + GetError(err));
            pipe.stop();
            return;
        }

        err = people.Initialize(w, h, intr.fx, intr.fy, intr.ppx, intr.ppy);
        if (Error::Ok != err)
        {
            this->Status("Failed to initialize people: " + GetError(err));
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

                this->dview->Draw(depth, w, h, orient);

                UsersFrame users;
                auto err = people.Detect(depth, w, h, (int64_t)frame.get_timestamp(), users);
                if (Error::Ok == err)
                {
                    this->dview->Draw(users);
                    this->Status("");
                }
                else
                {
                    this->Status("Failed to detect: " + GetError(err));
                }

                this->dview->Update();
            }
        }

        people.Uninitialize();
        pipe.stop();
    });

    this->panel->Disable();
    this->Arrange(w, h, orient);
    this->Status("");
}

void MainWindow::OnStop()
{
    this->stop = true;
    if (this->thread.joinable())
    {
        this->thread.join();
    }

    this->panel->Enable();
}

void MainWindow::OnStatus()
{
    lock_guard<mutex> lock(this->lock);
    this->statusBar()->showMessage(this->status);
}

void MainWindow::Arrange(uint32_t w, uint32_t h, Orientation o)
{
    if (Orientation::Landscape != o)
    {
        w = w ^ h;
        h = w ^ h;
        w = w ^ h;
    }

    this->dview->resize(w, h);
    this->dview->move(std::max(0, (MIN_WIDTH - (int)w) / 2), 0);

    this->panel->resize(std::max(MIN_WIDTH, (int)w), this->panel->height());
    this->panel->move(0, h);

    this->setFixedSize(std::max(MIN_WIDTH, (int)w), h + this->panel->height() + this->statusBar()->height());
}

void MainWindow::Status(const QString& status)
{
    if (this->status == status)
    {
        return;
    }

    {
        lock_guard<mutex> lock(this->lock);
        this->status = status;
    }

    this->UpdateStatus();
}

QString MainWindow::GetError(Error error)
{
    switch (error)
    {
    case Error::Ok:
        return QString("No error");

    case Error::Timeout:
        return QString("Timeout");

    case Error::InvalidParameter:
        return QString("Invalid parameter");

    case Error::InvalidOperation:
        return QString("Invalid operation");

    case Error::Initialization:
        return QString("Not initialized/Initialization failed");

    case Error::NotSupported:
        return QString("Not supported");

    case Error::NotFound:
        return QString("Target not found");

    case Error::DeviceClosed:
        return QString("Device closed");

    case Error::DevicePermission:
        return QString("Device permission not granted");

    case Error::StreamDisabled:
        return QString("Stream disabled");

    case Error::StreamContent:
        return QString("Stream content error");

    case Error::StreamCodec:
        return QString("Stream encoding error");

    case Error::StreamEnd:
        return QString("Stream end reached");

    case Error::StreamIO:
        return QString("Stream IO failure");

    case Error::Resource:
        return QString("Failed to load resource");

    default:
        return QString::fromStdString(to_string((int)error));
    }
}
