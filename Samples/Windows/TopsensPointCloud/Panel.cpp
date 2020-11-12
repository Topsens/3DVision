#include "Panel.h"
#include "CheckBox.h"
#include "resource.h"

Panel::Panel() : Dialog(IDD_PANEL)
{
}

bool Panel::OnCreated()
{
    ((CheckBox&)this->Item(IDC_COLOR)).Check();
    return Dialog::OnCreated();
}