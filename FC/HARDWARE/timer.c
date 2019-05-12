#include "timer.h"

void MainClockInit()
{				
	TIM_TimeBaseInitTypeDef  ti;
	NVIC_InitTypeDef ni;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
	ti.TIM_Period = 9; //10ms
	ti.TIM_Prescaler =7199;
	ti.TIM_ClockDivision = TIM_CKD_DIV1; 
	ti.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &ti);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 
	TIM_Cmd(TIM2, ENABLE);
	ni.NVIC_IRQChannel = TIM2_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority = 1; 
	ni.NVIC_IRQChannelSubPriority = 3;
	ni.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&ni);	
}
