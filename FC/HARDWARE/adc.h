#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

void ADCInit(void);
u16 ADCRead(u8 ch);
u16 ADCReadFiltered(u8 ch);
float ADCReadVol(u8 ch);
float ADCReadWithRef(void);
float ADCReadFilteredWithRef(u8 n);

#endif

