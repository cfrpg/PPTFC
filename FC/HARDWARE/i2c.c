#include "i2c.h"
#include "delay.h"

u8 i2c_init_flag;

void i2cDelay(void)
{
	//delay_us(1);
	//u8 i=2;
	//while(i--);
	__NOP();
}

void i2cStart1(void)
{
	I2C1_SDA_OUT=1;
	I2C1_SDA_OUT=1;
	I2C1_SCL_OUT=1;
	i2cDelay();
	I2C1_SDA_OUT=0;
	i2cDelay();
	I2C1_SCL_OUT=0;
	i2cDelay();
}

void i2cStop1(void)
{
	I2C1_SDA_OUT=0;
	I2C1_SCL_OUT=1;
	i2cDelay();
	I2C1_SDA_OUT=1;
	i2cDelay();
}

void i2cStart2(void)
{
	I2C2_SDA_OUT=1;
	I2C2_SDA_OUT=1;
	I2C2_SCL_OUT=1;
	i2cDelay();
	I2C2_SDA_OUT=0;
	i2cDelay();
	I2C2_SCL_OUT=0;
	i2cDelay();
}

void i2cStop2(void)
{
	I2C2_SDA_OUT=0;
	I2C2_SCL_OUT=1;
	i2cDelay();
	I2C2_SDA_OUT=1;
	i2cDelay();
}

void i2cStart3(void)
{
	I2C3_SDA_OUT=1;
	I2C3_SDA_OUT=1;
	I2C3_SCL_OUT=1;
	i2cDelay();
	I2C3_SDA_OUT=0;
	i2cDelay();
	I2C3_SCL_OUT=0;
	i2cDelay();
}

void i2cStop3(void)
{
	I2C3_SDA_OUT=0;
	I2C3_SCL_OUT=1;
	i2cDelay();
	I2C3_SDA_OUT=1;
	i2cDelay();
}

void i2cInit1()
{
	if(i2c_init_flag&0x01)
		return;
	GPIO_InitTypeDef gi;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13;
	gi.GPIO_Mode=GPIO_Mode_Out_OD;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);
	
	i2cStop1();
	i2c_init_flag|=0x01;
}

void i2cInit2()
{
	if(i2c_init_flag&0x02)
		return;
	GPIO_InitTypeDef gi;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_14|GPIO_Pin_15;
	gi.GPIO_Mode=GPIO_Mode_Out_OD;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);
	
	i2cStop2();
	i2c_init_flag|=0x02;
}

void i2cInit3()
{
	if(i2c_init_flag&0x04)
		return;
	GPIO_InitTypeDef gi;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	gi.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	gi.GPIO_Mode=GPIO_Mode_Out_OD;
	gi.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&gi);
	
	i2cStop3();
	i2c_init_flag|=0x04;
}

void i2cSendByte1(u8 b)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(b&0x80)
			I2C1_SDA_OUT=1;
		else
			I2C1_SDA_OUT=0;
		i2cDelay();
		I2C1_SCL_OUT=1;
		i2cDelay();
		I2C1_SCL_OUT=0;
		if(i==7)
			I2C1_SDA_OUT=1;
		b<<=1;
		i2cDelay();
	}
}

u8 i2cReadByte1(void)
{
	u8 i,res;
	res=0;
	for(i=0;i<8;i++)
	{
		res<<=1;
		I2C1_SCL_OUT=1;
		i2cDelay();
		if(I2C1_SDA_IN)
			res|=1;
		I2C1_SCL_OUT=0;
		i2cDelay();
	}
	return res;
}

u8 i2cWaitAck1(void)
{
	u8 res;
	I2C1_SDA_OUT=1;
	i2cDelay();
	I2C1_SCL_OUT=1;
	i2cDelay();
	if(I2C1_SDA_IN)
		res=1;
	else
		res=0;
	I2C1_SCL_OUT=0;
	i2cDelay();
	return res;
}

void i2cAck1(void)
{
	I2C1_SDA_OUT=0;
	i2cDelay();
	I2C1_SCL_OUT=1;
	i2cDelay();
	I2C1_SCL_OUT=0;
	i2cDelay();
	I2C1_SDA_OUT=1;
	i2cDelay();
}

void i2cNAck1(void)
{
	I2C1_SDA_OUT=1;
	i2cDelay();
	I2C1_SCL_OUT=1;
	i2cDelay();
	I2C1_SCL_OUT=0;
	i2cDelay();
}

void i2cSendByte2(u8 b)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(b&0x80)
			I2C2_SDA_OUT=1;
		else
			I2C2_SDA_OUT=0;
		i2cDelay();
		I2C2_SCL_OUT=1;
		i2cDelay();
		I2C2_SCL_OUT=0;
		if(i==7)
			I2C2_SDA_OUT=1;
		b<<=1;
		i2cDelay();
	}
}

