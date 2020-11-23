#pragma once

#include "Dialog.h"
#include <cstdint>

class Panel : public Dialog
{
public:
    Panel();

    void Count(uint32_t count);

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