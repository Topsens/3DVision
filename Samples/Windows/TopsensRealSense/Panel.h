#pragma once

#include "Dialog.h"
#include <Topsens.hpp>

#define noexcept
#include <librealsense2\rs.hpp>

class Panel : public Dialog
{
public:
    Panel();

    bool Flip() const;

    uint32_t Count() const;
    uint32_t XRes() const;
    uint32_t YRes() const;
    std::string Serial() const;

    Topsens::Orientation Orientation() const;

    void Enable();
    void Disable();

protected:
    bool OnCreated() override;

private:
    void Refresh();
    void OnRefresh();
    void OnDevice();

    static uint32_t GetWidth(std::wstring&);
    static uint32_t GetHeight(std::wstring&);

private:
    std::string serial;
    rs2::context context;
};