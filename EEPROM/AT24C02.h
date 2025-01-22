#ifndef __AT24C02_H__
#define __AT24C02_H__
#define AT24C02_ADDRESS		0xA0
void AT24C02_WriteByte(unsigned char WordAddress,Data);
unsigned char AT24C02_ReadByte(unsigned char WordAddress);

#endif
