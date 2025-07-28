#include <reg52.h>
#include "pwm.h"

char PWM = 1;  //1-10��λ

void time0_init(void)
{
		TMOD |= 0x02;		//��ʱ��0 8λ�Զ���װ
		TH0 = 0xc8;
		TL0 = 0;
		EA = 1;			//���ж�
		ET0 = 1;		//TIME0 �ж�
		TR0 = 1;		//������ʱ��0
		IN2 = 0;
}

/* �ر�̨�� */
void Lamp_Off()
{
		TR0 = 0;
		ET0 = 0;
		IN1 = 0;
}

/*����̨�� */
void Lamp_On()
{
		TR0 = 1;
		ET0 = 1;
}

void time0() interrupt 1		//200us �ж�һ�� ����1ms PWMƵ��1KHz
{
		static unsigned int num = 0;
		num++;
		if(num > 9)
			num = 0;
		if(num < PWM)
			IN1 = 1;
		else IN1 = 0;
}

