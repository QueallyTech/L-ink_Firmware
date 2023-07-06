#include "SSD1681.h"
#include "Epd_base.h"

static unsigned char GDOControl[] = {0x01, (yDot - 1) % 256, (yDot - 1) / 256, 0x00}; // Driver Output control
static unsigned char softStart[] = {0x0c, 0xd7, 0xd6, 0x9d}; // Soft start Control
static unsigned char ramBypass[] = {0x21, 0x8f};        // Display update
static unsigned char MASequency[] = {0x22, 0xf0};        // clock
static unsigned char GDVol[] = {0x03, 0x66};    // Gate voltage +18V/-18V
static unsigned char SDVol[] = {0x04, 0x04};    // Source voltage +12V/-12V
static unsigned char VCOMVol[] = {0x2c, 0x7f};    // VCOM 7c
static unsigned char boostERFB[] = {0xf0, 0x1f};    // Source voltage +15V/-15V
static unsigned char dummyLine[] = {0x3a, 0x1a};    // 4 dummy line per gate
static unsigned char gateTime[] = {0x3b, 0x08};    // 2us per line
static unsigned char borderWavefrom[] = {0x3c, 0x05};    // Border
static unsigned char ramDataEntryMode[] = {0x11, 0x01};    // Ram data entry mode

static unsigned char ReadBusy(void)
{
    unsigned long i = 0;


    for (i = 0; i < 400; i++)
    {
        if (isEPD_BUSY == EPD_BUSY_LEVEL)
        {
            return 1;
        }
        DriverDelay(10);
    }
    return 0;
}

static void EpdSSD1681_WriteCMD(unsigned char command)
{
    EPD_CS_0;
    EPD_DC_0;        // command write
    SpiWrite(command);
    EPD_CS_1;
}

static void EpdSSD1681_WriteCMD_p1(unsigned char command, unsigned char para)
{
    //while(isEPD_BUSY == 1);   // wait
    ReadBusy();

    EPD_CS_0;
    EPD_DC_0;        // command write
    SpiWrite(command);
    EPD_DC_1;        // command write
    SpiWrite(para);
    EPD_CS_1;
}

static void EpdSSD1681_Write(unsigned char *value, unsigned char datalen)
{
    unsigned char i = 0;
    unsigned char *ptemp;

    ptemp = value;

    EPD_CS_0;
    EPD_DC_0;        // command write

    SpiWrite(*ptemp);
    ptemp++;

    EPD_DC_1;        // data write

    for (i = 0; i < datalen - 1; i++)    // sub the command
    {
        SpiWrite(*ptemp);
        ptemp++;
    }

    EPD_CS_1;

}

static void EpdSSD1681_WriteDispRam(unsigned char XSize, unsigned int YSize,
                               unsigned char *Dispbuff)
{
    int i = 0, j = 0;

    if (XSize % 8 != 0)
    {
        XSize = XSize + (8 - XSize % 8);
    }
    XSize = XSize / 8;

    //while(isEPD_BUSY == 1);   //wait
    ReadBusy();

    EPD_CS_0;
    EPD_DC_0;        //command write
    SpiWrite(0x24);

    EPD_DC_1;        //data write
    for (i = 0; i < YSize; i++)
    {
        for (j = 0; j < XSize; j++)
        {
            SpiWrite(*Dispbuff);
            Dispbuff++;
        }
    }

    EPD_CS_1;
}

static void EpdSSD1681_WriteDispRamMono(unsigned char XSize, unsigned int YSize,
                                   unsigned char dispdata)
{
    int i = 0, j = 0;

    if (XSize % 8 != 0)
    {
        XSize = XSize + (8 - XSize % 8);
    }
    XSize = XSize / 8;
    //while(isEPD_BUSY == 1);   // wait
    ReadBusy();

    EPD_CS_0;
    EPD_DC_0;        // command write
    SpiWrite(0x24);

    EPD_DC_1;        // data write
    for (i = 0; i < YSize; i++)
    {
        for (j = 0; j < XSize; j++)
        {
            SpiWrite(dispdata);
        }
    }

    EPD_CS_1;
}

