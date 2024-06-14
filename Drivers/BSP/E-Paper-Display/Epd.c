#include "Epd.h"
#include "Epd_base.h"

//#define IL3829 // SSD1608
#define SSD1681

void EpdDisFull(unsigned char *DisBuffer, unsigned char Label) {
#ifdef IL3829
    EpdIL3829_DisFull(DisBuffer,Label);
#endif
#ifdef SSD1681
    EpdSSD1681_DisFull(DisBuffer,Label);
#endif
}

void EpdInitFull(void) {
#ifdef IL3829
    EpdIL3829_InitFull();
#endif
#ifdef SSD1681
    EpdSSD1681_InitFull();
#endif
}

void SpiDelay(unsigned char xrate)
{
    unsigned char i;
    while (xrate)
    {
        for (i = 0; i < 1; i++);
        xrate--;
    }
}


void DriverDelay(unsigned long xms)
{
    HAL_Delay(xms);
}


void SpiWrite(unsigned char value)
{
    unsigned char i;

    SpiDelay(1);
    for (i = 0; i < 8; i++)
    {
        EPD_CLK_0;
        SpiDelay(1);
        if (value & 0x80)
            EPD_MOSI_1;
        else
            EPD_MOSI_0;
        value = (value << 1);
        SpiDelay(1);
        EPD_CLK_1;
        SpiDelay(1);
    }
}