#include <REGX52.H>
#include "MatrixKey.h"	//矩阵键盘
#include "LCD1602.h"	//LCD显示
#include "DS18B20.h"	//温度传感器
#include "Delay.h"		//软件延时
#include "Timer.h"		//定时器
#include "DDTube.h"		//动态数码管
#include "Buzzer.h"		//蜂鸣器
#include "AT24C02.h"	//EEPROM断电保护


/** 项目名称：篮球定时计分系统(Basketball Timing Scoring System, BTSsystem)
  * Created by 韦继成 张琨皓
  * Finished at 2024.12.23
  * 使用功能介绍：
  * 键盘功能, 0-比赛开始/继续     1-比赛暂停     2-增加比赛节数（加时）   3-切换比赛模式
			  4-A队进1分球       5-A队进2分球   6-A队进3分球             7-断电恢复
			  8-B队进1分球       9-B队进2分球   10-B队进3分球   
    0:比赛开始，哨声响，总计时开始，24s计时开始；
    3:切换比赛模式：NBA or FIBA，每节比赛时间分别为12，10min，切换模式会进行一次初始化
    7:断电保护，每秒钟系统会将相关数据存入EEPROM中，断电后重新上电，进入系统后如需恢复到断电前现场即按7，
      若不需要恢复可直接按0开始比赛
	每节比赛结束后，定时器回到初始时间并暂停计时，等待下一节比赛开始；2节比赛后（即中场结束）会自动交换客场，主场
	比分位置
  */


//*************** -定义常量start- ******************
#define NBA_MODE 0
#define FIBA_MODE 1
//**************** -定义常量end- *******************

//*************** -声明变量start- *****************************************
unsigned char SetTime;//比赛初始时间（分钟）
float T;//温度
unsigned char S24_time;   //24S规则
bit S24_flag;//是否使用24s规则

unsigned char Score[6];        //用于存储处理的比赛分数
unsigned char Time_Data[4];    //数组用于存放处理的设定时间

//声明外部变量
extern unsigned int TempCounter;//温度更新计数器
extern bit sflag_updateTem;//温度更新标志位

//下面的变量需要存放在EEPROM，进行断电保护
unsigned char xg,xgs1,xgs2,xgs3,xh,xhs1,xhs2,xhs3;//比分和主场客场位置记录
unsigned char mode=NBA_MODE;//比赛模式默认为NBA
unsigned char G_Score;//客场Guest分数
unsigned char H_Score;//主场Home分数
unsigned char match;//节数
unsigned char count;//50ms个数
unsigned char minute;//分钟数
char second;//秒数


//*************** -声明变量 end- *****************************************

//*************** -函数声明start- *****************************************
void SysInit();//初始化，变量定义在此完成
void GetTempreture();//获取环境温度
void KeyControl();//按键控制函数
void SaveAT2402();//断电保护
void RecoverAT2402();//断电恢复
void SwapLocation(unsigned char *x,unsigned char *y);//交换位置数据
void StartMenu();//开始菜单
void UpdateDisplay();//更新显示
//*************** -函数声明end- *****************************************

void main(){
	DS18B20_ConvertT();//上电先进行一次温度转换，防止第一次数据读取错误
	Delay(1000);//等待转换完成
	SysInit();
	StartMenu();
	
	while(1){
		KeyControl();
		UpdateDisplay();

	}
}

//*************** -初始化函数定义start- *****************************************

//初始化
void SysInit()
{
	switch(mode){
		case NBA_MODE:
			SetTime=12;
			break;
		case FIBA_MODE:
			SetTime=1;
			break;
		default:
			break;
	}
	
	S24_time=24;
	S24_flag=0;
	minute=SetTime;
	second=0;
	count = 20;//50ms个数
	xg=1;xgs1=3;xgs2=4;xgs3=5;xh=11;xhs1=7;xhs2=8;xhs3=9;
	G_Score=0;H_Score=0;match=1;
	
	LCD_Init();//LCD初始化
	Timer0_Init();//初始化定时器
	Timer1_Init();
	TR0=0;//定时器0停止
}

//*************** -初始化函数定义end- *****************************************

//*************** -键盘控制函数定义start- ****************************************
/**
* @brief  定义键盘功能，0-比赛开始/继续    1-比赛暂停     2-增加比赛节数    3-切换比赛模式
					   4-A队进1分球       5-A队进2分球   6-A队进3分球      7-断电恢复
					   8-B队进1分球       9-B队进2分球   10-B队3分球     
  * @param  无
  * @retval 无
			
  */
