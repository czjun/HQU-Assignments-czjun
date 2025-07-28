#include <reg52.h>
#include "uart.h"
#include "pwm.h"

extern uchar Lamp_OnOff;
extern uchar Reaction_OnOff;
unsigned char Receive;

void UART_init(void)
{
	TMOD |= 0x20;  	//T1工作模式2  8位自动重装
	TH1 = 0xfd;
	TL1 = 0xfd; 	//比特率9600
	TR1 = 1;		//启动T1定时器
	SM0 = 0;
	SM1 = 1; 		//串口工作方式1 10位异步
	REN = 1;		//串口允许接收
	EA  = 1;		//开总中断
	ES  = 1;		//串口中断打开
}

void UART(void) interrupt 4
{
	TR0 = 0;			//关闭定时器中断，防止其影响接收数据
	ET0 = 0;
	if(RI)	//检测是否接收完成
	{
		Receive = SBUF;	  //取出接收缓存器的值
		RI = 0;
		switch(Receive)
		{
			case 'A': PWM++; break;
			case 'S': PWM--; break;
			case 'O': Lamp_On(); Lamp_OnOff = 1; break;
			case 'F': Lamp_Off(); Lamp_OnOff = 0; break;
			case 'M': Reaction_OnOff = ~Reaction_OnOff; break;
			default : break;
		}
		if(PWM > 10)
				PWM = 10;
		else if(PWM < 1)
				PWM = 1;
		SBUF = PWM + '0';				//返回当前档数	
		while(!TI);
		TI = 0;
	}
	if(Lamp_OnOff)
	{
			TR0 = 1;
			ET0 = 1;
	}
}

void Send_string(unsigned char *p)
{
		while(*p)
		{
				SBUF = *p++;	
				while(!TI);
				TI = 0;
		}
}

void Uart_SendByte(unsigned char num)
{
		SBUF = num;	
		while(!TI);
		TI = 0;
}
