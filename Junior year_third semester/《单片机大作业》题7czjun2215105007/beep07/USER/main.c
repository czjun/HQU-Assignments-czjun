#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "beep.h"
//��������ʵ�� 

u8 continuous_enable = 0; //��ʼ������������״̬


int main(void)
{
	u8 key;
	delay_init(); //��ʱ������ʼ�� 
	LED_Init(); //LED �˿ڳ�ʼ��
	KEY_Init(); //��ʼ���밴�����ӵ�Ӳ���ӿ�
	BEEP_Init(); //��ʼ���������˿�
	LED0=0; //�ȵ������
	
		while(1)
	{
		  // ͳһɨ�谴����ģʽ��continuous_enable������
        key = KEY_Scan(continuous_enable);  // ����continuous_enableѡ��ɨ��ģʽ
        
        if(key) 
        { 
//            switch(key) 
//            { 
//                case WKUP_PRES:            // WKUP����
//									 continuous_enable = !continuous_enable;
//                    delay_ms(200);         // ������ʱ               
//                    break;
//                    
//                case KEY0_PRES:            // KEY0����
//                    LED0 = !LED0;          
//                    break;
//                    
//                case KEY1_PRES:            // KEY1����
//                    BEEP = !BEEP;         
//                    break;
//            }
					switch(key) 
            { 
                case WKUP_PRES:            // WKUP����
									 BEEP = 1;
                    delay_ms(100);         // ��ʱ     
								   BEEP = 0;
                    break;
                    
                case KEY0_PRES:            // KEY0����
                    BEEP = 0 ;         
                    break;
                    
                case KEY1_PRES:            // KEY1����
                    BEEP = 1;    
            }
					
					
					
					
					
					
        }
        else {
            delay_ms(100);  // �ް���ʱ�Ķ�����ʱ
        }
    }   
}