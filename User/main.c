#include <REGX52.H>
#include "MatrixKey.h"	//�������
#include "LCD1602.h"	//LCD��ʾ
#include "DS18B20.h"	//�¶ȴ�����
#include "Delay.h"		//�����ʱ
#include "Timer.h"		//��ʱ��
#include "DDTube.h"		//��̬�����
#include "Buzzer.h"		//������
#include "AT24C02.h"	//EEPROM�ϵ籣��


/** ��Ŀ���ƣ�����ʱ�Ʒ�ϵͳ(Basketball Timing Scoring System, BTSsystem)
  * Created by Τ�̳� �����
  * Finished at 2024.12.23
  * ʹ�ù��ܽ��ܣ�
  * ���̹���, 0-������ʼ/����     1-������ͣ     2-���ӱ�����������ʱ��   3-�л�����ģʽ
			  4-A�ӽ�1����       5-A�ӽ�2����   6-A�ӽ�3����             7-�ϵ�ָ�
			  8-B�ӽ�1����       9-B�ӽ�2����   10-B�ӽ�3����   
    0:������ʼ�������죬�ܼ�ʱ��ʼ��24s��ʱ��ʼ��
    3:�л�����ģʽ��NBA or FIBA��ÿ�ڱ���ʱ��ֱ�Ϊ12��10min���л�ģʽ�����һ�γ�ʼ��
    7:�ϵ籣����ÿ����ϵͳ�Ὣ������ݴ���EEPROM�У��ϵ�������ϵ磬����ϵͳ������ָ����ϵ�ǰ�ֳ�����7��
      ������Ҫ�ָ���ֱ�Ӱ�0��ʼ����
	ÿ�ڱ��������󣬶�ʱ���ص���ʼʱ�䲢��ͣ��ʱ���ȴ���һ�ڱ�����ʼ��2�ڱ����󣨼��г����������Զ������ͳ�������
	�ȷ�λ��
  */


//*************** -���峣��start- ******************
#define NBA_MODE 0
#define FIBA_MODE 1
//**************** -���峣��end- *******************

//*************** -��������start- *****************************************
unsigned char SetTime;//������ʼʱ�䣨���ӣ�
float T;//�¶�
unsigned char S24_time;   //24S����
bit S24_flag;//�Ƿ�ʹ��24s����

unsigned char Score[6];        //���ڴ洢����ı�������
unsigned char Time_Data[4];    //�������ڴ�Ŵ�����趨ʱ��

//�����ⲿ����
extern unsigned int TempCounter;//�¶ȸ��¼�����
extern bit sflag_updateTem;//�¶ȸ��±�־λ

//����ı�����Ҫ�����EEPROM�����жϵ籣��
unsigned char xg,xgs1,xgs2,xgs3,xh,xhs1,xhs2,xhs3;//�ȷֺ������ͳ�λ�ü�¼
unsigned char mode=NBA_MODE;//����ģʽĬ��ΪNBA
unsigned char G_Score;//�ͳ�Guest����
unsigned char H_Score;//����Home����
unsigned char match;//����
unsigned char count;//50ms����
unsigned char minute;//������
char second;//����


//*************** -�������� end- *****************************************

//*************** -��������start- *****************************************
void SysInit();//��ʼ�������������ڴ����
void GetTempreture();//��ȡ�����¶�
void KeyControl();//�������ƺ���
void SaveAT2402();//�ϵ籣��
void RecoverAT2402();//�ϵ�ָ�
void SwapLocation(unsigned char *x,unsigned char *y);//����λ������
void StartMenu();//��ʼ�˵�
void UpdateDisplay();//������ʾ
//*************** -��������end- *****************************************

void main(){
	DS18B20_ConvertT();//�ϵ��Ƚ���һ���¶�ת������ֹ��һ�����ݶ�ȡ����
	Delay(1000);//�ȴ�ת�����
	SysInit();
	StartMenu();
	
	while(1){
		KeyControl();
		UpdateDisplay();

	}
}

//*************** -��ʼ����������start- *****************************************

//��ʼ��
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
	count = 20;//50ms����
	xg=1;xgs1=3;xgs2=4;xgs3=5;xh=11;xhs1=7;xhs2=8;xhs3=9;
	G_Score=0;H_Score=0;match=1;
	
	LCD_Init();//LCD��ʼ��
	Timer0_Init();//��ʼ����ʱ��
	Timer1_Init();
	TR0=0;//��ʱ��0ֹͣ
}

//*************** -��ʼ����������end- *****************************************