void KeyControl()
{
	switch(MatrixKey_GetKeyNum())
	{
		case 0:            //比赛开始/继续
            TR0 = 1;             //定时器开关打开，开始计时
			S24_flag=1;	//24S规则启用
            Buzzer_Time(600);//比赛开始/继续的哨声
            break;

		case 1:            //比赛暂停
            TR0 = 0;          //定时器开关打开，停止计时
			Buzzer_Time(400);//比赛暂停的哨声
            break;

		case 2:             //手动增加比赛节数，总计时器和24s复位
            if((match >= MATCH) && (minute == 0)&&(G_Score==H_Score))     //判断比赛结束是否超过四节以及此节比赛是否结束
            {
                TR0 = 0;                   //定时器停止
                match++;                   //赛程节数加一
                minute = 5;                //加时赛赛程时间设置 5分钟
                second = 0;
                S24_time = 24;     //24秒恢复初值
				S24_flag=0;
            }
            break;
			
		case 3:             //比赛模式切换
			   
			mode+=1;
			if(mode>1) mode=0;
			SysInit();
            break;
		
		 //每队加分后，24秒恢复初值
		case 4:                         //客场队伍加1分
			G_Score += 1;
			S24_time = 24;
            break;  

		case 5:                         //客场队伍加2分
			G_Score += 2;
			S24_time = 24;
            break;    
                                 
		case 6:                         //客场队伍加3分
			G_Score += 3;
			S24_time = 24;
            break;
		
		case 7:                         //从EEPROM中读入数据，断电恢复
			RecoverAT2402();
            break;
		
		case 8:                         //主场队伍加1分
			H_Score += 1;
			S24_time = 24;
            break;

		case 9:                         //主场队伍加2分
			H_Score += 2;
			S24_time = 24;

            break;

		case 10:                         //主场队伍加3分
			H_Score += 3;
			S24_time = 24;
            break;
		

		default:
			break;	
	}
	
}
//*************** -键盘控制函数定义end- ****************************************

//*************** -获取环境温度函数定义start- **********************************
//获取环境温度
void GetTempreture()
{
	DS18B20_ConvertT();	//转换温度
	T=DS18B20_ReadT();	//读取温度
	if(T<0)				//如果温度小于0
	{
		LCD_ShowChar(2,8,'-');	//显示负号
		T=-T;			//将温度变为正数
	}
	else				//如果温度大于等于0
	{
		LCD_ShowChar(2,8,'+');	//显示正号
	}
	LCD_ShowNum(2,9,T,2);		//显示温度整数部分
	LCD_ShowChar(2,11,'.');		//显示小数点
	LCD_ShowNum(2,12,(unsigned long)(T*100)%100,1);//显示温度小数部分
}
//*************** -获取环境温度函数定义end- ***************************************

//*************** -断电保护和恢复函数定义start- *****************************************
//断电保护
void SaveAT2402()
{
	//存入比分
	AT24C02_WriteByte(0,G_Score);
	Delay(5);
	AT24C02_WriteByte(1,H_Score);
	Delay(5);
	
	AT24C02_WriteByte(2,match);//存入节数
	Delay(5);
	AT24C02_WriteByte(3,count);//存入50ms个数
	Delay(5);
	
	AT24C02_WriteByte(4,minute);//存入分钟数
	Delay(5);
	AT24C02_WriteByte(5,second);//存入秒数
	Delay(5);
	
	//存入比分位置数据
	AT24C02_WriteByte(6,xg);
	Delay(5);
	AT24C02_WriteByte(7,xgs1);
	Delay(5);
	AT24C02_WriteByte(8,xgs2);
	Delay(5);
	AT24C02_WriteByte(9,xgs3);
	Delay(5);
	AT24C02_WriteByte(10,xh);
	Delay(5);
	AT24C02_WriteByte(11,xhs1);
	Delay(5);
	AT24C02_WriteByte(12,xhs2);
	Delay(5);
	AT24C02_WriteByte(13,xhs3);
	Delay(5);
	
	AT24C02_WriteByte(14,S24_time);//存入24秒剩余
	Delay(5);
	
	AT24C02_WriteByte(15,mode);//存入比赛模式
	Delay(5);
	
}

//断电恢复
void RecoverAT2402()
{
	//读入比分
	G_Score=AT24C02_ReadByte(0);
	H_Score=AT24C02_ReadByte(1);
	
	match=AT24C02_ReadByte(2);//读入节数
	count=AT24C02_ReadByte(3);//读入50ms个数
	
	minute=AT24C02_ReadByte(4);//读入分钟数
	second=AT24C02_ReadByte(5);//读入秒数
	
	//读入比分位置数据
	xg=AT24C02_ReadByte(6);
	xgs1=AT24C02_ReadByte(7);
	xgs2=AT24C02_ReadByte(8);
	xgs3=AT24C02_ReadByte(9);
	xh=AT24C02_ReadByte(10);
	xhs1=AT24C02_ReadByte(11);
	xhs2=AT24C02_ReadByte(12);
	xhs3=AT24C02_ReadByte(13);
	S24_time=AT24C02_ReadByte(14);//读入24秒剩余
	AT24C02_ReadByte(15);	//读入模式
}
//*************** -断电保护和恢复函数定义end- *****************************************

