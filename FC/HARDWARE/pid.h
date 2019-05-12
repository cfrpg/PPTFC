#ifndef __PID_H
#define __PID_H
#include "sys.h"

typedef struct
{
	float P;
	float I;
	float D;
	float err;
	float lasterr;
	float integral;
	float maxint;
	float derivate;
	float out;
	float maxout;
	float minout;
	float tau;
	float lpf;
} PIDData;

void PIDCalc(PIDData *pid,float err,float dt);

#endif
