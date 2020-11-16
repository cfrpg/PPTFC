#ifndef __CPG_H
#define __CPG_H
#include "sys.h"

#define LED_FLASH_T 100

typedef struct
{
	double phase;
	double mcpg[3];
	double xcpg[3];
	double am;
	double xm;
	double m;
	double dphase;
} CPGMotor;

void CPGInit(void);
void CPGUpdateHover(void);
void CPGUpdateFlight(void);

#endif
