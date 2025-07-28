#ifndef __UART_H
#define	__UART_H

void UART_init(void);
void Send_string(unsigned char *p);
void Uart_SendByte(unsigned char num);
#endif