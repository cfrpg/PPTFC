#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

typedef struct
{
	s32 value;
	u32 tmp;
	u16 upv;
	u16 downv;
	u8 STA;	
} TIMCCState;

extern TIMCCState pwmState[4];

void PWMInit(void);
void PWMInInit(void);
void PWMSet(float out[4]);


#endif
