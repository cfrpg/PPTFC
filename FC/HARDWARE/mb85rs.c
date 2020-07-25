#include "mb85rs.h"

void FRAMInit(void)
{
	GPIO_InitTypeDef gi;
	SPI_InitTypeDef si;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	//Init SPI GPIO
	gi.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	gi.GPIO_Mode=GPIO_Mode_AF_PP;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);
	//Init common GPIO
	gi.GPIO_Pin=GPIO_Pin_12;
	gi.GPIO_Mode=GPIO_Mode_Out_PP;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);
	
	
	//Init SPI2
	si.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
	si.SPI_Mode=SPI_Mode_Master;
	si.SPI_DataSize=SPI_DataSize_8b;
	si.SPI_CPOL=SPI_CPOL_Low;
	si.SPI_CPHA=SPI_CPHA_1Edge;
	si.SPI_NSS=SPI_NSS_Soft;
	si.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_2;
	si.SPI_FirstBit=SPI_FirstBit_MSB;
	si.SPI_CRCPolynomial=7;
	SPI_Init(SPI2,&si);
	SPI_Cmd(SPI2,ENABLE);
	FRAM_CS=1;
}

void FRAMSendByte(u8 b)
{		
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);		
	SPI_I2S_SendData(SPI2,b);	
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)==SET);	
}

u8 FRAMReceiveByte(void)
{	
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);	
	SPI_I2S_SendData(SPI2,0xFF);
	//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);	
	//while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_BSY)==SET);	
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET);	
	u8 b= SPI_I2S_ReceiveData(SPI2);
	return b;
}

void FRAMWrite(u16 addr,u16 len,u8* data)
{
	u8 addl,addh;
	u16 i;
	//Cmd WREN
	FRAM_CS=0;
	FRAMSendByte(FRAM_WREN);
	FRAM_CS=1;
	addl=(u8)(addr&0x00FF);
	addh=(u8)((addr>>8)&0x00FF);
	//Cmd WRITE
	FRAM_CS=0;
	FRAMSendByte(FRAM_WRITE);
	FRAMSendByte(addh);
	FRAMSendByte(addl);
	for(i=0;i<len;i++)
	{
		FRAMSendByte(data[i]);
	}
	FRAM_CS=1;
	FRAM_CS=1;
	//Cmd WRDI
	FRAM_CS=0;
	FRAMSendByte(FRAM_WRDI);
	FRAM_CS=1;
}

void FRAMRead(u16 addr,u16 len,u8* data)
{
	u8 addl,addh;
	u16 i;
	addl=(u8)(addr&0x00FF);
	addh=(u8)((addr>>8)&0x00FF);
	//Cmd READ
	FRAM_CS=0;	
	FRAMSendByte(FRAM_READ);
	FRAMSendByte(addh);
	FRAMSendByte(addl);	
	SPI_I2S_ReceiveData(SPI2);
	for(i=0;i<len;i++)
	{
		data[i]=FRAMReceiveByte();
	}
	FRAM_CS=1;
}
