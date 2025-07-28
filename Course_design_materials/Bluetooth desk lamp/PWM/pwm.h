#ifndef __PWM_H
#define	__PWM_H
#include "sys_define.h"

sbit IN1 = P1^1;
sbit IN2 = P1^0;

extern char PWM;

void time0_init(void);
void Lamp_On();
void Lamp_Off();

#endif