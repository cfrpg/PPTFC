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
	s32 thro_min;
	s32 thro_homing;
	s32 dead_zone;
	float roll_delay;
	s32 roll_step_val;
	float roll_span;
	float ratio;
	float rev;
	float cpg_am;
	float yaw_scale;
	float motor_freq_max;
	u32 ppm_enabled;
	u32 update_rate;
	u32 input_rev;
	u32 flight_mode;
	u16 tailFlag;
} ParameterSet;

extern ParameterSet params;

u8 ParamRead(void);
u8 ParamWrite(void);
void ParamReset(void);
u8 ParamSet(u8 id,s32 v);
void ParamShow(void);

#endif
