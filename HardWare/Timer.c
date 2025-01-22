#include <REGX52.H>
#include "Timer.h"
#include "DDTube.h"
#include "Buzzer.h"

//外部变量和函数声明
extern unsigned char count;
extern unsigned char minute;
extern char second;
extern unsigned char S24_time;   //24S规则
extern bit S24_flag;//是否使用24s规则
extern void SaveAT2402();

//定义变量
unsigned int TempCounter=1500;//每1.5s更新一次温度
bit sflag_updateTem=1;
bit sflag_SaveAT2402=0;

/**
  * @brief  定时器0初始化，1毫秒@12.000MHz
  * @param  无
  * @retval 无
  */
void Timer0_Init(void)
{
	TMOD |= 0x01;		//设置定时器0为工作方式1
	TH0 = 0x3C;		//设置定时初值
	TL0 = 0xB0;		//设置定时初值，50ms
	TF0 = 0;		//清除TF0标志
	TR0 = 0;		//定时器0关闭计时
	ET0=1;
	EA=1;
	PT0=0;
}


void Timer1_Init(void)
{
	TMOD |= 0x10;		//设置定时器1为工作方式1
	TH1 = 0xFC;		//设置定时初值
	TL1 = 0x18;		//设置定时初值，1ms
	TF1 = 0;		//清除TF1标志
	TR1 = 0;		//定时器1关闭计时
	ET1=1;
	EA=1;
	PT1=1;//定时器1高优先级
	TR1=1;//启动定时器1.定时器1只负责刷新显示数码管，刷新温度显示
}

//定时器0中断服务函数
void Timer0_Routine() interrupt 1
{
	
	if(count-- == 0)          //计时1秒
	{
		
        count = 20;
		second--;
		if(S24_flag)         //24秒规则启用
        {
            S24_time--;
			
        }
        // SaveAT2402();//每秒存入数据    这个地方直接调用存储函数导致中断服务函数时间太长了，导致后边数码管闪烁和温度传感器显示乱跳的bug，这里设置
										//一个标志位来实现，在外部实现存取数据
		sflag_SaveAT2402=~sflag_SaveAT2402;
	}

	TH0 = 0x3C;		//设置定时初值
	TL0 = 0xB0;		//设置定时初值，50ms

}


//定时器1中断服务函数
void Timer1_Routine() interrupt 3
{

	DDTube_Loop();//1ms调用一次数码管驱动函数
	TempCounter--;
	if(TempCounter<=0){
		TempCounter=1500;
		sflag_updateTem=~sflag_updateTem;
	}
	TH1 = 0xFC;		//设置定时初值
	TL1 = 0x18;		//设置定时初值，1ms
}