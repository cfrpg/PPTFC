#include "tle493d.h"
#include "i2c.h"
#include "delay.h"
#include "stdio.h"

void TLE493DInit(u8 id,u8 addr)
{
	I2CInitPort(id);
	TLE493DReset(id,addr);
	//Set Config
	TLE493DWriteReg(id,addr,TLE493D_TRIG_NO,TLE493D_CONFIG,0b00101001);
	//TLE493DWriteReg(id,addr,TLE493D_TRIG_NO,TLE493D_MOD2,0b00011111);
	TLE493DWriteReg(id,addr,TLE493D_TRIG_NO,TLE493D_MOD1,0b00010011);	
	I2CStart(id);
	I2CSendByte(id,addr+1);
	I2CWaitAck(id);
	I2CSendByte(id,TLE493D_TRIG_NO|TLE493D_MOD1);
	I2CWaitAck(id);
	u8 b=I2CReadByte(id);
	//printf("0x%x\r\n",b);
}

void TLE493DReset(u8 id,u8 addr)
{
	I2CStart(id);
	I2CSendByte(id,0xFF);
	I2CStop(id);
	I2CStart(id);
	I2CSendByte(id,0xFF);
	I2CStop(id);
	I2CStart(id);
	I2CSendByte(id,0x00);
	I2CStop(id);
	I2CStart(id);
	I2CSendByte(id,0x00);
	I2CStop(id);
	delay_us(40);
}

void TLE493DReadout(u8 id,u8 addr,u8 n,s16 data[])
{
	u8 byte[6];
	u8 i;
	I2CStart(id);
//	I2CSendByte(id,addr+1);
//	I2CWaitAck(id);
//	I2CSendByte(id,TLE493D_TRIG_05|TLE493D_BX);
//	I2CWaitAck(id);
	I2CSendByte(id,addr+1);
	I2CWaitAck(id);
	byte[0]=I2CReadByte(id);
	for(i=1;i<6;i++)
	{
		I2CAck(id);
		byte[i]=I2CReadByte(id);
	}
	I2CNAck(id);
	I2CStop(id);
	data[0]=byte[0];
	data[0]<<=4;
	data[0]|=byte[4]>>4;
	data[1]=byte[1];
	data[1]<<=4;
	data[1]|=byte[4]&0x0F;
	data[2]=byte[2];
	data[2]<<=4;
	data[2]|=byte[5]&0x0F;
	data[3]=byte[3];
	data[3]<<=2;
	data[0]|=byte[5]>>6;
	for(i=0;i<4;i++)
	{
		if(byte[i]&0x80)
			data[i]|=0xF000;
	}
	
}

void TLE493DWriteReg(u8 id,u8 addr,u8 trig,u8 reg,u8 val)
{
	I2CStart(id);
	I2CSendByte(id,addr);
	I2CWaitAck(id);
	I2CSendByte(id,trig|reg);
	I2CWaitAck(id);
	I2CSendByte(id,val);
	I2CWaitAck(id);
	I2CStop(id);	
}
