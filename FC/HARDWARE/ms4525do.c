#include "ms4525do.h"
#include "i2c.h"

void MS4525DOInit(u8 id)
{
	I2CInitPort(id);
	I2CStart(id);
	I2CSendByte(id,MS4525DO_ADDR);
	I2CWaitAck(id);
	I2CStop(id);
}

void MS4525DOGetRawData(u8 id,u16 data[2])
{
	u8 bytes[4];
	I2CStart(id);
	I2CSendByte(id,MS4525DO_ADDR);
	I2CWaitAck(id);
	bytes[0]=I2CReadByte(id);
	I2CAck(id);
	bytes[1]=I2CReadByte(id);
	I2CAck(id);
	bytes[2]=I2CReadByte(id);
	I2CAck(id);
	bytes[3]=I2CReadByte(id);
	I2CNAck(id);
	I2CStop(id);
	data[0]=bytes[0];
	data[0]<<=8;
	data[0]+=bytes[1];
	data[1]=bytes[2];
	data[1]<<=3;
	data[1]+=bytes[3]>>5;
}
		
void MS4525DOGetPackage(u8 id,u8 data[4])
{
	
}
