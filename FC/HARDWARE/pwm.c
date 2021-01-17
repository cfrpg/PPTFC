#include "pwm.h"
#include "parameter.h"

void PWMInit(u32 freq)
{
	GPIO_InitTypeDef gi;
	TIM_TimeBaseInitTypeDef  ti;
	TIM_OCInitTypeDef  to;	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	   
	gi.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	gi.GPIO_Mode = GPIO_Mode_AF_PP;
	gi.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gi);
	if(freq>=500)
		freq=500;
	ti.TIM_Period = 1000000/freq-1;
	ti.TIM_Prescaler =71;
	ti.TIM_ClockDivision = 0;
	ti.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &ti);
	
	to.TIM_OCMode = TIM_OCMode_PWM2;
 	to.TIM_OutputState = TIM_OutputState_Enable;
	to.TIM_Pulse=0;
	to.TIM_OCPolarity = TIM_OCPolarity_Low;
	to.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OC1Init(TIM2, &to);
	TIM_OC2Init(TIM2, &to);
	TIM_OC3Init(TIM2, &to);
	TIM_OC4Init(TIM2, &to);
	
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
 
	TIM_Cmd(TIM2, ENABLE);	
	
	//PWMInInit();
}

void PWMSet(float out[4])
{
	if(out[0]<0)
		TIM_SetCompare1(TIM2,900);
	else
		TIM_SetCompare1(TIM2,(u16)(out[0]*1000+1000));
	if(out[1]<0)
		TIM_SetCompare2(TIM2,900);
	else
		TIM_SetCompare2(TIM2,(u16)(out[1]*1000+1000));
	if(out[2]<0)
		TIM_SetCompare3(TIM2,900);
	else		
		TIM_SetCompare3(TIM2,(u16)(out[2]*1000+1000));
	if(out[3]<0)
		TIM_SetCompare4(TIM2,900);
	else
		TIM_SetCompare4(TIM2,(u16)(out[3]*1000+1000));	
}

void PWMSetVal(u8 ch,u16 val)
{
	switch(ch)
	{
		case 0:
			TIM_SetCompare1(TIM2,val);
		break;
		case 1:
			TIM_SetCompare2(TIM2,val);
		break;
		case 2:
			TIM_SetCompare3(TIM2,val);
		break;
		case 3:
			TIM_SetCompare4(TIM2,val);
		break;
	}
}

void PWMLock(u16 v)
{
	TIM_SetCompare1(TIM2,900);
	TIM_SetCompare2(TIM2,1500);
	TIM_SetCompare3(TIM2,1500);
	TIM_SetCompare4(TIM2,v);
}
