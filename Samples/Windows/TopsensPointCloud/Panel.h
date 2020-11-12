#pragma once

#include "Dialog.h"

class Panel : public Dialog
{
public:
    Panel();

protected:
    bool OnCreated() override;
};