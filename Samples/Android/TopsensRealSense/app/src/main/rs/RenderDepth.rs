#pragma version(1)
#pragma rs java_package_name(cn.topsens.realsense)

#define PALETTE_SIZE 0x1000

uchar4 palette[PALETTE_SIZE];
uchar4 red;

uint32_t width;
rs_allocation depth;

uchar4 RS_KERNEL render(uint32_t x, uint32_t y)
{
    ushort d = rsGetElementAt_uchar(depth, x * 2, y) | rsGetElementAt_uchar(depth, x * 2 + 1, y) << 8;
    return d < PALETTE_SIZE ? palette[d] : red;
}

uchar RS_KERNEL flip(uint32_t x, uint32_t y)
{
    return rsGetElementAt_uchar(depth, width - x - (1 - (x & 1)) * 2, y);
}

void init()
{
    for (short d = 0; d < 0x190; d++)
    {
        uchar4* c = &palette[d];
        c->r = 0;
        c->g = 0;
        c->b = 0;
        c->a = 0xFF;
    }

    for (short d = 0x190; d < 0x400; d++)
    {
        uchar4* c = &palette[d];
        c->r = 0xFF - 0xFF * (d - 0x190) / (0x400 - 0x190);
        c->g = 0;
        c->b = 0xFF;
        c->a = 0xFF;
    }

    for (short d = 0x400; d < 0x600; d++)
    {
        uchar4* c = &palette[d];
        c->r = 0;
        c->g = 0xFF * (d - 0x400) / (0x600 - 0x400);
        c->b = 0xFF;
        c->a = 0xFF;
    }

    for (short d = 0x600; d < 0x800; d++)
    {
        uchar4* c = &palette[d];
        c->r = 0;
        c->g = 0xFF;
        c->b = 0xFF - 0xFF * (d - 0x600) / (0x800 - 0x600);
        c->a = 0xFF;
    }

    for (short d = 0x800; d < 0xC00; d++)
    {
        uchar4* c = &palette[d];
        c->r = 0xFF * (d - 0x800) / (0xC00 - 0x800);
        c->g = 0xFF;
        c->b = 0;
        c->a = 0xFF;
    }

    for (short d = 0xC00; d < PALETTE_SIZE; d++)
    {
        uchar4* c = &palette[d];
        c->r = 0xFF;
        c->g = 0xFF - 0xFF * (d - 0xC00) / (PALETTE_SIZE - 0xC00);
        c->b = 0;
        c->a = 0xFF;
    }

    red.a = 0xFF;
    red.r = 0xFF;
    red.g = 0;
    red.b = 0;
}