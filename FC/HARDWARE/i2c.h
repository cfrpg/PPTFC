#ifndef __I2C_H
#define __I2C_H
#include "sys.h"

#define I2C1_SCL_IN PBin(12)
#define I2C1_SCL_OUT PBout(12)
#define I2C1_SDA_IN PBin(13)
#define I2C1_SDA_OUT PBout(13)

#define I2C2_SCL_IN PBin(14)
#define I2C2_SCL_OUT PBout(14)
#define I2C2_SDA_IN PBin(15)
#define I2C2_SDA_OUT PBout(15)

#define I2C3_SCL_IN PBin(6)
#define I2C3_SCL_OUT PBout(6)
#define I2C3_SDA_IN PBin(7)
#define I2C3_SDA_OUT PBout(7)

#define I2C_WR 0
#define I2C_RD 1

void I2CInit(void);
void I2CInitPort(u8 id);
void I2CStart(u8 id);
void I2CStop(u8 id);
void I2CSendByte(u8 id,u8 b);
u8 I2CReadByte(u8 id);
u8 I2CWaitAck(u8 id);
void I2CAck(u8 id);
void I2CNAck(u8 id);
u8 I2CCheckDevice(u8 addr);



#endif