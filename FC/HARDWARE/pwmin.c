#include "pwm.h"
#include "stdio.h"
#include "parameter.h"

TIMCCState pwmState[6];
PPMState ppmState;

s32 pwmValues[8];
PWMParams pwmParams;
u8 pwmin_rev[8];

void pwmInitPwmMode(void);
void pwmInitPPMMode(void);

void TIM1IntPWM(void);
void TIM1IntPPM(void);

void PWMInInit(void)
{
	u8 i;
	u32 t=params.input_rev;
	for(i=0;i<8;i++)
	{
		pwmValues[i]=0;
		pwmin_rev[i]=t&1;
		t>>=1;
			
	}
	if(params.ppm_enabled)
		pwmInitPPMMode();
	else
		pwmInitPwmMode();
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

void pwmInitPwmMode(void)
{
	u8 i;
	
	GPIO_InitTypeDef gi;
	TIM_TimeBaseInitTypeDef  ti;
	NVIC_InitTypeDef ni;
	TIM_ICInitTypeDef tc;
	
	pwmParams.center=1500;
	pwmParams.max=2000;
	pwmParams.min=1000;
	pwmParams.range=1000;
	pwmParams.halfRange=500;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

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
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
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
	
	ni.NVIC_IRQChannel=TIM3_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=2;
	ni.NVIC_IRQChannelSubPriority=1;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	TIM_ITConfig(TIM3,TIM_IT_CC1|TIM_IT_CC2,ENABLE);
	TIM_ITConfig(TIM1,TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,ENABLE);
	TIM_Cmd(TIM1,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
	
	for(i=0;i<6;i++)
	{
		pwmState[i].STA=0;
		pwmState[i].tmp=0;
		pwmState[i].flag=0;
	}
	
	
}

void pwmInitPPMMode(void)
{
	u8 i;
	
	GPIO_InitTypeDef gi;
	TIM_TimeBaseInitTypeDef  ti;
	NVIC_InitTypeDef ni;
	TIM_ICInitTypeDef tc;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	pwmParams.center=1520;
	pwmParams.max=1940;
	pwmParams.min=1100;
	pwmParams.range=840;
	pwmParams.halfRange=420;
	
	gi.GPIO_Pin=GPIO_Pin_8;	
	gi.GPIO_Mode=GPIO_Mode_IPD;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gi);	
	ti.TIM_Period=0xFFFF;
	ti.TIM_Prescaler=71;
	ti.TIM_ClockDivision=TIM_CKD_DIV1;
	ti.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1,&ti);
		
	tc.TIM_ICPolarity=TIM_ICPolarity_Falling;
	tc.TIM_ICSelection=TIM_ICSelection_DirectTI;
	tc.TIM_ICPrescaler=TIM_ICPSC_DIV1;
	tc.TIM_ICFilter=0x00;
	tc.TIM_Channel=TIM_Channel_1;
	TIM_ICInit(TIM1,&tc);
	
	ni.NVIC_IRQChannel=TIM1_CC_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=2;
	ni.NVIC_IRQChannelSubPriority=1;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	
	ppmState.state=0;
	ppmState.currChannel=0;
	ppmState.currVal=0;
	ppmState.lastVal=0;
	
	TIM_ITConfig(TIM1,TIM_IT_CC1,ENABLE);
	TIM_Cmd(TIM1,ENABLE);
		
}

void TIM1IntPWM(void)
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
					if(pwmin_rev[i])
					{
						pwmValues[i]=3000-pwmState[i].value;
					}
					else
					{
						pwmValues[i]=pwmState[i].value;
					}
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

void TIM1IntPPM(void)
{
	u8 i;
	u16 it;
	u32 tmp;	
	
	//printf("TIMCC\r\n");
	
	
	it=2;
		
	if(TIM_GetITStatus(TIM1,it)!=RESET)
	{						
		TIM_ClearITPendingBit(TIM1,it);	
		ppmState.currVal=getCap(0);
		if(ppmState.currVal<ppmState.lastVal)
		{
			tmp=65535;
		}
		else
		{
			tmp=0;
		}
		ppmState.value=ppmState.currVal+tmp-ppmState.lastVal;
		ppmState.lastVal=ppmState.currVal;
		if(ppmState.state==0)
		{
			if(ppmState.value>2000)
			{
				ppmState.state=1;
				ppmState.currChannel=0;
			}
		}
		else
		{
			if(ppmState.currChannel<8)
				pwmValues[ppmState.currChannel]=ppmState.value;
			ppmState.currChannel++;
			if(ppmState.value>2000)
			{
				ppmState.state=1;
				ppmState.currChannel=0;
			}
		}							
	}			
			
	
}

void TIM1_CC_IRQHandler(void)
{
	//printf("TIMCC\r\n");
	if(params.ppm_enabled)
		TIM1IntPPM();
	else
		TIM1IntPWM();
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
					if(pwmin_rev[i])
					{
						pwmValues[i]=3000-pwmState[i].value;
					}
					else
					{
						pwmValues[i]=pwmState[i].value;
					}
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
