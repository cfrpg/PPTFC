#include "sblink.h"
#include "string.h"
#include "stdio.h"

u8 sendBuff[SENDBUFFSIZE];
u8 linkSeq;
u8 recBuff[2][256];
u8 recBuffLen[2];
u8 currBuff;
u8 recState;
u8 sblinkReady;
u8 recSum;
u8 pkgLen;

void LinkInit(void)
{
	GPIO_InitTypeDef gi;
	USART_InitTypeDef ui;	
	DMA_InitTypeDef di;
	NVIC_InitTypeDef ni;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	
	gi.GPIO_Pin = GPIO_Pin_2;
	gi.GPIO_Mode = GPIO_Mode_AF_PP;
	gi.GPIO_Speed = GPIO_Speed_50MHz;		
	GPIO_Init(GPIOA,&gi);
	gi.GPIO_Pin=GPIO_Pin_3;
	gi.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&gi);

	ui.USART_BaudRate = 115200;
	ui.USART_WordLength = USART_WordLength_8b;
	ui.USART_StopBits = USART_StopBits_1;
	ui.USART_Parity = USART_Parity_No;
	ui.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	ui.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &ui);
	
	USART_Cmd(USART2, ENABLE);
	
	ni.NVIC_IRQChannel=USART2_IRQn;
	ni.NVIC_IRQChannelPreemptionPriority=2;
	ni.NVIC_IRQChannelSubPriority=1;
	ni.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&ni);
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	

	
	
	
	linkSeq=0;
	recState=0;
	currBuff=0;
	recBuffLen[0]=0;
	sblinkReady=0;
}

void LinkSendData(void* buff,u8 len)
{
	u8 i,sum=0;
	if(buff)
		memcpy(sendBuff,buff,len);
	sendBuff[2]=linkSeq;
	for(i=0;i<len;i++)
	{
		sum^=sendBuff[i];
	}
	sendBuff[len]=sum;
	
	linkSeq++;	
	//printf("%d\r\n",len);
//	while(DMA_GetCurrDataCounter(DMA1_Stream6));		
//	DMA_Cmd(DMA1_Stream6,DISABLE);	
//	while (DMA_GetCmdStatus(DMA1_Stream6) != DISABLE);	
//	DMA_SetCurrDataCounter(DMA1_Stream6,len+1);
//	DMA_Cmd(DMA1_Stream6,ENABLE);
	
}

u32 LinkPackTime()
{
	RTCReadTime();
	return (time.hour<<22)|(time.min<<16)|(time.sec<<10)|time.ms;
}

void DMA1_Stream6_IRQHandler(void)
{
	
}

void USART2_IRQHandler(void)
{
	
	u8 b;
	u8 curr=currBuff^1;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);		
		b=USART_ReceiveData(USART2);
		//printf("RX %d %x\r\n",recState,b);
		if(recState!=0)
		{
			if(recState<9)
				recSum^=b;
			recBuff[curr][recBuffLen[curr]]=b;
			recBuffLen[curr]++;
		}
		switch(recState)
		{
			case 0://get STX
				if(b==0xFC)
				{
					recBuff[curr][0]=b;
					recSum=0^b;
					recBuffLen[curr]=1;
					recState=1;
				}
			break;
			case 1://get LEN
				pkgLen=b;
				recState=2;
			break;
			case 2://get SEQ
				recState=3;
			break;
			case 3://get FUN
				recState=4;
			break;
			case 4://get TIM1
				recState=5;
			break;
			case 5://get TIM2
				recState=6;
			break;
			case 6://get TIM3
				recState=7;
			break;
			case 7://get TIM4
				recState=8;
			break;
			case 8:
				if(recBuffLen[curr]==pkgLen+8)
				{
					recState=9;
				}
			break;
			case 9:
				if(recSum==b)
				{
					currBuff^=1;
					sblinkReady=1;
				}
				else
				{
					printf("Bad checksum %d %d\r\n",recSum,b);
				}
				recState=0;
			break;
		}
		
	}
}