static void EpdSSD1681_PowerOn(void)
{
    EpdSSD1681_WriteCMD_p1(0x22, 0xf8);
    EpdSSD1681_WriteCMD(0x20);
    //EpdSSD1681_WriteCMD(0xff);
}

static void EpdSSD1681_SetRamArea(unsigned char Xstart, unsigned char Xend,
                             unsigned char Ystart, unsigned char Ystart1, unsigned char Yend, unsigned char Yend1)
{
    unsigned char RamAreaX[3];    // X start and end
    unsigned char RamAreaY[5];    // Y start and end
    RamAreaX[0] = 0x44;    // command
    RamAreaX[1] = Xstart;
    RamAreaX[2] = Xend;
    RamAreaY[0] = 0x45;    // command
    RamAreaY[1] = Ystart;
    RamAreaY[2] = Ystart1;
    RamAreaY[3] = Yend;
    RamAreaY[4] = Yend1;
    EpdSSD1681_Write(RamAreaX, sizeof(RamAreaX));
    EpdSSD1681_Write(RamAreaY, sizeof(RamAreaY));
}

static void EpdSSD1681_SetRamPointer(unsigned char addrX, unsigned char addrY, unsigned char addrY1)
{
    unsigned char RamPointerX[2];    // default (0,0)
    unsigned char RamPointerY[3];
    RamPointerX[0] = 0x4e;
    RamPointerX[1] = addrX;
    RamPointerY[0] = 0x4f;
    RamPointerY[1] = addrY;
    RamPointerY[2] = addrY1;

    EpdSSD1681_Write(RamPointerX, sizeof(RamPointerX));
    EpdSSD1681_Write(RamPointerY, sizeof(RamPointerY));
}

static void EpdSSD1681_DispInit(void)
{
    EpdSSD1681_Write(GDOControl, sizeof(GDOControl));    // Pannel configuration, Gate selection
    EpdSSD1681_Write(ramDataEntryMode, sizeof(ramDataEntryMode)); // X decrease, Y decrease
    EpdSSD1681_Write(borderWavefrom, sizeof(borderWavefrom)); // Set panel border
    EpdSSD1681_SetRamArea(0x00, (xDot - 1) / 8, (yDot - 1) % 256, (yDot - 1) / 256, 0x00,
                     0x00);    // X-source area,Y-gage area
    EpdSSD1681_SetRamPointer(0x00, (yDot - 1) % 256, (yDot - 1) / 256);    // set ram
}

static void EpdSSD1681_Init(void)
{
    EPD_RST_0;        // Module reset
    DriverDelay(100);
    EPD_RST_1;
    DriverDelay(100);

    EpdSSD1681_DispInit();        // pannel configure
}

static void EpdSSD1681_Update(void)
{
    EpdSSD1681_WriteCMD_p1(0x22, 0xf4);
    EpdSSD1681_WriteCMD(0x20);
    EpdSSD1681_WriteCMD(0xff);
}

static void EpdSSD1681_UpdatePart(void)
{
    EpdSSD1681_WriteCMD_p1(0x22, 0xfc);
    //EpdSSD1681_WriteCMD_p1(0x22,0x08);
    EpdSSD1681_WriteCMD(0x20);
    EpdSSD1681_WriteCMD(0xff);
}

static void EpdSSD1681_WirteLUT(unsigned char *LUTvalue, unsigned char Size)
{
    EpdSSD1681_Write(LUTvalue, Size);
}

void EpdSSD1681_InitFull(void)
{
    EpdSSD1681_Init();            // display
    EpdSSD1681_PowerOn();
}

void EpdSSD1681_DisFull(unsigned char *DisBuffer, unsigned char Label)
{
    EpdSSD1681_SetRamPointer(0x00, (yDot - 1) % 256, (yDot - 1) / 256);    // set ram
    if (Label == 0)
    {
        EpdSSD1681_WriteDispRamMono(xDot, yDot, 0xff);    // white
    } else
    {
        EpdSSD1681_WriteDispRam(xDot, yDot, (unsigned char *) DisBuffer);    // white
    }
    EpdSSD1681_Update();

}
