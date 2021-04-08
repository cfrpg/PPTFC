#ifndef __GLIDELOCK_H
#define __GLIDELOCK_H

#include "sys.h"

#define GLInt_CH0 PBin(12)
#define GLInt_CH1 PBin(13)
#define RollTestCh 1
#define LAileZero 2000
#define RAileZero 1000

typedef struct
{
	s32 counter;
	s32 freqCounter;
	s32 peroid[3];
	s32 rollValue;
	float throVal;
	u16 pwmBackup[4];	
	u16 throTgt;	
	u8 mode;	
	u8 hallState[2];
	u8 GLEnabled;
	u8 rollEnabled;
	
} GlideLockState;

extern GlideLockState GLState;

void GLInit(void);

void GLUpdate(u8 glEnabled,u8 rollEnabled);


#endif