#include "sys.h"
#include "usart.h"	  

#pragma import(__use_no_semihosting)             
               
struct __FILE 
{ 
	int handle;
}; 

FILE __stdout;       

void _sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);  
    USART3->DR = (u8) ch;      
	return ch;
} 
 
u8 USART_RX_BUF[USART_REC_LEN]; 
u16 USART_RX_STA=0;
void uart_init(u32 baud)
{    
    GPIO_InitTypeDef gi;
	USART_InitTypeDef ui;
	NVIC_InitTypeDef ni;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
 	USART_DeInit(USART3);	
    gi.GPIO_Pin = GPIO_Pin_10;
    gi.GPIO_Speed = GPIO_Speed_50MHz;
    gi.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &gi);    
    gi.GPIO_Pin = GPIO_Pin_11;
    gi.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &gi);
  
    ni.NVIC_IRQChannel = USART3_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=3 ;
	ni.NVIC_IRQChannelSubPriority = 3;
	ni.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&ni);

	ui.USART_BaudRate = baud;
	ui.USART_WordLength = USART_WordLength_8b;
	ui.USART_StopBits = USART_StopBits_1;
	ui.USART_Parity = USART_Parity_No;
	ui.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	ui.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &ui);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
    USART_Cmd(USART3, ENABLE);
}

void USART3_IRQHandler(void)
{
	u8 Res;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		Res =USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		
		if((USART_RX_STA&0x8000)==0)
		{
			if(USART_RX_STA&0x4000)
			{
				if(Res=='/')
				{
					USART_RX_STA|=0x8000;
				}
				else 
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if((USART_RX_STA&0X3FFF)>(USART_REC_LEN-1))USART_RX_STA=0;
				}
			}
			else
			{	
				if(Res=='*')USART_RX_STA=0x4000;				
			}
		}
		//printf("rx %c %d\r\n",Res,USART_RX_STA>>13);		
    } 
} 

