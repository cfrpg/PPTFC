#include "pwm.h"
#include "stdio.h"

TIMCCState pwmState[6];

void PWMInInit(void)
{
	u8 i;
	
	GPIO_InitTypeDef gi;
	TIM_TimeBaseInitTypeDef  ti;
	NVIC_InitTypeDef ni;
	TIM_ICInitTypeDef tc;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	gi.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;	
	gi.GPIO_Mode=GPIO_Mode_IPD;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gi);	
	ti.TIM_Period=0xFFFF;
	ti.TIM_Prescaler=71;
	ti.TIM_ClockDivision=TIM_CKD_DIV1;
	ti.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1,&ti);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	gi.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;	
	gi.GPIO_Mode=GPIO_Mode_IPD;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);	
	ti.TIM_Period=0xFFFF;
	ti.TIM_Prescaler=71;
	ti.TIM_ClockDivision=TIM_CKD_DIV1;
	ti.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&ti);
	for(i=0;i<4;i++)
	{
		tc.TIM_ICPolarity=TIM_ICPolarity_Rising;
		tc.TIM_ICSelection=TIM_ICSelection_DirectTI;
		tc.TIM_ICPrescaler=TIM_ICPSC_DIV1;
		tc.TIM_ICFilter=0x00;
		tc.TIM_Channel=i<<2;
		TIM_ICInit(TIM1,&tc);
		if(i<2)
			TIM_ICInit(TIM3,&tc);
	}	

	ni.NVIC_IRQChannel=TIM1_CC_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=2;
	ni.NVIC_IRQChannelSubPriority=1;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	TIM_ITConfig(TIM3,TIM_IT_CC1|TIM_IT_CC2,ENABLE);
	ni.NVIC_IRQChannel=TIM3_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=2;
	ni.NVIC_IRQChannelSubPriority=1;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	TIM_ITConfig(TIM1,TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,ENABLE);
	TIM_Cmd(TIM1,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
	
	for(i=0;i<6;i++)
	{
		pwmState[i].STA=0;
		pwmState[i].tmp=0;
	}
}

u16 getCap(u8 i)
{
	switch(i)
	{
		case 0:return TIM_GetCapture1(TIM1);
		case 1:return TIM_GetCapture2(TIM1);
		case 2:return TIM_GetCapture3(TIM1);
		case 3:return TIM_GetCapture4(TIM1);
		case 4:return TIM_GetCapture1(TIM3);
		case 5:return TIM_GetCapture2(TIM3);
	}
}

void setPolarity(u8 i,u16 p)
{
	switch(i)
	{
		case 0:TIM_OC1PolarityConfig(TIM1,p);return;
		case 1:TIM_OC2PolarityConfig(TIM1,p);return;
		case 2:TIM_OC3PolarityConfig(TIM1,p);return;
		case 3:TIM_OC4PolarityConfig(TIM1,p);return;
		case 4:TIM_OC1PolarityConfig(TIM3,p);return;
		case 5:TIM_OC2PolarityConfig(TIM3,p);return;
	}
}


void TIM1_CC_IRQHandler(void)
{
	u8 i;
	u16 it;
	u32 tmp;	
	
	//printf("TIMCC\r\n");
	
	for(i=0;i<4;i++)
	{
		it=2<<i;
		if((pwmState[i].STA & 0x80)==0)
		{			
			if(TIM_GetITStatus(TIM1,it)!=RESET)
			{						
				TIM_ClearITPendingBit(TIM1,it);				
				if(pwmState[i].STA&0x40)
				{
					
					pwmState[i].downv=getCap(i);
					if(pwmState[i].downv<pwmState[i].upv)
					{
						tmp=65535;
					}
					else
					{
						tmp=0;
					}
					pwmState[i].value=(pwmState[i].downv+tmp-pwmState[i].upv);
					pwmState[i].STA=0;
					setPolarity(i,TIM_ICPolarity_Rising);
				}
				else
				{
					
					pwmState[i].upv=getCap(i);
					pwmState[i].STA|=0x40;
					setPolarity(i,TIM_ICPolarity_Falling);
				}				
			}			
		}		
	}	
}

void TIM3_IRQHandler(void)
{
	u8 i;
	u16 it;
	u32 tmp;	
	
	//printf("TIMCC\r\n");
	
	for(i=4;i<6;i++)
	{
		it=2<<(i-4);
		if((pwmState[i].STA & 0x80)==0)
		{			
			if(TIM_GetITStatus(TIM3,it)!=RESET)
			{						
				TIM_ClearITPendingBit(TIM3,it);				
				if(pwmState[i].STA&0x40)
				{
					
					pwmState[i].downv=getCap(i);
					if(pwmState[i].downv<pwmState[i].upv)
					{
						tmp=65535;
					}
					else
					{
						tmp=0;
					}
					pwmState[i].value=(pwmState[i].downv+tmp-pwmState[i].upv);
					pwmState[i].STA=0;
					setPolarity(i,TIM_ICPolarity_Rising);
				}
				else
				{
					
					pwmState[i].upv=getCap(i);
					pwmState[i].STA|=0x40;
					setPolarity(i,TIM_ICPolarity_Falling);
				}				
			}			
		}		
	}	
}
