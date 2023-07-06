#ifndef _DISPLAY_EPD_SSD1681_H_
#define _DISPLAY_EPD_SSD1681_H_

// FPC: FPC-8101
// Datasheet: www.waveshare.net/datasheet/SSD_PDF/SSD1681.pdf
#define xDot 200
#define yDot 200

extern void EpdSSD1681_DisFull(unsigned char *DisBuffer, unsigned char Label);

extern void EpdSSD1681_InitFull(void);

#endif

