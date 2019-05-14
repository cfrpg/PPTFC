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
	u16 Rev;
	float phi_k;
	float phi_b;
	float phi_p;
	float phi_i;
	float phi_d;
	float yaw_p;
	float aile;
	float elev;
	float phi;
	float yaw_scale;
	u16 tailFlag;
} ParameterSet;

extern ParameterSet params;

u8 ParamRead(void);
u8 ParamWrite(void);
void ParamReset(void);
u8 ParamSet(u8 id,s32 v);
void ParamShow(void);

#endif
