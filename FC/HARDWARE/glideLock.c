#include "glideLock.h"
#include "stdio.h"
#include "pwm.h"
#include "parameter.h"

u8 glstate;
u8 rollstate;
GlideLockState GLState;
s32 rollpos1,rollpos2;

void GLInit(void)
{
	//Init Exti
	GPIO_InitTypeDef gi;
	EXTI_InitTypeDef ei;
	NVIC_InitTypeDef ni;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	
	
	gi.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13;
	gi.GPIO_Mode=GPIO_Mode_IPU;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);
	
	ei.EXTI_Line=EXTI_Line12;
	ei.EXTI_Mode=EXTI_Mode_Interrupt;
	ei.EXTI_Trigger=EXTI_Trigger_Falling;
	ei.EXTI_LineCmd=ENABLE;
	EXTI_Init(&ei);
	ei.EXTI_Line=EXTI_Line13;
	EXTI_Init(&ei);
	
	ni.NVIC_IRQChannel=EXTI15_10_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=1;
	ni.NVIC_IRQChannelSubPriority=2;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	
	glstate=2;
	
	GLState.pwmBackup[0]=0;
	GLState.pwmBackup[1]=0;
	GLState.pwmBackup[2]=0;
	GLState.pwmBackup[3]=0;
	
	GLState.hallState[0]=0;
	GLState.hallState[1]=0;
	
	GLState.GLEnabled=0;
	GLState.rollEnabled=0;
	
	GLState.counter=0;
	GLState.freqCounter=0;
	
	rollstate=0;
}

void updateGlideLock(void)
{
	if(glstate==0)
	{
		//printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaagl0\r\n");
		if(pwmValues[0]<params.thro_min)
		{
			glstate=1;
			//GLState.hallState[0]=0;
		}
		else
		{
			GLState.throTgt=(u16)pwmValues[0];
		}
	}
	if(glstate==1)
	{		
		//printf("aaaaaaaaaaaaaaaaaagl1\r\n");
		if(pwmValues[0]>params.thro_min)
		{			
			glstate=0;
			GLState.throTgt=(u16)pwmValues[0];
		}
		else if(GLState.hallState[0])
		{
			//GLState.hallState[0]=0;
			glstate=2;
		}
		else
		{
			GLState.throTgt=(u16)params.thro_homing;
		}
	}
	if(glstate==2)
	{	
		//printf("gl2\r\n");
		if(pwmValues[0]>params.thro_min)
		{
			//printf("%d\r\n",pwmValues[0]);
			glstate=0;
			GLState.throTgt=(u16)pwmValues[0];
		}
		else if(GLState.mode)
		{
			if(GLInt_CH0)
			{
				//printf("a%d\r\n",pwmValues[0]);
				//GLState.hallState[0]=0;
				glstate=1;
				GLState.throTgt=(u16)params.thro_homing;
			}
		}
		else
		{
			GLState.throTgt=900;
		}
	}
}

void updateRollTest(void)
{
	if(rollstate==0)
	{
		//printf("rt0\r\n");
		if(GLState.hallState[0])
		{
			//GLState.hallState[0]=0;
			rollstate=1;
			GLState.counter=0;
			rollpos1=params.roll_delay*GLState.peroid[0];
			rollpos2=params.roll_span*GLState.peroid[0]+rollpos1;
		}
		else
		{
			GLState.pwmBackup[1]=LAileZero;
			GLState.pwmBackup[2]=RAileZero;
		}
	}
	if(rollstate==1)
	{
		//printf("rt1\r\n");
		if(GLState.counter>=rollpos1)
		{
			rollstate=2;
		}
		else
		{
			GLState.pwmBackup[1]=LAileZero;
			GLState.pwmBackup[2]=RAileZero;
		}
	}
	if(rollstate==2)
	{
		//printf("rt2\r\n");
		if(GLState.counter>rollpos2)
		{
			rollstate=3;
		}
		else
		{
			GLState.pwmBackup[1]=(u16)(LAileZero+GLState.rollValue);
			GLState.pwmBackup[2]=RAileZero;
		}
	}
	if(rollstate==3)
	{
		//printf("rt3\r\n");
		GLState.pwmBackup[1]=LAileZero;
		GLState.pwmBackup[2]=RAileZero;
	}
}

void GLUpdate(u8 glEnabled,u8 rollEnabled)
{
	//default:follow input
	GLState.pwmBackup[0]=(u16)pwmValues[0];
	GLState.pwmBackup[1]=(u16)pwmValues[1];
	GLState.pwmBackup[2]=(u16)pwmValues[2];
		
	if(glEnabled)
	{
		if(!GLState.GLEnabled)
		{		
			glstate=0;
		}
		updateGlideLock();
		GLState.throVal+=params.thro_slope;
		if(GLState.throVal>GLState.throTgt)
			GLState.throVal=GLState.throTgt;
		GLState.pwmBackup[0]=(u16)GLState.throVal;
	}
	else
	{
		GLState.pwmBackup[0]=(u16)pwmValues[0];
	}
	GLState.GLEnabled=glEnabled;
	
	if(rollEnabled)
	{
		if(!GLState.rollEnabled)
		{
			rollstate=0;
		}
		updateRollTest();
	}
	else
	{
		GLState.pwmBackup[1]=(u16)pwmValues[1];
		GLState.pwmBackup[2]=(u16)pwmValues[2];
	}
	GLState.rollEnabled=rollEnabled;
	GLState.hallState[0]=0;
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET)
	{
		
		EXTI_ClearITPendingBit(EXTI_Line12);
		GLState.hallState[0]=1;
		GLState.peroid[2]=GLState.peroid[1];
		GLState.peroid[1]=GLState.freqCounter;
		GLState.peroid[0]=(GLState.peroid[1]+GLState.peroid[2])>>1;		
		GLState.freqCounter=0;
		//printf("12\r\n");
	}
	if(EXTI_GetITStatus(EXTI_Line13)!=RESET)
	{
		
		EXTI_ClearITPendingBit(EXTI_Line13);
		GLState.hallState[1]=1;
		//printf("13\r\n");
	}

}
