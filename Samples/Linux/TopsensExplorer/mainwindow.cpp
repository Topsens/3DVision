#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_panel.h"
#include "userpainter.h"
#include <QPainter>
#include <QMessageBox>
#include <iostream>

#define COLOR_FAR       (0xFFFF0000)
#define COLOR_GROUND    (0xFF808080)

using namespace std;
using namespace Topsens;

vector<int> MainWindow::palette;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->panel = new Panel(this);
    this->panel->resize(this->width(), this->panel->height());
    this->panel->move(0, max(this->ui->lbColor->height(), this->ui->lbDepth->height()));

    this->connect(this, SIGNAL(UpdateColor()), this, SLOT(OnUpdateColor()));
    this->connect(this, SIGNAL(UpdateDepth()), this, SLOT(OnUpdateDepth()));

    this->connect(this->panel->ui->rbUsersNo,   SIGNAL(clicked(bool)), this, SLOT(OnUsersNo(bool)));
    this->connect(this->panel->ui->rbGroundYes, SIGNAL(clicked(bool)), this, SLOT(OnGroundYes(bool)));
    this->connect(this->panel->ui->bnRefresh,   SIGNAL(clicked()),     this, SLOT(OnRefresh()));
    this->connect(this->panel->ui->bnStart,     SIGNAL(clicked()),     this, SLOT(OnStart()));
    this->connect(this->panel->ui->bnStop,      SIGNAL(clicked()),     this, SLOT(OnStop()));

    if (palette.empty())
    {
        palette.resize(0x1000);

        for (auto d = 0; d < 0x190; d++)
        {
            palette[d] = 0xFF000000;
        }

        for (auto d = 0x190; d < 0x400; d++)
        {
            int r = 0xFF - 0xFF * (d - 0x190) / (0x400 - 0x190);
            int g = 0;
            int b = 0xFF;

            palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
        }

        for (auto d = 0x400; d < 0x600; d++)
        {
            int r = 0;
            int g = 0xFF * (d - 0x400) / (0x600 - 0x400);
            int b = 0xFF;

            palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
        }

        for (auto d = 0x600; d < 0x800; d++)
        {
            int r = 0;
            int g = 0xFF;
            int b = 0xFF - 0xFF * (d - 0x600) / (0x800 - 0x600);

            palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
        }

        for (auto d = 0x800; d < 0xC00; d++)
        {
            int r = 0xFF * (d - 0x800) / (0xC00 - 0x800);
            int g = 0xFF;
            int b = 0;

            palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
        }

        for (auto d = 0xC00; d < 0x1000; d++)
        {
            int r = 0xFF;
            int g = 0xFF - 0xFF * (d - 0xC00) / (0x1000 - 0xC00);
            int b = 0;

            this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
        }
    }
}

MainWindow::~MainWindow()
{
    delete this->panel;
    delete ui;
}

void MainWindow::showEvent(QShowEvent* e)
{
    QMainWindow::showEvent(e);

    this->Arrange(Orientation::Landscape);
    this->OnRefresh();

    // Pre-load resources to minimize sensor start delay.
    Sensor::Preload({ 0, 1 });
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    QMainWindow::closeEvent(e);
    this->OnStop();
}

void MainWindow::OnUpdateColor()
{
    lock_guard<mutex> lock(this->cmutex);

    if (this->cpixmap.width())
    {
        auto l = this->ui->lbColor;

        switch (this->orientation)
        {
            case Orientation::Landscape:
            {
                l->setPixmap(this->cpixmap.scaled(l->width(), l->height()));
                break;
            }

            case Orientation::PortraitClockwise:
            {
                l->setPixmap(this->cpixmap.transformed(QTransform().rotate(90)).scaled(l->width(), l->height()));
                break;
            }

            case Orientation::PortraitAntiClockwise:
            {
                l->setPixmap(this->cpixmap.transformed(QTransform().rotate(-90)).scaled(l->width(), l->height()));
                break;
            }

            default: break;
        }
    }
}

