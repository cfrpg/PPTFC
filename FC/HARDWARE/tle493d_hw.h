#ifndef __TLE493DHW_H
#define __TLE493DHW_H
#include "sys.h"

void TLE493DHWInit(u8 addr);
void TLE493DHWReset(u8 addr);
void TLE493DHWReadout(u8 addr,u8 n,s16 data[]);
void TLE493DHWWriteReg(u8 addr,u8 trig,u8 reg,u8 val);

#endif