#ifndef _DISPLAY_EPD_H_
#define _DISPLAY_EPD_H_

#include "IL3829.h"
#include "SSD1681.h"

extern void EpdInitFull();
extern void EpdDisFull(unsigned char *DisBuffer, unsigned char Label);

#endif
