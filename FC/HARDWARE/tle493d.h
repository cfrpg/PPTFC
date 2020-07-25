#ifndef __TLE493D_H
#define __TLE493D_H
#include "sys.h"

#define TLE493D_ADDR1 0x6A
#define TLE493D_ADDR2 0x44
#define TLE493D_ADDR3 0xF0
#define TLE493D_ADDR4 0x88

#define TLE493D_TRIG_NO		0x00
#define TLE493D_TRIG_WRITE 	0x20
#define TLE493D_TRIG_MSB	0x40
#define TLE493D_TRIG_05		0x80 

#define TLE493D_BX		0x00
#define TLE493D_BY		0x01
#define TLE493D_BZ		0x02
#define TLE493D_TEMP	0x03
#define TLE493D_BX2		0x04
#define TLE493D_TEMP2	0x05
#define TLE493D_DIAG	0x06
#define TLE493D_XL		0x07
#define TLE493D_XH		0x08
#define TLE493D_YL		0x09
#define TLE493D_YH		0x0A
#define TLE493D_ZL		0x0B
#define TLE493D_ZH		0x0C
#define TLE493D_WU		0x0D
#define TLE493D_TMODE	0x0E
#define TLE493D_TPHASE	0x0F
#define TLE493D_CONFIG	0x10
#define TLE493D_MOD1	0x11
#define TLE493D_MOD2	0x13
#define TLE493D_VER		0x16

void TLE493DInit(u8 id,u8 addr);
void TLE493DReset(u8 id,u8 addr);
void TLE493DReadout(u8 id,u8 addr,u8 n,s16 data[]);
void TLE493DWriteReg(u8 id,u8 addr,u8 trig,u8 reg,u8 val);


#endif
