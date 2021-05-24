#include "depthview.h"
#include "ui_depthview.h"
#include "userpainter.h"
#include <qpainter.h>

#define COLOR_FAR       (0xFFFF0000)

using namespace std;
using namespace Topsens;

vector<uint32_t> DepthView::palette;

DepthView::DepthView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DepthView)
{
    ui->setupUi(this);

    this->connect(this, SIGNAL(Update()), this, SLOT(OnUpdate()));

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

DepthView::~DepthView()
{
    delete ui;
}

void DepthView::Draw(const uint16_t* depth, uint32_t w, uint32_t h, Orientation orientation)
{
    lock_guard<mutex> lock(this->lock);

    this->pixels.resize(w * h);
    for (size_t i = 0; i < this->pixels.size(); i++)
    {
        auto d = depth[i];
        this->pixels[i] = d < palette.size() ? palette[d] : COLOR_FAR;
    }

    this->pixmap = QPixmap::fromImage(QImage((uchar*)this->pixels.data(), w, h, QImage::Format_ARGB32));
    this->orientation = orientation;
}

void DepthView::Draw(const UsersFrame& users, Orientation orienation)
{
    lock_guard<mutex> lock(this->lock);
    QPainter painter(&this->pixmap);
    UserPainter::Paint(users, painter, this->pixmap.height() / 480.f, orientation);
}

void DepthView::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    this->ui->label->resize(this->width(), this->height());
}

void DepthView::OnUpdate()
{
    lock_guard<mutex> lock(this->lock);

    if (Orientation::Landscape == this->orientation || Orientation::Aerial == this->orientation)
    {
        this->ui->label->setPixmap(this->pixmap);
    }
    else if (Orientation::PortraitClockwise == this->orientation)
    {
        this->ui->label->setPixmap(this->pixmap.transformed(QTransform().rotate(90)));
    }
    else
    {
        this->ui->label->setPixmap(this->pixmap.transformed(QTransform().rotate(-90)));
    }
}
