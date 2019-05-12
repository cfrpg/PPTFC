#include "led.h"
#include "delay.h"

u8 _led_state;

void LEDInit(void)
{
	GPIO_InitTypeDef gi;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_13;
	gi.GPIO_Mode=GPIO_Mode_Out_PP;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&gi);
	_led_state=0;
}

void LEDSet(u8 v)
{
	if(v)
	{
		_led_state=0;		
	}
	else
	{
		_led_state=1;		
	}
	PCout(13)=_led_state;
}

void LEDFlip(void)
{
	_led_state^=1;
	PCout(13)=_led_state;
}

void LEDFlash(u8 t)
{
	PCout(13)=1;
	delay_ms(LED_FLASH_T);	
	while(t--)
	{
		PCout(13)=0;
		delay_ms(LED_FLASH_T);	
		PCout(13)=1;
		delay_ms(LED_FLASH_T);		
	}
	PCout(13)=1;
	delay_ms(LED_FLASH_T);	
	PCout(13)=_led_state;
}
