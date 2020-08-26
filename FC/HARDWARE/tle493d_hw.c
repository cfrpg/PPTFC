#include "tle493d_hw.h"
#include "tle493d.h"
#include "delay.h"

void i2c1Write(u8 addr,u8 data)
{

}

void TLE493DHWInit(u8 addr)
{
	GPIO_InitTypeDef gi;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	gi.GPIO_Mode=GPIO_Mode_AF_OD;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);
	
	I2C_InitTypeDef ii;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	I2C_DeInit(I2C1);
	ii.I2C_Mode=I2C_Mode_I2C;
	ii.I2C_DutyCycle=I2C_DutyCycle_2;
	ii.I2C_OwnAddress1=addr;
	ii.I2C_Ack=I2C_Ack_Enable;
	ii.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	ii.I2C_ClockSpeed=400000;
	I2C_Init(I2C1,&ii);
	I2C_Cmd(I2C1,ENABLE);
	printf("reset\r\n");
	//TLE493DHWReset(addr);
	printf("ok\r\n");
	printf("config\r\n");
	TLE493DHWWriteReg(addr,TLE493D_TRIG_NO,TLE493D_CONFIG,0b00101001);
	//TLE493DWriteReg(id,addr,TLE493D_TRIG_NO,TLE493D_MOD2,0b00011111);
	TLE493DHWWriteReg(addr,TLE493D_TRIG_NO,TLE493D_MOD1,0b00010011);	
	printf("ok\r\n");
}

void TLE493DHWReset(u8 addr)
{
	//printf("1\r\n");
	I2C_AcknowledgeConfig(I2C1,DISABLE);
	//printf("2\r\n");
	I2C_GenerateSTART(I2C1,ENABLE);
	//printf("3\r\n");
	I2C_SendData(I2C1,0xFF); 
	//printf("4\r\n");
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	delay_us(50);
	(void)I2C1->SR2;
	I2C1->SR1=0;
	//printf("5\r\n");
	I2C_GenerateSTOP(I2C1,ENABLE);
	//printf("6\r\n");
	I2C_GenerateSTART(I2C1,ENABLE);
	I2C_SendData(I2C1,0xFF); 
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	delay_us(50);
	(void)I2C1->SR2;
	I2C1->SR1=0;
	I2C_GenerateSTOP(I2C1,ENABLE);
	I2C_GenerateSTART(I2C1,ENABLE);
	I2C_SendData(I2C1,0x00); 
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	delay_us(50);
	(void)I2C1->SR2;
	I2C1->SR1=0;
	I2C_GenerateSTOP(I2C1,ENABLE);
	I2C_GenerateSTART(I2C1,ENABLE);
	I2C_SendData(I2C1,0x00); 
	//while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	delay_us(50);
	(void)I2C1->SR2;
	I2C1->SR1=0;
	I2C_GenerateSTOP(I2C1,ENABLE);
	I2C_AcknowledgeConfig(I2C1,ENABLE);
	delay_us(40);
}

void TLE493DHWReadout(u8 addr,u8 n,s16 data[])
{
	
}

void TLE493DHWWriteReg(u8 addr,u8 trig,u8 reg,u8 val)
{
	//addr>>=1;
	printf("1\r\n");
	//I2C_AcknowledgeConfig(I2C1,ENABLE);
	printf("2\r\n");
	I2C_GenerateSTART(I2C1,ENABLE);	
	//printf("3\r\n");
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));//EV5
	printf("4\r\n");
	I2C_Send7bitAddress(I2C1,addr,I2C_Direction_Transmitter); 
	printf("5\r\n");
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); //EV6
	printf("6\r\n");
	I2C_SendData(I2C1,trig|reg); 
	printf("7\r\n");
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));//EV8
	printf("8\r\n");
	I2C_SendData(I2C1,val);
	printf("9\r\n");
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED)); //EV8
	printf("0\r\n");
	I2C_GenerateSTOP(I2C1,ENABLE); 
}
