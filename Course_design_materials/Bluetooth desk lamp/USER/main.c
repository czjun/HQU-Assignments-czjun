
#include <reg52.h>
#include <intrins.h>
#include "sys_define.h"
#include "pwm.h"
#include "uart.h"

uchar Lamp_OnOff = 1;		//̨�ƿ��ر�־λ
uchar Reaction_OnOff = 0;		//�����Ӧ���ر�־λ

sbit Key2 = P2^6;
sbit Key4 = P2^5;
sbit Key5 = P2^4;
sbit Key6 = P2^3;
sbit Reaction = P2^7;	

void delay_ms(uint z)
{
	uint x,y;
	for(x = z; x > 0; x--)
		for(y = 114; y > 0 ; y--);
}

uchar Key_Scan()
{
		static uchar Key_up = 1;
		uchar i;
		if(Key_up && ( Key5 == 0 ||Key4 == 0 ))
		{
				Key_up = 0;
//				delay_ms(10);
//				if(Key6 == 0) i = 6;
				if(Key5 == 0) i = 5;
				if(Key4 == 0) i = 4;
//				if(Key2 == 0) i = 2;
				Uart_SendByte(i+'0');
				return i;
		}
		else if( Key5 == 1 && Key4 == 1)
		{
				Key_up = 1;
		}

}
/* Key2 ���ȼ�
 * Key4 ���ȼ�
 * Key5 �����Զ���Ӧ����
 * Key6 ̨�ƿ��� */
void Function()
{
		uchar Key = 0;
		Key = Key_Scan();
		if(Key)
		{
//				if(Key == 6)
//				{
//						if(Lamp_OnOff)
//						{
//								Lamp_Off();
//								Lamp_OnOff = 0;
//						}
//						else
//						{
//								Lamp_On();
//								Lamp_OnOff = 1;
//						}
//				}
//				if(Lamp_OnOff)
//				{
						if(Key == 4)
								PWM++;
						if(Key == 5)
								PWM--;
						if(PWM > 10)
								PWM = 10;
						else if(PWM < 1)
								PWM = 1;
						Uart_SendByte(PWM+'0');
//						if(Key == 5)
//								Reaction_OnOff = ~Reaction_OnOff;
//				}
		}
		if(Reaction_OnOff)
		{
				if(Reaction)
						Lamp_On();
				else Lamp_Off();
		}
}

void main()
{
		char num = 0;
		UART_init();
		delay_ms(50);
		time0_init();
		Lamp_On();
		while(1)
		{
				Function();
				delay_ms(50);
		}
}