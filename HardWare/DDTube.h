#ifndef __DDTUBE_H__
#define __DDTUBE_H__
#include <REGX52.H>

sbit LA=P3^3;
sbit LB=P2^4;


void DDTube_Scan(unsigned char Location,unsigned char Number);
void DDTube_Loop(void);
void DDTube_SetBuf(unsigned char Location,unsigned char Number);

#endif
