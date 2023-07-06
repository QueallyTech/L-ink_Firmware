#ifndef _DISPLAY_EPD_IL3829_H_
#define _DISPLAY_EPD_IL3829_H_

// FPC: WFC0000CZ07
#define xDot 200
#define yDot 200

extern void EpdIL3829_DisFull(unsigned char *DisBuffer, unsigned char Label);

extern void EpdIL3829_InitFull(void);

extern void EpdIL3829_InitPart(void);

#endif

