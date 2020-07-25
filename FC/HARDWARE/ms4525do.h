#ifndef __MS4525DO_H
#define __MS4525DO_H

#include "sys.h"

#define MS4525DO_ADDR 0x51
void MS4525DOInit(u8 id);
void MS4525DOGetRawData(u8 id,u16 data[2]);
void MS4525DOGetPackage(u8 id,u8 data[4]);


#endif