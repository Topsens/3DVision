#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <librealsense2/rs.hpp>
#include <ros/callback_queue.h>
#include <boost/core/null_deleter.hpp>

#define LANDSCAPE   (0)
#define CLOCKWISE   (1)
#define ANTICLOCK   (2)
#define COLOR_FAR   (0xFFFF0000);

using namespace std;
using namespace topsens;
using namespace sensor_msgs;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(this), node("~"), spinner(1)
{
    ui->setupUi(this);

    this->connect(&this->timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    this->timer.start(300);

    this->connect(this, SIGNAL(UpdateDepth()), this, SLOT(OnUpdateDepth()));
    this->connect(this, SIGNAL(Arrange(int, int, int)),this, SLOT(OnArrange(int, int, int)));

    this->palette.resize(0x1000);

    for (auto d = 0; d < 0x190; d++)
    {
        this->palette[d] = 0xFF000000;
    }

    for (auto d = 0x190; d < 0x400; d++)
    {
        int r = 0xFF - 0xFF * (d - 0x190) / (0x400 - 0x190);
        int g = 0;
        int b = 0xFF;

        this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
    }

    for (auto d = 0x400; d < 0x600; d++)
    {
        int r = 0;
        int g = 0xFF * (d - 0x400) / (0x600 - 0x400);
        int b = 0xFF;

        this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
    }

    for (auto d = 0x600; d < 0x800; d++)
    {
        int r = 0;
        int g = 0xFF;
        int b = 0xFF - 0xFF * (d - 0x600) / (0x800 - 0x600);

        this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
    }

    for (auto d = 0x800; d < 0xC00; d++)
    {
        int r = 0xFF * (d - 0x800) / (0xC00 - 0x800);
        int g = 0xFF;
        int b = 0;

        this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
    }

    for (auto d = 0xC00; d < 0x1000; d++)
    {
        int r = 0xFF;
        int g = 0xFF - 0xFF * (d - 0xC00) / (0x1000 - 0xC00);
        int b = 0;

        this->palette[d] = b | (g << 8) | (r << 16) | 0xFF000000;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent* e)
{
    QMainWindow::showEvent(e);

    string param;
    if (this->node.getParam("depth_frame", param) && !param.empty())
    {
        this->depth = this->node.subscribe(param, 1, &MainWindow::OnDepth, this);
    }
    else
    {
        ROS_WARN("Failed to subscribe depth");
    }

    if (this->node.getParam("users_frame", param) && !param.empty())
    {
        this->users = this->node.subscribe(param, 1, &MainWindow::OnUsers, this);
    }
    else
    {
        ROS_WARN("Failed to subscribe users");
    }

    if (this->node.getParam("orientation", param))
    {
        if ("landscape" == param)
        {
            this->orient = LANDSCAPE;
        }
        else if ("clockwise" == param)
        {
            this->orient = CLOCKWISE;
        }
        else if ("anticlock" == param)
        {
            this->orient = ANTICLOCK;
        }
        else
        {
            this->orient = LANDSCAPE;
            ROS_WARN("Unknown orientation. Use default: landscpae");
        }
    }
    else
    {
        this->orient = LANDSCAPE;
        ROS_WARN("Failed to get orientation. Use default: landscape");
    }

    this->spinner.start();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    this->spinner.stop();
    this->depth.shutdown();
    this->users.shutdown();
    QMainWindow::closeEvent(e);
}

void MainWindow::OnTimer()
{
    if (!ros::ok())
    {
        this->timer.stop();
        this->close();
    }
}

void MainWindow::OnArrange(int w, int h, int o)
{
    if (LANDSCAPE != o)
    {
        w = w ^ h;
        h = w ^ h;
        w = w ^ h;
    }

    this->ui->dlabel->resize(w, h);
    this->ui->dlabel->move(0, 0);

    this->ui->ulabel->resize(w, h);
    this->ui->ulabel->move(0, 0);

    this->setFixedSize(w, h);
}

void MainWindow::OnUpdateDepth()
{
    lock_guard<mutex> guard(this->dlock);

    if (this->pixmap.width())
    {
        this->ui->dlabel->setPixmap(LANDSCAPE == this->orient ? this->pixmap :
                                    CLOCKWISE == this->orient ? this->pixmap.transformed(QTransform().rotate(90)) :
                                                                this->pixmap.transformed(QTransform().rotate(-90)));
    }
}

void MainWindow::OnUsers(const UsersFrame::ConstPtr& users)
{
    this->ui->ulabel->Draw(users, this->orient);
    this->ui->ulabel->update();
}

void MainWindow::OnDepth(const Image::ConstPtr& depth)
{
    {
        lock_guard<mutex> guard(this->dlock);
        this->dpixels.resize(depth->width * depth->height);

        auto pixels = (uint16_t*)depth->data.data();
        for (size_t i = 0; i < this->dpixels.size(); i++)
        {
            auto d = pixels[i];
            this->dpixels[i] = d < this->palette.size() ? this->palette[d] : COLOR_FAR;
        }
        this->pixmap = QPixmap::fromImage(QImage((uchar*)this->dpixels.data(), depth->width, depth->height, QImage::Format_ARGB32));
    }
    this->Arrange(depth->width, depth->height, this->orient);
    this->UpdateDepth();
}