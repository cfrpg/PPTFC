#include "key.h"
#include "delay.h"

void KeyInit(void)
{
	GPIO_InitTypeDef gi;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	
	
	gi.GPIO_Pin=GPIO_Pin_1;
	gi.GPIO_Mode=GPIO_Mode_IPU;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);
}	

u8 KeyRead(void)
{
	if(KEY==0)
	{
		delay_us(20);
		if(KEY==0)
		{
			return 1;
		}
	}
	return 0;
}
