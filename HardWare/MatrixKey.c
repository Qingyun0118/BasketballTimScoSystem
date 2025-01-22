#include <REGX52.H>
#include "Delay.h"
#include "MatrixKey.h"
#include "Timer.h"
#include "Buzzer.h"


/**
  * @brief  矩阵键盘读取按键键码
  * @param  无
  * @retval KeyNumber 按下按键的键码值
			如果按键按下不放，程序会停留在此函数，松手的一瞬间，返回按键键码，没有按键按下时，返回-1
  */
unsigned char MatrixKey_GetKeyNum()
{
	char KeyNumber=-1;
	//进行下面的修改是为了减少矩阵键盘IO口的占用，
//	P3=0xFF;
//	P3_1=1;P3_2=1;P3_3=1;P3_4=1;P3_5=1;P3_6=1;P3_7=1;
//	P3_3=0;
//	if(P3_7==0){Delay(20);while(P3_7==0);Delay(20);KeyNumber=15;}
//	if(P3_6==0){Delay(20);while(P3_6==0);Delay(20);KeyNumber=14;}
//	if(P3_5==0){Delay(20);while(P3_5==0);Delay(20);KeyNumber=13;}
//	if(P3_4==0){Delay(20);while(P3_4==0);Delay(20);KeyNumber=12;}
	
//	P3=0xFF;
	P3_0=1;P3_1=1;P3_2=1;     P3_4=1;P3_5=1;P3_6=1;P3_7=1;//不使用P3_3，P3_3用于动态数码管的位码
	P3_2=0;
	if(P3_7==0){Delay(20);while(P3_7==0);Delay(20);KeyNumber=11;}
	if(P3_6==0){Delay(20);while(P3_6==0);Delay(20);KeyNumber=10;}
	if(P3_5==0){Delay(20);while(P3_5==0);Delay(20);KeyNumber=9;}
	if(P3_4==0){Delay(20);while(P3_4==0);Delay(20);KeyNumber=8;}
	
//	P3=0xFF;
	P3_0=1;P3_1=1;P3_2=1;     P3_4=1;P3_5=1;P3_6=1;P3_7=1;
	P3_1=0;
	if(P3_7==0){Delay(20);while(P3_7==0);Delay(20);KeyNumber=7;}
	if(P3_6==0){Delay(20);while(P3_6==0);Delay(20);KeyNumber=6;}
	if(P3_5==0){Delay(20);while(P3_5==0);Delay(20);KeyNumber=5;}
	if(P3_4==0){Delay(20);while(P3_4==0);Delay(20);KeyNumber=4;}
	
//	P3=0xFF;
	P3_0=1;P3_1=1;P3_2=1;     P3_4=1;P3_5=1;P3_6=1;P3_7=1;
	P3_0=0;
	if(P3_7==0){Delay(20);while(P3_7==0);Delay(20);KeyNumber=3;}
	if(P3_6==0){Delay(20);while(P3_6==0);Delay(20);KeyNumber=2;}
	if(P3_5==0){Delay(20);while(P3_5==0);Delay(20);KeyNumber=1;}
	if(P3_4==0){Delay(20);while(P3_4==0);Delay(20);KeyNumber=0;}
	
	return KeyNumber;
}


