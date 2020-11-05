#ifndef __LED_H
#define __LED_H
#include "sys.h"

#define LED_FLASH_T 100

void LEDInit(void);
void LEDSet(u8 v);
void LEDFlip(void);
void LEDFlash(u8 t);
void LEDSetRGB(u8 r,u8 g,u8 b);

#endif