void MainWindow::OnUpdateDepth()
{
    lock_guard<mutex> lock(this->dmutex);

    if (this->dpixmap.width())
    {
        auto l = this->ui->lbDepth;

        switch (this->orientation)
        {
            case Orientation::Landscape:
            {
                l->setPixmap(this->dpixmap.scaled(l->width(), l->height()));
                break;
            }

            case Orientation::PortraitClockwise:
            {
                l->setPixmap(this->dpixmap.transformed(QTransform().rotate(90)).scaled(l->width(), l->height()));
                break;
            }

            case Orientation::PortraitAntiClockwise:
            {
                l->setPixmap(this->dpixmap.transformed(QTransform().rotate(-90)).scaled(l->width(), l->height()));
                break;
            }

            default: break;
        }
    }
}

void MainWindow::OnRefresh()
{
    this->ui->statusBar->showMessage(QString());

    uint32_t count;
    auto err = Sensor::Count(count);
    if (Error::Ok != err)
    {
        this->ui->statusBar->showMessage(QString("Failed to get sensor count. Error: ") + GetError(err));
        return;
    }

    if (0 == count)
    {
        this->ui->statusBar->showMessage(QString("No sensor found"));
    }

    this->panel->Count(count);
}

void MainWindow::OnStart()
{
    auto err = this->sensor.Open(this->panel->ui->cbDevice->currentIndex());
    if (Error::Ok != err)
    {
        this->ui->statusBar->showMessage(QString("Failed to open sensor. Error: ") + GetError(err));
        return;
    }

    this->orientation = this->panel->Orientation();
    err = this->sensor.SetOrientation(this->orientation);
    if (Error::Ok != err)
    {
        this->ui->statusBar->showMessage(QString("Failed to set sensor orientation. Error: ") + GetError(err));
        return;
    }

    err = this->sensor.SetDepthAligned(this->panel->Align());
    if (Error::Ok != err)
    {
        this->ui->statusBar->showMessage(QString("Failed to set depth alignment. Error: ") + GetError(err));
        return;
    }

    err = this->sensor.SetImageFlipped(this->panel->Flip());
    if (Error::Ok != err)
    {
        this->ui->statusBar->showMessage(QString("Failed to set image flipping. Error: ") + GetError(err));
        return;
    }

    err = this->sensor.SetRecording(this->panel->Record());
    if (Error::Ok != err)
    {
        this->ui->statusBar->showMessage(QString("Failed to set stream recording. Error: ") + GetError(err));
        return;
    }

    auto colorResolution = this->panel->ColorRes();
    auto depthResolution = this->panel->DepthRes();
    auto generateUsers   = this->panel->GenUsers();
    auto paintGround     = this->panel->PaintGround();

    err = this->sensor.Start(colorResolution, depthResolution, generateUsers);
    if (Error::Ok != err)
    {
        this->ui->statusBar->showMessage(QString("Failed to start sensor. Error: ") + GetError(err));
        return;
    }

    this->ui->statusBar->showMessage(QString());

    this->stop = false;
    this->thread = std::thread([=]
    {
        ColorFrame cframe;
        DepthFrame dframe;
        UsersFrame uframe;

        vector<int> dpixels;

        while (!this->stop)
        {
            if (Resolution::Disabled != (Resolution)colorResolution)
            {
                auto err = this->sensor.WaitColor(cframe, 1);
                switch (err)
                {
                    case Error::Ok:
                    {
                        this->ui->lbColor->setText(QString(""));
                        this->PaintColor(cframe);
                        this->UpdateColor();
                        break;
                    }

                    case Error::Timeout:
                    {
                        break;
                    }

                    default:
                    {
                        this->ui->lbColor->setText(QString("Color: ") + GetError(err));
                        break;
                    }
                }
            }

            if (Resolution::Disabled != (Resolution)depthResolution)
            {
                auto err = this->sensor.WaitDepth(dframe, 1);
                switch (err)
                {
                    case Error::Ok:
                    {
                        this->PaintDepth(dframe);

                        if (generateUsers)
                        {
                            err = this->sensor.WaitUsers(uframe, 0);
                            switch (err)
                            {
                                case Error::Ok:
                                {
                                    if (paintGround && !(isnan(uframe.GroundPlane.X) || isnan(uframe.GroundPlane.Y) || isnan(uframe.GroundPlane.Z) || isnan(uframe.GroundPlane.W)))
                                    {
                                        this->PaintGround(dframe, uframe.GroundPlane);
                                    }
                                    else
                                    {
                                        this->PaintDepth(dframe);
                                    }
                                    PaintUsers(uframe);
                                    this->ui->lbDepth->setText(QString(""));
                                    break;
                                }

                                case Error::Timeout:
                                {
                                    this->ui->lbDepth->setText(QString("Users frame sync error"));
                                    break;
                                }

                                default:
                                {
                                    this->ui->lbDepth->setText(QString("Users: " + GetError(err)));
                                    this->PaintDepth(dframe);
                                    break;
                                }
                            }
                        }

                        this->UpdateDepth();
                        break;
                    }

                    case Error::Timeout:
                    {
                        break;
                    }

                    default:
                    {
                        this->ui->lbDepth->setText(QString("Depth: ") + GetError(err));
                        break;
                    }
                }
            }
        }

        this->sensor.Stop();
        this->sensor.Close();
    });

    this->panel->Disable();

    this->Arrange(this->orientation);
}

