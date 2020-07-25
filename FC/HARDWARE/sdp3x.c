#include "sdp3x.h"
#include "i2c.h"

void Sdp3xInit(u8 id)
{
	I2CInitPort(id);
	
}
void Sdp3xSetMode(u8 id,u8 addr,u16 cmd)
{
	u8 h=cmd>>8;
	u8 l=cmd&0xFF;
	u8 b=0;
	I2CStart(id);
	I2CSendByte(id,addr);
	b=I2CWaitAck(id);
	I2CSendByte(id,h);
	I2CWaitAck(id);
	I2CSendByte(id,l);
	I2CWaitAck(id);
	I2CStop(id);
	printf("%d\r\n",b);
}

void Sdp3xReadOut(u8 id,u8 addr,u8 n,s16 data[3])
{
	u8 b;
	u8 crc;
	u8 i;
	I2CStart(id);
	I2CSendByte(id,addr+1);
	I2CWaitAck(id);
	for(i=0;i<n;i++)
	{
		if(i!=0)
			I2CAck(id);
		data[i]=I2CReadByte(id);
		I2CAck(id);
		data[i]<<=8;
		data[i]+=I2CReadByte(id);
		I2CAck(id);
		crc=I2CReadByte(id);		
	}
	I2CNAck(id);
	I2CStop(id);
}
