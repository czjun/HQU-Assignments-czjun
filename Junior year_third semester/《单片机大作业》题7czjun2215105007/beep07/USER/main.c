#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "beep.h"
//按键输入实验 

u8 continuous_enable = 0; //初始化连续按开关状态


int main(void)
{
	u8 key;
	delay_init(); //延时函数初始化 
	LED_Init(); //LED 端口初始化
	KEY_Init(); //初始化与按键连接的硬件接口
	BEEP_Init(); //初始化蜂鸣器端口
	LED0=0; //先点亮红灯
	
		while(1)
	{
		  // 统一扫描按键（模式由continuous_enable决定）
        key = KEY_Scan(continuous_enable);  // 根据continuous_enable选择扫描模式
        
        if(key) 
        { 
//            switch(key) 
//            { 
//                case WKUP_PRES:            // WKUP按下
//									 continuous_enable = !continuous_enable;
//                    delay_ms(200);         // 消抖延时               
//                    break;
//                    
//                case KEY0_PRES:            // KEY0按下
//                    LED0 = !LED0;          
//                    break;
//                    
//                case KEY1_PRES:            // KEY1按下
//                    BEEP = !BEEP;         
//                    break;
//            }
					switch(key) 
            { 
                case WKUP_PRES:            // WKUP按下
									 BEEP = 1;
                    delay_ms(100);         // 延时     
								   BEEP = 0;
                    break;
                    
                case KEY0_PRES:            // KEY0按下
                    BEEP = 0 ;         
                    break;
                    
                case KEY1_PRES:            // KEY1按下
                    BEEP = 1;    
            }
					
					
					
					
					
					
        }
        else {
            delay_ms(100);  // 无按键时的短暂延时
        }
    }   
}