//*************** -���̿��ƺ�������start- ****************************************
/**
* @brief  ������̹��ܣ�0-������ʼ/����    1-������ͣ     2-���ӱ�������    3-�л�����ģʽ
					   4-A�ӽ�1����       5-A�ӽ�2����   6-A�ӽ�3����      7-�ϵ�ָ�
					   8-B�ӽ�1����       9-B�ӽ�2����   10-B��3����     
  * @param  ��
  * @retval ��
			
  */
void KeyControl()
{
	switch(MatrixKey_GetKeyNum())
	{
		case 0:            //������ʼ/����
            TR0 = 1;             //��ʱ�����ش򿪣���ʼ��ʱ
			S24_flag=1;	//24S��������
            Buzzer_Time(600);//������ʼ/����������
            break;

		case 1:            //������ͣ
            TR0 = 0;          //��ʱ�����ش򿪣�ֹͣ��ʱ
			Buzzer_Time(400);//������ͣ������
            break;

		case 2:             //�ֶ����ӱ����������ܼ�ʱ����24s��λ
            if((match >= MATCH) && (minute == 0)&&(G_Score==H_Score))     //�жϱ��������Ƿ񳬹��Ľ��Լ��˽ڱ����Ƿ����
            {
                TR0 = 0;                   //��ʱ��ֹͣ
                match++;                   //���̽�����һ
                minute = 5;                //��ʱ������ʱ������ 5����
                second = 0;
                S24_time = 24;     //24��ָ���ֵ
				S24_flag=0;
            }
            break;
			
		case 3:             //����ģʽ�л�
			   
			mode+=1;
			if(mode>1) mode=0;
			SysInit();
            break;
		
		 //ÿ�Ӽӷֺ�24��ָ���ֵ
		case 4:                         //�ͳ������1��
			G_Score += 1;
			S24_time = 24;
            break;  

		case 5:                         //�ͳ������2��
			G_Score += 2;
			S24_time = 24;
            break;    
                                 
		case 6:                         //�ͳ������3��
			G_Score += 3;
			S24_time = 24;
            break;
		
		case 7:                         //��EEPROM�ж������ݣ��ϵ�ָ�
			RecoverAT2402();
            break;
		
		case 8:                         //���������1��
			H_Score += 1;
			S24_time = 24;
            break;

		case 9:                         //���������2��
			H_Score += 2;
			S24_time = 24;

            break;

		case 10:                         //���������3��
			H_Score += 3;
			S24_time = 24;
            break;
		

		default:
			break;	
	}
	
}
//*************** -���̿��ƺ�������end- ****************************************

//*************** -��ȡ�����¶Ⱥ�������start- **********************************
//��ȡ�����¶�
void GetTempreture()
{
	DS18B20_ConvertT();	//ת���¶�
	T=DS18B20_ReadT();	//��ȡ�¶�
	if(T<0)				//����¶�С��0
	{
		LCD_ShowChar(2,8,'-');	//��ʾ����
		T=-T;			//���¶ȱ�Ϊ����
	}
	else				//����¶ȴ��ڵ���0
	{
		LCD_ShowChar(2,8,'+');	//��ʾ����
	}
	LCD_ShowNum(2,9,T,2);		//��ʾ�¶���������
	LCD_ShowChar(2,11,'.');		//��ʾС����
	LCD_ShowNum(2,12,(unsigned long)(T*100)%100,1);//��ʾ�¶�С������
}
//*************** -��ȡ�����¶Ⱥ�������end- ***************************************

//*************** -�ϵ籣���ͻָ���������start- *****************************************
//�ϵ籣��
void SaveAT2402()
{
	//����ȷ�
	AT24C02_WriteByte(0,G_Score);
	Delay(5);
	AT24C02_WriteByte(1,H_Score);
	Delay(5);
	
	AT24C02_WriteByte(2,match);//�������
	Delay(5);
	AT24C02_WriteByte(3,count);//����50ms����
	Delay(5);
	
	AT24C02_WriteByte(4,minute);//���������
	Delay(5);
	AT24C02_WriteByte(5,second);//��������
	Delay(5);
	
	//����ȷ�λ������
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
	
	AT24C02_WriteByte(14,S24_time);//����24��ʣ��
	Delay(5);
	
	AT24C02_WriteByte(15,mode);//�������ģʽ
	Delay(5);
	
}

