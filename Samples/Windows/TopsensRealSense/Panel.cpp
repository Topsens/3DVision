#include "Panel.h"
#include "resource.h"
#include "CheckBox.h"
#include "ComboBox.h"

#define WM_REFRESH (WM_APP + 1)

using namespace std;
using namespace rs2;
using namespace Topsens;

Panel::Panel() : Dialog(IDD_PANEL)
{
}

bool Panel::Flip() const
{
    return ((CheckBox&)this->Item(IDC_FLIPY)).IsChecked();
}

uint32_t Panel::Count() const
{
    return (uint32_t)((ComboBox&)this->Item(IDC_DEVICE)).Count();
}

uint32_t Panel::XRes() const
{
    return GetWidth(((ComboBox&)this->Item(IDC_RESOLUTION)).Text());
}

uint32_t Panel::YRes() const
{
    return GetHeight(((ComboBox&)this->Item(IDC_RESOLUTION)).Text());
}

std::string Panel::Serial() const
{
    return this->serial;
}

Orientation Panel::Orientation() const
{
    if (((CheckBox&)this->Item(IDC_LAND)).IsChecked())
    {
        return Orientation::Landscape;
    }

    if (((CheckBox&)this->Item(IDC_CLOCK)).IsChecked())
    {
        return Orientation::PortraitClockwise;
    }

    return Orientation::PortraitAntiClockwise;
}

void Panel::Enable()
{
    this->Item(IDC_STOP).Disable();
    this->Item(IDC_START).Enable();
    this->Item(IDC_LAND).Enable();
    this->Item(IDC_CLOCK).Enable();
    this->Item(IDC_ANTIC).Enable();
    this->Item(IDC_FLIPY).Enable();
    this->Item(IDC_FLIPN).Enable();
    this->Item(IDC_DEVICE).Enable();
    this->Item(IDC_RESOLUTION).Enable();
}

void Panel::Disable()
{
    this->Item(IDC_RESOLUTION).Disable();
    this->Item(IDC_DEVICE).Disable();
    this->Item(IDC_START).Disable();
    this->Item(IDC_LAND).Disable();
    this->Item(IDC_CLOCK).Disable();
    this->Item(IDC_ANTIC).Disable();
    this->Item(IDC_FLIPY).Disable();
    this->Item(IDC_FLIPN).Disable();
    this->Item(IDC_STOP).Enable();
}

bool Panel::OnCreated()
{
    if (!Dialog::OnCreated())
    {
        return false;
    }

    ((CheckBox&)this->Item(IDC_FLIPN)).Check();
    ((CheckBox&)this->Item(IDC_LAND)).Check();

    this->context.set_devices_changed_callback([this](event_information){ this->Refresh(); });

    this->RegisterCommand(IDC_DEVICE, [this]{ if (CBN_SELCHANGE == HIWORD(this->wparam)) this->OnDevice(); });
    this->RegisterMessage(WM_REFRESH, [this]
    {
        this->OnRefresh();
        return 0;
    });

    this->Refresh();

    return true;
}

void Panel::Refresh()
{
    this->Post(WM_REFRESH);
}

void Panel::OnRefresh()
{
    auto& list = (ComboBox&)this->Item(IDC_DEVICE);
    list.Clear();

    auto devices = this->context.query_devices();
    if (devices.size())
    {
        for (auto& device : devices)
        {
            auto name = device.get_info(RS2_CAMERA_INFO_NAME);
            list.Add(wstring(name, name + strlen(name)));

            auto serial = device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
            if (this->serial == serial)
            {
                list.Select(list.Count() - 1);
            }
        }

        if (list.Selection() < 0)
        {
            list.Select(0);
        }
    }
    else
    {
        ((ComboBox&)this->Item(IDC_RESOLUTION)).Clear();
        this->serial.clear();
    }

    this->OnDevice();
}

void Panel::OnDevice()
{
    auto list = (ComboBox&)this->Item(IDC_DEVICE);
    if (list.Selection() < 0)
    {
        return;
    }

    auto devices = this->context.query_devices();
    if (list.Selection() < (int)devices.size())
    {
        auto& device = devices[list.Selection()];
        auto  serial = device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);

        if (this->serial == serial)
        {
            return;
        }

        this->serial = serial;

        auto resolution = (ComboBox&)this->Item(IDC_RESOLUTION);
        resolution.Clear();

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
                    resolution.Add(to_wstring(prof.width()) + L'x' + to_wstring(prof.height()));

                    if (640 == prof.width() && 480 == prof.height())
                    {
                        resolution.Select(resolution.Count() - 1);
                    }
                }
            }
        }
    }
}

uint32_t Panel::GetWidth(wstring& res)
{
    try
    {
        return stoul(res);

    } catch (...)
    {
        return 0;
    }
}

uint32_t Panel::GetHeight(wstring& res)
{
    try
    {
        return stoul(res.substr(res.find(L'x') + 1));

    } catch (...)
    {
        return 0;
    }
}