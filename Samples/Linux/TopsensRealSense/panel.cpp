#include "panel.h"
#include "ui_panel.h"

using namespace std;
using namespace rs2;
using namespace Topsens;

Panel::Panel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Panel)
{
    ui->setupUi(this);
    this->ui->stop->setEnabled(false);
    this->ui->land->setChecked(true);
    this->ui->flipNo->setChecked(true);

    this->connect(this, SIGNAL(Refresh()), this, SLOT(OnRefresh()));
    this->connect(this->ui->device, SIGNAL(currentIndexChanged(int)), this, SLOT(OnDevice()));
    this->context.set_devices_changed_callback([this](event_information){ this->Refresh(); });
    this->Refresh();
}

Panel::~Panel()
{
    delete ui;
}

uint32_t Panel::Width() const
{
    return GetWidth(this->ui->resolution->currentText());
}

uint32_t Panel::Height() const
{
    return GetHeight(this->ui->resolution->currentText());
}

string Panel::Serial() const
{
    return this->serial;
}

bool Panel::Flip() const
{
    return this->ui->flipYes->isChecked();
}

Orientation Panel::Orientation() const
{
    if (this->ui->land->isChecked())
    {
        return Topsens::Orientation::Landscape;
    }

    if (this->ui->clock->isChecked())
    {
        return Topsens::Orientation::PortraitClockwise;
    }

    if (this->ui->antic->isChecked())
    {
        return Topsens::Orientation::PortraitAntiClockwise;
    }

    return Topsens::Orientation::Aerial;
}

void Panel::Enable()
{
    this->ui->resolution->setEnabled(true);
    this->ui->device->setEnabled(true);
    this->ui->flipYes->setEnabled(true);
    this->ui->flipNo->setEnabled(true);
    this->ui->land->setEnabled(true);
    this->ui->clock->setEnabled(true);
    this->ui->antic->setEnabled(true);
    this->ui->aerial->setEnabled(true);
    this->ui->start->setEnabled(true);
    this->ui->stop->setEnabled(false);
}

void Panel::Disable()
{
    this->ui->resolution->setEnabled(false);
    this->ui->device->setEnabled(false);
    this->ui->flipYes->setEnabled(false);
    this->ui->flipNo->setEnabled(false);
    this->ui->land->setEnabled(false);
    this->ui->clock->setEnabled(false);
    this->ui->antic->setEnabled(false);
    this->ui->aerial->setEnabled(false);
    this->ui->start->setEnabled(false);
    this->ui->stop->setEnabled(true);
}

void Panel::OnRefresh()
{
    this->ui->device->clear();

    auto devices = this->context.query_devices();
    if (devices.size())
    {
        for (const auto& device : devices)
        {
            auto name = device.get_info(RS2_CAMERA_INFO_NAME);
            this->ui->device->addItem(name);

            auto serial = device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
            if (this->serial == serial)
            {
                this->ui->device->setCurrentIndex(this->ui->device->count() - 1);
            }
        }
    }
    else
    {
        this->ui->resolution->clear();
        this->serial.clear();
    }
}

void Panel::OnDevice()
{
    auto index = this->ui->device->currentIndex();
    if (index >= 0)
    {
        auto devices = this->context.query_devices();
        if (index < (int)devices.size())
        {
            const auto& device = devices[index];
            auto  serial = device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

            if (this->serial == serial)
            {
                return;
            }

            this->serial = serial;
            this->ui->resolution->clear();

            auto sensors = device.query_sensors();

            for (auto& sensor : sensors)
            {
                auto streams = sensor.get_stream_profiles();

                for (auto& stream : streams)
                {
                    if (RS2_STREAM_DEPTH == stream.stream_type() &&
                        RS2_FORMAT_Z16 == stream.format() &&
                        30 == stream.fps())
                    {
                        auto prof = (video_stream_profile)stream.as<video_stream_profile>();
                        this->ui->resolution->addItem(QString::fromStdString(to_string(prof.width()) + 'x' + to_string(prof.height())));

                        if (640 == prof.width() && 480 == prof.height())
                        {
                            this->ui->resolution->setCurrentIndex(this->ui->resolution->count() - 1);
                        }
                    }
                }
            }
        }
    }
}

void Panel::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    this->ui->hline->resize(this->width(), this->ui->hline->height());
}

int Panel::GetWidth(const QString& res)
{
    try
    {
        return stoi(res.toStdString());

    } catch (...)
    {
        return 0;
    }
}

int Panel::GetHeight(const QString& res)
{
    try
    {
        auto r = res.toStdString();
        return stoi(r.substr(r.find('x') + 1));

    } catch (...)
    {
        return 0;
    }
}
