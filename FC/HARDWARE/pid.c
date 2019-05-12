#include "pid.h"

float PIDSat(float v,float min,float max)
{
	if(v>max)
		return max;
	if(v<min)
		return min;
	return v;
}

void PIDCalc(PIDData *pid,float err,float dt)
{
	float u,u0;
	
	pid->err=pid->lpf*pid->err+(1-pid->lpf)*err;

	pid->integral=pid->integral+dt/2*(pid->err+pid->lasterr);
	pid->integral=PIDSat(pid->integral,-pid->maxint,pid->maxint);
	pid->derivate=(2*pid->tau-dt)/(2*pid->tau+dt)*pid->derivate;
	pid->derivate+=(2/(2*pid->tau+dt))*(pid->err-pid->lasterr);
	
	u0=pid->P*pid->err+pid->I*pid->integral+pid->D*pid->derivate;
	u=PIDSat(u0,pid->minout,pid->maxout);
	
	if(pid->I>1e-5||pid->I<-1e-5)
		pid->I+=dt/pid->I*(u-u0);
	
	pid->out=u;
}