void MainWindow::OnStop()
{
    this->stop = true;
    if (this->thread.joinable())
    {
        this->thread.join();
    }

    this->panel->Enable();
    this->OnRefresh();
}

void MainWindow::OnUsersNo(bool checked)
{
    if (checked)
    {
        this->panel->ui->rbGroundNo->setChecked(true);
    }
}

void MainWindow::OnGroundYes(bool checked)
{
    if (checked)
    {
        this->panel->ui->rbUsersYes->setChecked(true);
    }
}

void MainWindow::PaintColor(const ColorFrame& frame)
{
    lock_guard<mutex> lock(this->cmutex);
    this->cpixels.resize(frame.Width * frame.Height);
    memcpy(&this->cpixels[0], frame.Pixels, this->cpixels.size() * sizeof(this->cpixels[0]));
    this->cpixmap = QPixmap::fromImage(QImage((uchar*)this->cpixels.data(), frame.Width, frame.Height, QImage::Format_ARGB32));
}

void MainWindow::PaintDepth(const DepthFrame& frame)
{
    lock_guard<mutex> lock(this->dmutex);

    this->dpixels.resize(frame.Width * frame.Height);

    for (size_t i = 0; i < this->dpixels.size(); i++)
    {
        auto d = frame.Pixels[i];
        this->dpixels[i] = d < palette.size() ? palette[d] : COLOR_FAR;
    }
    this->dpixmap = QPixmap::fromImage(QImage((uchar*)this->dpixels.data(), frame.Width, frame.Height, QImage::Format_ARGB32));
}

void MainWindow::PaintUsers(const UsersFrame& frame)
{
    QPainter painter(&this->dpixmap);
    UserPainter::Paint(frame, painter, this->dpixmap.width() / 640.f);
}

void MainWindow::PaintGround(const DepthFrame& frame, const Vector4& groundPlane)
{
    vector<Vector3> cloud;
    if (Error::Ok == frame.ToPointCloud(cloud))
    {
        lock_guard<mutex> lock(this->dmutex);

        this->dpixels.resize(frame.Width * frame.Height);

        for (size_t i = 0; i < this->dpixels.size(); i++)
        {
            auto d = frame.Pixels[i];
            if (d < palette.size())
            {
                if (IsGround(cloud[i], groundPlane))
                {
                    this->dpixels[i] = COLOR_GROUND;
                }
                else
                {
                    this->dpixels[i] = palette[d];
                }
                //this->dpixels[i] = IsGround(cloud[i], groundPlane) ? COLOR_GROUND : palette[d];
            }
            else
            {
                this->dpixels[i] = COLOR_FAR;
            }
        }

        this->dpixmap = QPixmap::fromImage(QImage((uchar*)this->dpixels.data(), frame.Width, frame.Height, QImage::Format_ARGB32));
    }
}

void MainWindow::Arrange(Orientation orientation)
{
    if (Orientation::Landscape == orientation)
    {
        this->ui->lbColor->setFixedSize(640, 480);
        this->ui->lbDepth->setFixedSize(640, 480);
    }
    else
    {
        this->ui->lbColor->setFixedSize(640, 640 * 4 / 3);
        this->ui->lbDepth->setFixedSize(640, 640 * 4 / 3);
    }

    this->panel->move(this->panel->x(), this->ui->lbColor->height());
    this->setFixedSize(this->width(), this->ui->lbColor->height() + this->panel->height() + this->ui->statusBar->height());
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

        case Error::DevicePermission:
            return QString("Device permission not granted");

        case Error::DeviceClosed:
            return QString("Device closed");

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
