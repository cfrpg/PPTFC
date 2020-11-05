#include "cpg.h"
#include "parameter.h"
#include "math.h"
#include "pwm.h"

const double pi=3.1415926535897932384626;
const double twopi=6.28318530717958647692528;

CPGMotor cm[4];
double cpgdt;
double cpgpd[3];
double cpgThrust[3];
float cpgout[4];

double cpgm;
double cpgc;
double cpgk;
double cpgF;

double cpgc1;
double cpgk1;
double cpgF1;

double cpgfre;

double cpgam;

double cpgThrustT;
double cpgpdt;

double cpgdt2;		//dt*dt
double cpgdt2F;		//dt*dt*F
double cpgdt2k2m;	//dt*dt*k-2*m
double cpgmmdtc;	//m-0.5*dt*c
double cpgmpcdt;	//m+0.5*c*dt;

void CPGInit(void)
{
	u8 i;
	cpgdt=1.0/params.pwm_rate/params.scale_ratio;
	
	cpgam=20*params.scale_ratio;
	for(i=0;i<4;i++)
	{
		cm[i].am=0;
		//cm[i].dphase=cpgdt*2*pi*cpgfre;
		cm[i].m=0;
		cm[i].mcpg[0]=0;
		cm[i].mcpg[1]=0;
		cm[i].mcpg[2]=0;
		cm[i].xcpg[0]=0;
		cm[i].xcpg[1]=0;
		cm[i].xcpg[2]=0;
		cm[i].phase=0;
		cm[i].xm=0;
	}
	
	cpgm=1;
	cpgc=cpgam;
	cpgk=cpgam*cpgam/4;
	cpgF=cpgk;
	
	cpgc1=cpgam*params.ratio;
	cpgk1=cpgc1*cpgc1/4;
	cpgF1=cpgk1;
	
	cpgThrustT=0.9;
	cpgpdt=pi;
	
	cpgdt2=cpgdt*cpgdt;
	cpgdt2F=cpgdt2*cpgF;
	cpgdt2k2m=cpgdt2*cpgk-2*cpgm;
	cpgmmdtc=cpgm-0.5*cpgdt*cpgc;
	cpgmpcdt=cpgm+0.5*cpgdt*cpgc;
}

void CPGUpdate(void)
{
	s8 i,j;
	
	double roll=(pwmState[0].value-1500)/500.0;
	double pitch=(pwmState[1].value-1500)/500.0;
	double thro=(pwmState[2].value-1000)/1000.0;
	double yaw=(pwmState[3].value-1500)/500.0;
	double splitNormal=0.5*yaw+0.5;
	
	cpgThrust[0]=(cpgdt2*cpgF1*thro-(cpgdt2*cpgk1-2*cpgm)*cpgThrust[1]-(cpgm-0.5*cpgdt*cpgc1)*cpgThrust[2])/(cpgm+0.5*cpgc1*cpgdt);
	cpgfre=params.freq_min+(params.freq_max-params.freq_min)*cpgThrust[0];
		
	cpgpd[0]=(cpgdt2F*cpgpdt-cpgdt2k2m*cpgpd[1]-cpgmmdtc*cpgpd[2])/cpgmpcdt;
	
	cm[0].am= roll-pitch;
	cm[1].am=-roll-pitch;
	cm[2].am=-roll+pitch;
	cm[3].am= roll+pitch;
	cm[0].xm=-pitch*params.bias_max;
	cm[1].xm=pitch*params.bias_max;
	cm[2].xm=pitch*params.bias_max;
	cm[3].xm=-pitch*params.bias_max;
	
	for(i=0;i<4;i++)
	{
		cm[i].dphase=cpgdt*2*pi*cpgfre;
		cm[i].am=(((cm[i].am*0.5)*params.man_adv)+cpgThrustT)*params.am_max;
		cm[i].mcpg[0]=(cpgdt2F*cm[i].am-cpgdt2k2m*cm[i].mcpg[1]-cpgmmdtc*cm[i].mcpg[2])/cpgmpcdt;
		cm[i].xcpg[0]=(cpgdt2F*cm[i].xm-cpgdt2k2m*cm[i].xcpg[1]-cpgmmdtc*cm[i].xcpg[2])/cpgmpcdt;
	}
	
	if(fabs(yaw)>params.dead_zone)
	{		
		double sp;
		double spp;
		
		//M1,M3
		sp=cm[0].phase/twopi;
		spp=sp-(s32)sp;
		if(spp>=0&&spp<0.25)
		{
			cm[0].dphase=cpgdt*(pi*cpgfre/splitNormal);
			cm[2].dphase=cm[0].dphase;			
		}
		if(spp>=0.25&&spp<=0.75)
		{
			cm[0].dphase=cpgdt*(pi*cpgfre/(1-splitNormal));
			cm[2].dphase=cm[0].dphase;
		}
		if(spp>0.75&&spp<=1)
		{
			cm[0].dphase=cpgdt*(pi*cpgfre/splitNormal);
			cm[2].dphase=cm[0].dphase;			
		}
		
		//M2,M4
		sp=cm[1].phase/twopi;
		spp=sp-(s32)sp;
		if(spp>=0&&spp<0.25)
		{
			
			cm[1].dphase=cpgdt*(pi*cpgfre/(1-splitNormal));
			cm[3].dphase=cm[1].dphase;			
		}
		if(spp>=0.25&&spp<=0.75)
		{
			cm[1].dphase=cpgdt*(pi*cpgfre/splitNormal);
			cm[3].dphase=cm[1].dphase;	
		}
		if(spp>0.75&&spp<=1)
		{
			cm[1].dphase=cpgdt*(pi*cpgfre/(1-splitNormal));
			cm[3].dphase=cm[1].dphase;			
		}
	}
	
	cm[0].m=cm[0].mcpg[0]*sin(cm[0].phase)+cm[0].xcpg[0];
	cm[1].m=-cm[1].mcpg[0]*sin(cm[1].phase)+cm[1].xcpg[0];
	cm[2].m=-cm[2].mcpg[0]*sin(cm[2].phase+cpgpd[0])+cm[2].xcpg[0];
	cm[3].m=cm[3].mcpg[0]*sin(cm[3].phase+cpgpd[0])+cm[3].xcpg[0];
	
	for(j=2;j>0;j--)
	{
		for(i=0;i<4;i++)
		{
			cm[i].mcpg[j]=cm[i].mcpg[j-1];
			cm[i].xcpg[j]=cm[i].xcpg[j-1];
			cm[i].phase+=cm[i].dphase;
		}
		cpgThrust[j]=cpgThrust[j-1];
		cpgpd[j]=cpgpd[j-1];
	}
	
	for(i=0;i<4;i++)
	{
		cpgout[i]=(float)((cm[i].m+1)/2);
	}
	PWMSet(cpgout);
}