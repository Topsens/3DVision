#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <ros/callback_queue.h>
#include <limits>

#define COLOR_FAR   (0xFFFF0000)
#define COLOR_GND   (0xFF808080)

#define LANDSCAPE   (0)
#define CLOCKWISE   (1)
#define ANTICLOCK   (2)

using namespace std;
using namespace topsens;
using namespace sensor_msgs;

const float fNaN = numeric_limits<float>::quiet_NaN();

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow()), timer(this), node("~"), spinner(1)
{
    this->ui->setupUi(this);

    this->connect(&timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    this->timer.start(300);

    this->connect(this, SIGNAL(UpdateColor()), this, SLOT(OnUpdateColor()));
    this->connect(this, SIGNAL(UpdateDepth()), this, SLOT(OnUpdateDepth()));

    this->ground.X = fNaN;
    this->ground.Y = fNaN;
    this->ground.Z = fNaN;
    this->ground.W = fNaN;

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
    delete this->ui;
}

void MainWindow::showEvent(QShowEvent* e)
{
    QMainWindow::showEvent(e);

    string param;
    if (this->node.getParam("depth_camera", param))
    {
        this->dinfo = this->node.subscribe(param, 1, &MainWindow::OnDInfo, this);
    }
    else
    {
        ROS_WARN("Failed to get param <depth_camera>");
    }

    if (this->node.getParam("color_frame", param))
    {
        this->color = this->node.subscribe(param, 1, &MainWindow::OnColor, this);
    }
    else
    {
        ROS_WARN("Failed to get param <color_frame>");
    }

    this->orient = LANDSCAPE;
    if (this->node.getParam("orientation", param))
    {
        transform(param.begin(), param.end(), param.begin(), [](uint8_t c){ return tolower(c); });

        if ("clockwise" == param)
        {
            this->orient = CLOCKWISE;
        }
        else if ("anticlock" == param)
        {
            this->orient = ANTICLOCK;
        }
    }
    else
    {
        ROS_WARN("Failed to get param <orientation>. Use default: landscape");
    }

    this->Arrange();
    this->spinner.start();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    this->spinner.stop();
    this->color.shutdown();
    this->depth.shutdown();
    this->users.shutdown();
    QMainWindow::closeEvent(e);
}

void MainWindow::OnUpdateColor()
{
    lock_guard<mutex> guard(this->clock);

    if (!this->cpixmap.width())
    {
        return;
    }
    
    auto w = this->ui->lbColor->width();
    auto h = this->ui->lbColor->height();

    if (LANDSCAPE == this->orient)
    {
        this->ui->lbColor->setPixmap(this->cpixmap.scaled(w, h));
    }
    else
    {
        float scale  = (float)h / this->cpixmap.width();
        float rotate = (CLOCKWISE == this->orient ? 90.f : -90.f);
        this->ui->lbColor->setPixmap(this->cpixmap.transformed(QTransform().rotate(rotate).scale(scale, scale)));
    }
}

void MainWindow::OnUpdateDepth()
{
    auto w = this->ui->lbDepth->width();
    auto h = this->ui->lbDepth->height();

    lock_guard<mutex> guard(this->dlock);

    if (this->utime && this->utime != this->dtime)
    {
        return;
    }

    if (!this->dpixmap.width())
    {
        return;
    }

    QPainter painter(&this->dpixmap);
    this->upainter.OnPaint(painter, this->dpixmap.width() / 640.f);

    if (LANDSCAPE == this->orient)
    {
        this->ui->lbDepth->setPixmap(this->dpixmap.scaled(w, h));
    }
    else
    {
        float scale  = (float) h / this->dpixmap.width();
        float rotate = (CLOCKWISE == this->orient ? 90.f : -90.f);
        this->ui->lbDepth->setPixmap(this->dpixmap.transformed(QTransform().rotate(rotate).scale(scale, scale)));
    }
}

void MainWindow::OnTimer()
{
    if (!ros::ok())
    {
        this->timer.stop();
        this->close();
    }
}

void MainWindow::OnDInfo(const CameraInfo::ConstPtr& camera)
{
    this->dinfo.shutdown();

    this->fx = 1.f / (float)camera->K.elems[0];
    this->fy = 1.f / (float)camera->K.elems[4];
    this->cx = (float)camera->K.elems[2];
    this->cy = (float)camera->K.elems[5];

    this->dtime = 0;
    this->utime = 0;

    string param;
    if (this->node.getParam("depth_frame", param))
    {
        this->depth = this->node.subscribe(param, 1, &MainWindow::OnDepth, this);
    }
    else
    {
        ROS_WARN("Failed to get param <depth_frame>");
    }
    
    if (this->node.getParam("users_frame", param))
    {
        this->users = this->node.subscribe(param, 1, &MainWindow::OnUsers, this);
    }
    else
    {
        ROS_WARN("Failed to get param <users_frame>");
    }
}

void MainWindow::OnColor(const Image::ConstPtr& frame)
{
    {
        lock_guard<mutex> guard(this->clock);

        this->cpixels = (vector<uint32_t>&)frame->data;
        this->cpixmap = QPixmap::fromImage(QImage((uchar*)this->cpixels.data(), frame->width, frame->height, QImage::Format_ARGB32));
    }
    this->UpdateColor();
}

void MainWindow::OnDepth(const Image::ConstPtr& frame)
{
    {
        lock_guard<mutex> guard(this->dlock);

        this->dpixels.resize(frame->width * frame->height);

        this->ToPointCloud(frame);

        auto depth = (uint16_t*)frame->data.data();
        for (uint32_t i = 0; i < frame->width * frame->height; i++)
        {
            auto d = depth[i];
            if (d < this->palette.size())
            {
                this->dpixels[i] = IsGround(this->cloud[i], this->ground) ? COLOR_GND : this->palette[d];
            }
            else
            {
                this->dpixels[i] = COLOR_FAR;
            }
        }

        this->dpixmap = QPixmap::fromImage(QImage((uchar*)this->dpixels.data(), frame->width, frame->height, QImage::Format_ARGB32));
        this->dtime = (uint64_t)frame->header.stamp.sec * 1000 + (uint64_t)frame->header.stamp.nsec / 1000000;
    }
    this->UpdateDepth();
}

void MainWindow::OnUsers(const UsersFrame::ConstPtr& frame)
{
    this->upainter.Draw(frame);

    lock_guard<mutex> guard(this->dlock);
    this->utime = (uint64_t)frame->header.stamp.sec * 1000 + (uint64_t)frame->header.stamp.nsec / 1000000;
    this->ground = frame->GroundPlane;
    this->UpdateDepth();
}

void MainWindow::Arrange()
{
    int w = 640;
    int h = 480;

    if (LANDSCAPE != this->orient)
    {
        w = 480;
        h = 640;
    }

    this->ui->lbColor->resize(w, h);
    this->ui->lbDepth->resize(w, h);
    this->ui->lbColor->move(0, 0);
    this->ui->lbDepth->move(w, 0);

    this->setFixedSize(w * 2, h);
}

void MainWindow::ToPointCloud(const Image::ConstPtr& frame)
{
    this->cloud.resize(frame->width * frame->height);

    auto depth = (uint16_t*)frame->data.data();
    for (uint32_t i = 0, idx = 0; i < frame->height; i++)
    {
        auto fi = (float)i;

        for (uint32_t j = 0; j < frame->width; j++, idx++)
        {
            auto fj = (float)j;
            auto  d = depth[idx];
            auto& c = this->cloud[idx];

            if (d)
            {
                c.Z = d * 0.001f;
                c.X = c.Z * (fj - this->cx) * this->fx;
                c.Y = c.Z * (this->cy - fi) * this->fy;
            }
            else
            {
                c.X = fNaN;
                c.Y = fNaN;
                c.Z = fNaN;
            }
        }
    }
}