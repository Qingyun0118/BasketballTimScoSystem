#ifndef __MATRIX_LED_H__
#define __MATRIX_LED_H__
sbit RCK=P3^5;		//RCLK
sbit SCK=P3^6;		//SRCLK
sbit SER=P3^4;		//SER

#define MATRIX_LED_PORT		P0

void MatrixLED_Init();
void MatrixLED_ShowColumn(unsigned char Column,Data);


#endif
