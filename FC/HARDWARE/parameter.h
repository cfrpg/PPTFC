#ifndef __PARAMETER_H
#define __PARAMETER_H
#include "sys.h"

#define FLASH_SIZE 64
#if FLASH_SIZE<256
  #define SECTOR_SIZE 1024   
#else 
  #define SECTOR_SIZE 2048
#endif

#define TGT_SECTOR_ADDR 0x0000FC00

typedef struct
{
	u16 headFlag;
	u16 pwm_cali_flag;
	u16 Rev;
	s32 pwm_min;
	s32 pwm_max;
	float curr_off;
	float curr_gain;
	float volt_gain;
	float pwr_p;
	float pwr_i;
	float pwr_d;
	float pwr_i_max;
	float curr_max;
	float power_max;
	float freq;
	float strength;
	u16 tailFlag;
} ParameterSet;

extern ParameterSet params;

u8 ParamRead(void);
u8 ParamWrite(void);
void ParamReset(void);
u8 ParamSet(u8 id,s32 v);
void ParamShow(void);

#endif