u8 i2cReadByte2(void)
{
	u8 i,res;
	res=0;
	for(i=0;i<8;i++)
	{
		res<<=1;
		I2C2_SCL_OUT=1;
		i2cDelay();
		if(I2C2_SDA_IN)
			res|=1;
		I2C2_SCL_OUT=0;
		i2cDelay();
	}
	return res;
}

u8 i2cWaitAck2(void)
{
	u8 res;
	I2C2_SDA_OUT=1;
	i2cDelay();
	I2C2_SCL_OUT=1;
	i2cDelay();
	if(I2C2_SDA_IN)
		res=1;
	else
		res=0;
	I2C2_SCL_OUT=0;
	i2cDelay();
	return res;
}

void i2cAck2(void)
{
	I2C2_SDA_OUT=0;
	i2cDelay();
	I2C2_SCL_OUT=1;
	i2cDelay();
	I2C2_SCL_OUT=0;
	i2cDelay();
	I2C2_SDA_OUT=1;
	i2cDelay();
}

void i2cNAck2(void)
{
	I2C2_SDA_OUT=1;
	i2cDelay();
	I2C2_SCL_OUT=1;
	i2cDelay();
	I2C2_SCL_OUT=0;
	i2cDelay();
}

void i2cSendByte3(u8 b)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		if(b&0x80)
			I2C3_SDA_OUT=1;
		else
			I2C3_SDA_OUT=0;
		//i2cDelay();
		I2C3_SCL_OUT=1;
		i2cDelay();
		I2C3_SCL_OUT=0;
		if(i==7)
			I2C3_SDA_OUT=1;
		b<<=1;
		//i2cDelay();
	}
}

u8 i2cReadByte3(void)
{
	u8 i,res;
	res=0;
	for(i=0;i<8;i++)
	{
		res<<=1;
		I2C3_SCL_OUT=1;
		i2cDelay();
		//__NOP();
		if(I2C3_SDA_IN)
			res|=1;
		I2C3_SCL_OUT=0;
		i2cDelay();
		i2cDelay();
	}
	return res;
}

u8 i2cWaitAck3(void)
{
	u8 res;
	I2C3_SDA_OUT=1;
	i2cDelay();
	I2C3_SCL_OUT=1;
	i2cDelay();
	if(I2C3_SDA_IN)
		res=1;
	else
		res=0;
	I2C3_SCL_OUT=0;
	i2cDelay();
	return res;
}

void i2cAck3(void)
{
	I2C3_SDA_OUT=0;
	i2cDelay();
	I2C3_SCL_OUT=1;
	i2cDelay();
	I2C3_SCL_OUT=0;
	i2cDelay();
	I2C3_SDA_OUT=1;
	i2cDelay();
}

void i2cNAck3(void)
{
	I2C3_SDA_OUT=1;
	i2cDelay();
	I2C3_SCL_OUT=1;
	i2cDelay();
	I2C3_SCL_OUT=0;
	i2cDelay();
}

void I2CInitPort(u8 id)
{
	if(id==1)
		i2cInit1();
	if(id==2)
		i2cInit2();
	if(id==3)
		i2cInit3();
}

void I2CInit()
{
	i2c_init_flag=0;
}


void I2CStart(u8 id)
{
	if(id==1)
		i2cStart1();
	if(id==2)
		i2cStart2();
	if(id==3)
		i2cStart3();
}

void I2CStop(u8 id)
{
	if(id==1)
		i2cStop1();
	if(id==2)
		i2cStop2();
	if(id==3)
		i2cStop3();
}

void I2CSendByte(u8 id,u8 b)
{
	if(id==1)
		i2cSendByte1(b);
	if(id==2)
		i2cSendByte2(b);
	if(id==3)
		i2cSendByte3(b);
}

u8 I2CReadByte(u8 id)
{
	if(id==1)
		return i2cReadByte1();
	if(id==2)
		return i2cReadByte2();
	if(id==3)
		return i2cReadByte3();
}

u8 I2CWaitAck(u8 id)
{
	if(id==1)
		return i2cWaitAck1();
	if(id==2)
		return i2cWaitAck2();
	if(id==3)
		return i2cWaitAck3();
}

void I2CAck(u8 id)
{
	if(id==1)
		i2cAck1();
	if(id==2)
		i2cAck2();
	if(id==3)
		i2cAck3();
}

void I2CNAck(u8 id)
{
	if(id==1)
		i2cNAck1();
	if(id==2)
		i2cNAck2();
	if(id==3)
		i2cNAck3();	
}

u8 I2CCheckDevice(u8 addr)
{
	
}

