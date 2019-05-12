#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

#define SW1 PBin(12)
#define SW2 PBin(13)
#define SW3 PBin(14)
#define SW4 PBin(15)

#define KEY PBin(5)

void KeyInit(void);
u8 KeyRead(void);	

#endif
