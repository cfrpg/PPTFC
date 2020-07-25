#ifndef __GPS_H
#define __GPS_H
#include "sys.h"

extern u8 gpsBuff[2][256];
extern u8 gpsBuffLen[2];
extern u8 gpsCurrBuff;
extern u8 gpsRecState;
extern u8 gpsReady;

void GPSInit(u16 baud);


#endif