//�ϵ�ָ�
void RecoverAT2402()
{
	//����ȷ�
	G_Score=AT24C02_ReadByte(0);
	H_Score=AT24C02_ReadByte(1);
	
	match=AT24C02_ReadByte(2);//�������
	count=AT24C02_ReadByte(3);//����50ms����
	
	minute=AT24C02_ReadByte(4);//���������
	second=AT24C02_ReadByte(5);//��������
	
	//����ȷ�λ������
	xg=AT24C02_ReadByte(6);
	xgs1=AT24C02_ReadByte(7);
	xgs2=AT24C02_ReadByte(8);
	xgs3=AT24C02_ReadByte(9);
	xh=AT24C02_ReadByte(10);
	xhs1=AT24C02_ReadByte(11);
	xhs2=AT24C02_ReadByte(12);
	xhs3=AT24C02_ReadByte(13);
	S24_time=AT24C02_ReadByte(14);//����24��ʣ��
	AT24C02_ReadByte(15);	//����ģʽ
}
//*************** -�ϵ籣���ͻָ���������end- *****************************************

//*************** -����λ�����ݺ�������start- *****************************************
//����λ������
void SwapLocation(unsigned char *x,unsigned char *y)
{
	 unsigned char temp;
	 temp=*x;
	 *x=*y;
	 *y=temp;
 }
//*************** -����λ�����ݺ�������end- *****************************************
 
//*************** -��ʼ�˵���������start- *****************************************
//��ʼ�˵�
void StartMenu()
{
	
	LCD_ShowString(1,1,"Welcome To BTSsystem!Creatd by wjc zkh");
	LCD_ShowString(2,1,"0--START USING BTS");
	while(1){
		LCD_WriteCommand(0x18);
		Delay(500);
		if(MatrixKey_GetKeyNum()==0){
			LCD_Init();//LCD��ʼ��
			break;
		}
	}

}
//*************** -��ʼ�˵���������end- *****************************************

//*************** -������ʾ��������start- *****************************************
//������ʾ
void UpdateDisplay()
{
	    Time_Data[0] = '0' + minute / 10;     //����ʱ�� �� ʮλ
        Time_Data[1] = '0' + minute % 10;     //����ʱ�� �� ��λ
        Time_Data[2] = '0' + second / 10;     //����ʱ�� �� ʮλ
        Time_Data[3] = '0' + second % 10;     //����ʱ�� �� ��λ

        Score[0] = '0' + G_Score / 100;         //G�ӷ��� ��λ
        Score[1] = '0' + G_Score % 100 / 10;    //G�ӷ��� ʮλ
        Score[2] = '0' + G_Score % 100 % 10;    //G�ӷ��� ��λ
        Score[3] = '0' + H_Score / 100;         //H�ӷ��� ��λ
        Score[4] = '0' + H_Score % 100 / 10;    //H�ӷ��� ʮλ
        Score[5] = '0' + H_Score % 100 % 10;    //H�ӷ��� ��λ

        LCD_ShowString(1,xg,"G");               //��Ӧ��λ����ʾ
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
		
		DDTube_SetBuf(4,minute / 10);//������ʾ���棬��ʾ����
		DDTube_SetBuf(5,minute % 10);
		DDTube_SetBuf(6,second / 10);
		DDTube_SetBuf(7,second % 10);
		

		while(sflag_updateTem){//��ȡ�¶Ȳ���LCD����ʾ
			GetTempreture();
			sflag_updateTem=~sflag_updateTem;
		}
		
		if(mode==NBA_MODE){
			LCD_ShowString(1,13,"NBA");
			
			
		}else if(mode==FIBA_MODE){
			LCD_ShowString(1,13,"FIBA");
			
		}
		
		 if((S24_time <= 0) && S24_flag)      //24���������δ���֣���ʱ����ͣ��24�븴λ
        {
            TR0 = 0;
            S24_time = 24;
			S24_flag=0;
			Buzzer_Time(100);
			
        }
		
		
		if(second < 0)
		{
			if(minute == 0)       //�ж�ĳ�ڵ��Ƿ����
			{
				if(match < MATCH)      //�������һ�ڱ�������
				{
					//��ʱϵͳ�ָ�ΪĬ��ֵ������������һ
					match++;       
					minute = SetTime;  
					Buzzer_Time(150);//ÿ�ڱ�����������
				}
				else //���һ�ڱ�������
				{
					Buzzer_Time(700);//���һ�ڱ�����������
				}
				second = 0;
				TR0 = 0;     //ֹͣʱ�ӣ��ȴ���һ�ڱ�����ʼ
				S24_time = 24;
				S24_flag=0;
				
				
				if(match==3)//�ڶ��ڱ������������г������������ڱ�����Ҫ��ʼ�������ȷ֣�ͬʱ����
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
//*************** -������ʾ��������end- *****************************************

//*************** -��������end- **********************************************************
