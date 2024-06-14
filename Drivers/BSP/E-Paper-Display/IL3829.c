#include "Epd_base.h"
#include "IL3829.h"

//static const unsigned char LUTDefault_full[31] = {
//    0x32,  // command
//    0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
//};

static const unsigned char LUTDefault_full[31] = {
	0x32,  // command
	0x66,0x66,0x44,0x66,0xAA,0x11,0x80,0x08,0x11,0x18,0x81,0x18,0x11,0x88,0x11,
	0x88,0x11,0x88,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x5F,0xAF,0xFF,0xFF,0x2F,0x00
};

static const unsigned char LUTDefault_part[31] = {
    0x32,  // command
    0x50, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char GDOControl[] = {0x01, (yDot - 1) % 256, (yDot - 1) / 256, 0x00}; // Driver Output control
static unsigned char softStart[] = {0x0c, 0xd7, 0xd6, 0x9d}; // Soft start Control
static unsigned char ramBypass[] = {0x21, 0x8f};        // Display update
static unsigned char MASequency[] = {0x22, 0xf0};        // clock
static unsigned char GDVol[] = {0x03, 0x66};    // Gate voltage +18V/-18V
static unsigned char SDVol[] = {0x04, 0x04};    // Source voltage +12V/-12V
static unsigned char VCOMVol[] = {0x2c, 0x7f};    // VCOM 7f
static unsigned char boostERFB[] = {0xf0, 0x1f};    // Source voltage +15V/-15V
static unsigned char dummyLine[] = {0x3a, 0x1a};    // 4 dummy line per gate
static unsigned char gateTime[] = {0x3b, 0x08};    // 2us per line
static unsigned char borderWavefrom[] = {0x3c, 0x33};    // Border
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

static void EpdIL3829_WriteCMD(unsigned char command)
{
    EPD_CS_0;
    EPD_DC_0;        // command write
    SpiWrite(command);
    EPD_CS_1;
}

static void EpdIL3829_WriteCMD_p1(unsigned char command, unsigned char para)
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

static void EpdIL3829_Write(unsigned char *value, unsigned char datalen)
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

static void EpdIL3829_WriteDispRam(unsigned char XSize, unsigned int YSize,
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

static void EpdIL3829_WriteDispRamMono(unsigned char XSize, unsigned int YSize,
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

static void EpdIL3829_PowerOn(void)
{
    EpdIL3829_WriteCMD_p1(0x22, 0xc0);
    EpdIL3829_WriteCMD(0x20);
    //EpdIL3829_WriteCMD(0xff);
}

static void EpdIL3829_SetRamArea(unsigned char Xstart, unsigned char Xend,
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
    EpdIL3829_Write(RamAreaX, sizeof(RamAreaX));
    EpdIL3829_Write(RamAreaY, sizeof(RamAreaY));
}

static void EpdIL3829_SetRamPointer(unsigned char addrX, unsigned char addrY, unsigned char addrY1)
{
    unsigned char RamPointerX[2];    // default (0,0)
    unsigned char RamPointerY[3];
    RamPointerX[0] = 0x4e;
    RamPointerX[1] = addrX;
    RamPointerY[0] = 0x4f;
    RamPointerY[1] = addrY;
    RamPointerY[2] = addrY1;

    EpdIL3829_Write(RamPointerX, sizeof(RamPointerX));
    EpdIL3829_Write(RamPointerY, sizeof(RamPointerY));
}

static void EpdIL3829_DispInit(void)
{

    EpdIL3829_Write(GDOControl, sizeof(GDOControl));    // Pannel configuration, Gate selection
    EpdIL3829_Write(softStart, sizeof(softStart));    // X decrease, Y decrease
	EpdIL3829_Write(VCOMVol,sizeof(VCOMVol));         // VCOM
    EpdIL3829_Write(dummyLine, sizeof(dummyLine));    // dummy line per gate
    EpdIL3829_Write(gateTime, sizeof(gateTime));        // Gage time setting
    EpdIL3829_Write(ramDataEntryMode, sizeof(ramDataEntryMode));    // X increase, Y decrease
    EpdIL3829_SetRamArea(0x00, (xDot - 1) / 8, (yDot - 1) % 256, (yDot - 1) / 256, 0x00,
                     0x00);    // X-source area,Y-gage area
    EpdIL3829_SetRamPointer(0x00, (yDot - 1) % 256, (yDot - 1) / 256);    // set ram
}

static void EpdIL3829_Init(void)
{
    EPD_RST_0;        // Module reset
    DriverDelay(100);
    EPD_RST_1;
    DriverDelay(100);

    EpdIL3829_DispInit();        // pannel configure
}

static void EpdIL3829_Update(void)
{
    EpdIL3829_WriteCMD_p1(0x22, 0xc7);
    EpdIL3829_WriteCMD(0x20);
    EpdIL3829_WriteCMD(0xff);
}

static void EpdIL3829_UpdatePart(void)
{
    EpdIL3829_WriteCMD_p1(0x22, 0x04);
    //EpdIL3829_WriteCMD_p1(0x22,0x08);
    EpdIL3829_WriteCMD(0x20);
    EpdIL3829_WriteCMD(0xff);
}

static void EpdIL3829_WirteLUT(unsigned char *LUTvalue, unsigned char Size)
{
    EpdIL3829_Write(LUTvalue, Size);
}


void EpdIL3829_InitFull(void)
{

    EpdIL3829_Init();            // display
    EpdIL3829_WirteLUT((unsigned char *) LUTDefault_full, sizeof(LUTDefault_full));

    EpdIL3829_PowerOn();
}

void EpdIL3829_InitPart(void)
{

    EpdIL3829_Init();            // display
    EpdIL3829_WirteLUT((unsigned char *) LUTDefault_part, sizeof(LUTDefault_part));
    EpdIL3829_PowerOn();
}

void EpdIL3829_DisFull(unsigned char *DisBuffer, unsigned char Label)
{
    EpdIL3829_SetRamPointer(0x00, (yDot - 1) % 256, (yDot - 1) / 256);    // set ram
    if (Label == 0)
    {
        EpdIL3829_WriteDispRamMono(xDot, yDot, 0xff);    // white
    } else
    {
        EpdIL3829_WriteDispRam(xDot, yDot, (unsigned char *) DisBuffer);    // white
    }
    EpdIL3829_Update();

}
