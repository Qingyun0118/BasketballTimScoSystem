#include <REGX52.H>
#include "Delay.h"
#include "DDTube.h"
//数码管显示缓存区
unsigned char DDTube_Buf[9]={10,10,10,10,10,10,10,10,10};
//动态数码管段码表
unsigned char DDTubeTable[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00,0x40};

/**
  * @brief  设置显示缓存区
  * @param  Location 要设置的位置，范围：1~8
  * @param  Number 要设置的数字，范围：段码表索引范围
  * @retval 无
  */
void DDTube_SetBuf(unsigned char Location,Number)
{
	DDTube_Buf[Location]=Number;
}

/**
  * @brief  动态数码管显示
  * @param  Location 要显示的位置，范围：1~8
  * @param  Number 要显示的数字，范围：段码表索引范围
  * @retval 无
  */
void DDTube_Scan(unsigned char Location,unsigned char Number)
{
	P1=0x00;				//段码清0，消影
	switch(Location)		//位码输出
	{
		case 7:LA=1;LB=1;break;
		case 6:LA=0;LB=1;break;
		case 5:LA=1;LB=0;break;
		case 4:LA=0;LB=0;break;
	}
	P1=DDTubeTable[Number];	//段码输出
	//Delay(1);				//显示一段时间

}

/**
  * @brief  数码管驱动函数，在中断中调用
  * @param  无
  * @retval 无
  */
void DDTube_Loop(void)
{
	static unsigned char i=1;
	DDTube_Scan(i,DDTube_Buf[i]);
	i++;
	if(i>=9){i=1;}
}
