#ifndef __I2C_H__
#define __I2C_H__
sbit I2C_SCL=P2^2;
sbit I2C_SDA=P2^3;
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(unsigned char Byte);
unsigned char I2C_ReceiveByte(void);
void I2C_SendAck(unsigned char AckBit);
unsigned char I2C_ReceiveAck(void);

#endif
