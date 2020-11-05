#include "timer.h"

void MainClockInit()
{				
	TIM_TimeBaseInitTypeDef  ti;
	NVIC_InitTypeDef ni;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
	ti.TIM_Period = 9; //1ms
	ti.TIM_Prescaler =719;
	ti.TIM_ClockDivision = TIM_CKD_DIV1; 
	ti.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM4, &ti);
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); 
	TIM_Cmd(TIM4, ENABLE);
	ni.NVIC_IRQChannel = TIM4_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority = 1; 
	ni.NVIC_IRQChannelSubPriority = 3;
	ni.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&ni);	
}
