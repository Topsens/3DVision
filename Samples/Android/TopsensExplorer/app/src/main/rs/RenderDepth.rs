#pragma version(1)
#pragma rs java_package_name(cn.topsens.explorer)

#define PALETTE_SIZE 0x1000

uchar4 palette[PALETTE_SIZE];

uchar4 RS_KERNEL render(short d)
{
    if (d >= PALETTE_SIZE)
    {
        d = PALETTE_SIZE - 1;
    }

    return palette[d];
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
}