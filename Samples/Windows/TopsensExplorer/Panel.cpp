#include "Panel.h"
#include "resource.h"
#include "CheckBox.h"
#include "ComboBox.h"

Panel::Panel() : Dialog(IDD_PANEL)
{
}

void Panel::Enable()
{
    this->Item(IDC_SENSOR).Enable();
    this->Item(IDC_CRES).Enable();
    this->Item(IDC_DRES).Enable();
    this->Item(IDC_USERS_YES).Enable();
    this->Item(IDC_USERS_NO).Enable();
    this->Item(IDC_ALIGN_YES).Enable();
    this->Item(IDC_ALIGN_NO).Enable();
    this->Item(IDC_FLIP_YES).Enable();
    this->Item(IDC_FLIP_NO).Enable();
    this->Item(IDC_ORIENT_LAND).Enable();
    this->Item(IDC_ORIENT_CLOCK).Enable();
    this->Item(IDC_ORIENT_ANTIC).Enable();
    this->Item(IDC_RECORD_YES).Enable();
    this->Item(IDC_RECORD_NO).Enable();
    this->Item(IDC_GROUND_YES).Enable();
    this->Item(IDC_GROUND_NO).Enable();
    this->Item(IDC_START).Enable();
}

void Panel::Disable()
{
    this->Item(IDC_SENSOR).Disable();
    this->Item(IDC_CRES).Disable();
    this->Item(IDC_DRES).Disable();
    this->Item(IDC_USERS_YES).Disable();
    this->Item(IDC_USERS_NO).Disable();
    this->Item(IDC_ALIGN_YES).Disable();
    this->Item(IDC_ALIGN_NO).Disable();
    this->Item(IDC_FLIP_YES).Disable();
    this->Item(IDC_FLIP_NO).Disable();
    this->Item(IDC_ORIENT_LAND).Disable();
    this->Item(IDC_ORIENT_CLOCK).Disable();
    this->Item(IDC_ORIENT_ANTIC).Disable();
    this->Item(IDC_RECORD_YES).Disable();
    this->Item(IDC_RECORD_NO).Disable();
    this->Item(IDC_GROUND_YES).Disable();
    this->Item(IDC_GROUND_NO).Disable();
    this->Item(IDC_START).Disable();
}

int Panel::ColorResolution()
{
    return ((ComboBox&)this->Item(IDC_CRES)).Selection();
}

int Panel::DepthResolution()
{
    return ((ComboBox&)this->Item(IDC_DRES)).Selection();
}

int Panel::Orientation()
{
    if (((CheckBox&)this->Item(IDC_ORIENT_CLOCK)).IsChecked())
    {
        return 1;
    }
    else if (((CheckBox&)this->Item(IDC_ORIENT_ANTIC)).IsChecked())
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

bool Panel::GenerateUsers()
{
    return ((CheckBox&)this->Item(IDC_USERS_YES)).IsChecked();
}

bool Panel::PaintGround()
{
    return ((CheckBox&)this->Item(IDC_GROUND_YES)).IsChecked();
}

bool Panel::Align()
{
    return ((CheckBox&)this->Item(IDC_ALIGN_YES)).IsChecked();
}

bool Panel::Flip()
{
    return ((CheckBox&)this->Item(IDC_FLIP_YES)).IsChecked();
}

bool Panel::Record()
{
    return ((CheckBox&)this->Item(IDC_RECORD_YES)).IsChecked();
}

bool Panel::OnCreated()
{
    if (!Dialog::OnCreated())
    {
        return false;
    }

    auto& cres = (ComboBox&)this->Item(IDC_CRES);
    auto& dres = (ComboBox&)this->Item(IDC_DRES);

    cres.Clear();
    cres.Add(L"Disabled");
    cres.Add(L"QVGA");
    cres.Add(L"VGA");
    cres.Select(2);

    dres.Clear();
    dres.Add(L"Disabled");
    dres.Add(L"QVGA");
    dres.Add(L"VGA");
    dres.Select(2);

    ((CheckBox&)this->Item(IDC_USERS_YES)).Check();
    ((CheckBox&)this->Item(IDC_ALIGN_NO)).Check();
    ((CheckBox&)this->Item(IDC_FLIP_YES)).Check();
    ((CheckBox&)this->Item(IDC_ORIENT_LAND)).Check();
    ((CheckBox&)this->Item(IDC_RECORD_NO)).Check();
    ((CheckBox&)this->Item(IDC_GROUND_YES)).Check();

    return true;
}