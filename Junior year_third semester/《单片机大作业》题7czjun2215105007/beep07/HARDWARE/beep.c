#include "beep.h"

void BEEP_Init (void)
{

	GPIO_InitTypeDef GPIOB_Struct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能时钟


	GPIOB_Struct.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出
	GPIOB_Struct.GPIO_Pin=GPIO_Pin_8;
	GPIOB_Struct.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_Init (GPIOB,&GPIOB_Struct);

	GPIO_SetBits(GPIOB,GPIO_Pin_8);//PB8输出高电平
}