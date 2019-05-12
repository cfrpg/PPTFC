#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

void ADCInit(void);
u16 ADCRead(void);
u16 ADCReadFiltered(void);
float ADCReadVol(void);
float ADCReadWithRef(void);
float ADCReadFilteredWithRef(u8 n);

#endif
