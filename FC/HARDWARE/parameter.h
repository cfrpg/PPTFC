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
	u32 pwm_rate;
	float scale_ratio;
	float man_adv;
	float freq_min;
	float freq_max;
	float am_max;
	float bias_max;
	float ratio;
	float dead_zone;
	float cpg_am;
	float yaw_scale;
	float motor_freq_max;
	u32 ppm_enabled;
	u32 update_rate;
	u16 tailFlag;
} ParameterSet;

extern ParameterSet params;

u8 ParamRead(void);
u8 ParamWrite(void);
void ParamReset(void);
u8 ParamSet(u8 id,s32 v);
void ParamShow(void);

#endif
