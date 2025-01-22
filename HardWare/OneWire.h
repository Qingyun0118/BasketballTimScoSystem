#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__
//Òý½Å¶¨Òå
sbit OneWire_DQ=P2^1;

unsigned char OneWire_Init(void);
void OneWire_SendBit(unsigned char Bit);
unsigned char OneWire_ReceiveBit(void);
void OneWire_SendByte(unsigned char Byte);
unsigned char OneWire_ReceiveByte(void);

#endif
