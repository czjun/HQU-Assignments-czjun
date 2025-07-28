#include <reg52.h>
#include "uart.h"
#include "pwm.h"

extern uchar Lamp_OnOff;
extern uchar Reaction_OnOff;
unsigned char Receive;

void UART_init(void)
{
	TMOD |= 0x20;  	//T1����ģʽ2  8λ�Զ���װ
	TH1 = 0xfd;
	TL1 = 0xfd; 	//������9600
	TR1 = 1;		//����T1��ʱ��
	SM0 = 0;
	SM1 = 1; 		//���ڹ�����ʽ1 10λ�첽
	REN = 1;		//�����������
	EA  = 1;		//�����ж�
	ES  = 1;		//�����жϴ�
}

void UART(void) interrupt 4
{
	TR0 = 0;			//�رն�ʱ���жϣ���ֹ��Ӱ���������
	ET0 = 0;
	if(RI)	//����Ƿ�������
	{
		Receive = SBUF;	  //ȡ�����ջ�������ֵ
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
		SBUF = PWM + '0';				//���ص�ǰ����	
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
