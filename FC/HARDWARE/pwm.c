#include "pwm.h"
#include "parameter.h"

void PWMInit(void)
{
	GPIO_InitTypeDef gi;
	TIM_TimeBaseInitTypeDef  ti;
	TIM_OCInitTypeDef  to;	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	   
	gi.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	gi.GPIO_Mode = GPIO_Mode_AF_PP;
	gi.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gi);
 
	ti.TIM_Period = 19999;
	ti.TIM_Prescaler =71;
	ti.TIM_ClockDivision = 0;
	ti.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &ti);
	
	to.TIM_OCMode = TIM_OCMode_PWM2;
 	to.TIM_OutputState = TIM_OutputState_Enable;
	to.TIM_Pulse=0;
	to.TIM_OCPolarity = TIM_OCPolarity_Low;
	to.TIM_OCIdleState=TIM_OCIdleState_Set;
	TIM_OC1Init(TIM4, &to);
	TIM_OC2Init(TIM4, &to);
	TIM_OC3Init(TIM4, &to);
	TIM_OC4Init(TIM4, &to);
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
 
	TIM_Cmd(TIM4, ENABLE);	
	
	PWMInInit();
}

void PWMSet(float out[4])
{
	u8 i;
	if(out[0]<0)
		TIM_SetCompare1(TIM4,900);
	else
		TIM_SetCompare1(TIM4,(u16)(out[0]*1000+1000));
	if(out[1]<0)
		TIM_SetCompare2(TIM4,900);
	else
		TIM_SetCompare2(TIM4,(u16)(out[1]*1000+1000));
	if(out[2]<0)
		TIM_SetCompare3(TIM4,900);
	else
		TIM_SetCompare3(TIM4,(u16)(out[2]*1000+1000));
	if(out[3]<0)
		TIM_SetCompare4(TIM4,900);
	else
		TIM_SetCompare4(TIM4,(u16)(out[3]*1000+1000));	
}
