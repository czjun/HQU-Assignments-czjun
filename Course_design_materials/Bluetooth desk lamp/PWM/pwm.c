#include <reg52.h>
#include "pwm.h"

char PWM = 1;  //1-10档位

void time0_init(void)
{
		TMOD |= 0x02;		//定时器0 8位自动重装
		TH0 = 0xc8;
		TL0 = 0;
		EA = 1;			//总中断
		ET0 = 1;		//TIME0 中断
		TR0 = 1;		//开启定时器0
		IN2 = 0;
}

/* 关闭台灯 */
void Lamp_Off()
{
		TR0 = 0;
		ET0 = 0;
		IN1 = 0;
}

/*开启台灯 */
void Lamp_On()
{
		TR0 = 1;
		ET0 = 1;
}

void time0() interrupt 1		//200us 中断一次 周期1ms PWM频率1KHz
{
		static unsigned int num = 0;
		num++;
		if(num > 9)
			num = 0;
		if(num < PWM)
			IN1 = 1;
		else IN1 = 0;
}