//*************** -交换位置数据函数定义start- *****************************************
//交换位置数据
void SwapLocation(unsigned char *x,unsigned char *y)
{
	 unsigned char temp;
	 temp=*x;
	 *x=*y;
	 *y=temp;
 }
//*************** -交换位置数据函数定义end- *****************************************
 
//*************** -开始菜单函数定义start- *****************************************
//开始菜单
void StartMenu()
{
	
	LCD_ShowString(1,1,"Welcome To BTSsystem!Creatd by wjc zkh");
	LCD_ShowString(2,1,"0--START USING BTS");
	while(1){
		LCD_WriteCommand(0x18);
		Delay(500);
		if(MatrixKey_GetKeyNum()==0){
			LCD_Init();//LCD初始化
			break;
		}
	}

}
//*************** -开始菜单函数定义end- *****************************************

//*************** -更新显示函数定义start- *****************************************
//更新显示
void UpdateDisplay()
{
	    Time_Data[0] = '0' + minute / 10;     //赛程时间 分 十位
        Time_Data[1] = '0' + minute % 10;     //赛程时间 分 个位
        Time_Data[2] = '0' + second / 10;     //赛程时间 秒 十位
        Time_Data[3] = '0' + second % 10;     //赛程时间 秒 个位

        Score[0] = '0' + G_Score / 100;         //G队分数 百位
        Score[1] = '0' + G_Score % 100 / 10;    //G队分数 十位
        Score[2] = '0' + G_Score % 100 % 10;    //G队分数 个位
        Score[3] = '0' + H_Score / 100;         //H队分数 百位
        Score[4] = '0' + H_Score % 100 / 10;    //H队分数 十位
        Score[5] = '0' + H_Score % 100 % 10;    //H队分数 个位

        LCD_ShowString(1,xg,"G");               //对应的位置显示
        LCD_ShowChar(1,xgs1,Score[0]);
        LCD_ShowChar(1,xgs2,Score[1]);
        LCD_ShowChar(1,xgs3,Score[2]);
        LCD_ShowString(1,6,":");
        LCD_ShowChar(1,xhs1,Score[3]);
        LCD_ShowChar(1,xhs2,Score[4]);
        LCD_ShowChar(1,xhs3,Score[5]);
        LCD_ShowString(1,xh,"H");
		LCD_ShowString(2,6,"T:");
		LCD_ShowString(2,1,"SEC");
		LCD_ShowNum(2,4,match,1);
		LCD_ShowNum(2,14,S24_time,2);
		
		DDTube_SetBuf(4,minute / 10);//设置显示缓存，显示数据
		DDTube_SetBuf(5,minute % 10);
		DDTube_SetBuf(6,second / 10);
		DDTube_SetBuf(7,second % 10);
		

		while(sflag_updateTem){//获取温度并在LCD上显示
			GetTempreture();
			sflag_updateTem=~sflag_updateTem;
		}
		
		if(mode==NBA_MODE){
			LCD_ShowString(1,13,"NBA");
			
			
		}else if(mode==FIBA_MODE){
			LCD_ShowString(1,13,"FIBA");
			
		}
		
		 if((S24_time <= 0) && S24_flag)      //24秒结束，球未出手，计时器暂停，24秒复位
        {
            TR0 = 0;
            S24_time = 24;
			S24_flag=0;
			Buzzer_Time(100);
			
        }
		
		
		if(second < 0)
		{
			if(minute == 0)       //判断某节点是否结束
			{
				if(match < MATCH)      //不是最后一节比赛结束
				{
					//计时系统恢复为默认值，比赛节数加一
					match++;       
					minute = SetTime;  
					Buzzer_Time(150);//每节比赛结束吹哨
				}
				else //最后一节比赛结束
				{
					Buzzer_Time(700);//最后一节比赛结束吹哨
				}
				second = 0;
				TR0 = 0;     //停止时钟，等待下一节比赛开始
				S24_time = 24;
				S24_flag=0;
				
				
				if(match==3)//第二节比赛结束，即中场结束，第三节比赛将要开始，交换比分，同时吹哨
				{
					unsigned char *p1,*p2;
					p1=&xg;p2=&xh;
					SwapLocation(p1,p2);
					p1=&xgs1;p2=&xhs1;
					SwapLocation(p1,p2);
					p1=&xgs2;p2=&xhs2;
					SwapLocation(p1,p2);
					p1=&xgs3;p2=&xhs3;
					SwapLocation(p1,p2);
					Buzzer_Time(300);
				}
			}
			else
			{
				second = 59;
				minute--;
			}
		}
		
}
//*************** -更新显示函数定义end- *****************************************

//*************** -函数定义end- **********************************************************
