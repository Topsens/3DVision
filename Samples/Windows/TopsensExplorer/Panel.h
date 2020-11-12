#pragma once

#include "Dialog.h"

class Panel : public Dialog
{
public:
    Panel();

    void Enable();
    void Disable();

    int ColorResolution();
    int DepthResolution();
    int Orientation();

    bool GenerateUsers();
    bool PaintGround();
    bool Align();
    bool Flip();
    bool Record();

protected:
    bool OnCreated() override;
};