#include <reg51.h>       //51单片机核心头文件
#include <intrins.h>     //包含内部函数
#define uchar unsigned char  //宏定义无符号字符型
#define uint unsigned int   //宏定义无符号整型


static uint last_qhao = 0;

// 普中开发板适配后引脚定义
sbit RS=P2^6;      // LCD RS接P2^6
sbit RW=P2^5;      // LCD RW接P2^5
sbit E=P2^7;       // LCD E接P2^7
sbit K1=P3^2;      // 取号键K1接P3^2（普中独立按键1）
sbit K2=P3^3;      // 查询键K2接P3^3（普中独立按键2）
 
sbit P3_0 = P3^0;    //串行口
sbit P3_1 = P3^1;    // 

uint m=0,i=0;  //循环变量
//LCD显示字符串（初始及提示信息）
uchar data test[]="Please Take K3!";             //初始化显示"Welcome!"
uchar data prompt1[]="Your No.is 00!";    //取号成功提示（"Your No.is XX!"）
uchar data prompt8[]="00 is left";        //等待人数提示（"XX is left"）
uchar data prompt4[]="The queue is";      //队列满提示第一行
uchar data prompt5[]="full,please wait";  //队列满提示第二行

//系统核心变量
unsigned  qhao=1;  //取号总数（从1开始，每取一次+1）
uint dhao=1;           //等待人数（qhao - jhao）
uint jhao=0;           //已叫号数（从叫号机通过串口接收）


/**
 * 串口初始化函数（与叫号机通信，同步数据）
 * 说明：配置同叫号机，确保波特率和工作方式一致（才能正常通信）
 */
void intCon() 
{  
    EA=0;         //关闭总中断（查询方式处理串口）
    SCON=0x50;    //0101 0000：工作方式1（10位异步收发），允许接收
    PCON=0X00;    //SMOD=0（波特率不加倍）
    TMOD=0x20;    //定时器1方式2（自动重装载，产生波特率）
    TH1=0xfd;     //9600波特率初值（同叫号机，确保通信一致）
    TL1=0xfd;  
    TR1=1;        //启动定时器1（产生波特率）
		P3_0=1;
	  P3_1=1;
} 

/**
 * 短延时函数（用于按键防抖）
 * @param n：延时倍数
 */
void delay(int n)   
{ 
    int k,j;   
    for(k=0;k<=n;k++)   
        for(j=0;j<=10;j++); 
} 

/**
 * 长延时函数（用于状态保持）
 * @param n：延时倍数
 */
void delay1s(int n)   
{
    int k,j;   
    for(k=0;k<=n;k++)   
        for(j=0;j<=120;j++); 
}       


/**************LCD1602相关函数（同叫号机，复用逻辑）**********/

/**
 * 毫秒级延时（用于LCD时序）
 * @param ms：延时毫秒数
 */
void delay1ms(unsigned int ms)   
{ 
    uint k,j;  
    for(k=0;k<ms;k++)   
        for(j=0;j<100;j++); 
}

/**
 * 向LCD写入指令
 * @param com：指令代码
 */
void LCD_w_com(unsigned com)  
{  
    RW=0;  
    RS=0;
    E=1;  
    P0=com; 
    delay1ms(40);  
    E=0; 
    RW=1;
} 

/**
 * 向LCD写入数据（显示字符）
 * @param dat：字符ASCII码
 */
void LCD_w_dat(uchar dat)   
{ 
    RW=0; 
    RS=1;  
    E=1; 
    P0=dat; 
    delay1ms(40); 
    E=0;  
    RW=1; 
}  

/**
 * 设置LCD光标位置
 * @param x：行号（1/2）；@param y：列号（0-15）
 */
void gotoxy(unsigned x,unsigned y)  
{  
    if(x==1)  
        LCD_w_com(0x80 + y); 
    else 
        LCD_w_com(0xC0 + y); 
} 

/**
 * 清空LCD显示
 */
void clear_LCD(void) 
{ 
    LCD_w_com(0x01);  //清屏
    LCD_w_com(0x02);  //光标归位
} 

/**
 * 初始化LCD1602
 */
void init_LCD(void)      
{  
    LCD_w_com(0x38);  //16x2显示，8位接口
    LCD_w_com(0x0c);  //显示开，光标关
    LCD_w_com(0x06);  //光标自动右移
    gotoxy(1,0);      //定位第一行
    //显示初始化欢迎语"Welcome!"
    for(i=0;i<=14;i++)  
    {  
        LCD_w_dat(test[i]); 
    } 
} 


/**
 * 取号键处理函数（K1按下时执行）
 * 功能：生成新号码，显示取号结果，超过上限提示队列满
 */
void panduan_01() 	
{  
    if(K1 == 0)          //检测到K1按下
    {  
        delay(10);       //10ms防抖（避免抖动误判）
        if(K1 == 0)      //再次检测，确认按下
        {    
            //判断取号数是否超过上限（99，避免两位数溢出）
            if(qhao > 99)  
            {
                EA=0;    //关闭中断（备用）
                delay1s(5000);  //延时保持显示
                clear_LCD();    //清屏
                gotoxy(1,2);    //第一行第3列（居中显示）
                //显示队列满提示第一行"The queue is"
                for(m=0;m<=11;m++)   
                {    
                    LCD_w_dat(prompt4[m]);      
                }    
                gotoxy(2,0);    //第二行
                //显示队列满提示第二行"full,please wait"
                for(m=0;m<=15;m++)   
                {    
                    LCD_w_dat(prompt5[m]);      
                }     
            } 	
            else  //未超过上限，正常取号
            {
                clear_LCD();     //清屏
                //更新取号提示的号码（"Your No.is XX!"）
                prompt1[11] = (qhao)/10 + '0';  //十位
                prompt1[12] = (qhao)%10 + '0';  //个位
                qhao++;        //取号数+1（下一个人取号用）
                gotoxy(1,0);   //定位第一行
                //显示取号结果（"Your No.is XX!"）
                for(m=0;m<=13;m++)   
                {   
                    LCD_w_dat(prompt1[m]); 
                }   
            } 
            //计算等待人数（当前取号数 - 已叫号数）
            dhao = qhao - jhao;    
            //显示等待人数（当前取号者前面的等待人数）
            prompt8[0] = (dhao - 1)/10 + '0';  //十位
            prompt8[1] = (dhao - 1)%10 + '0';  //个位
            gotoxy(2,0);                       //定位第二行
            for(m=0;m<=9;m++)                  //显示"XX is left"
            {  
                LCD_w_dat(prompt8[m]);     
            } 
        }
    }
    while(K1 == 0);  //等待按键松开（避免长按重复触发）
}



/**
 * 查询键处理函数（K2按下时执行）
 * 功能：显示当前等待人数（取号总数 - 已叫号数）
 */
void panduan_02() 
{   
    if(K2 == 0)      //检测到K2按下
    {  
        delay(10);   //10ms防抖
        if(K2 == 0)  //确认按下
        { 
            dhao = qhao - jhao;    //计算等待人数
            clear_LCD();           //清屏
            //更新等待人数显示
            prompt8[0] = dhao / 10 + '0';  //十位
            prompt8[1] = dhao % 10 + '0';  //个位
            gotoxy(2,0);                  //定位第二行
            for(m=0;m<=9;m++)             //显示"XX is left"
            {  
                LCD_w_dat(prompt8[m]);     
            }    
        }  
    }   
}  


/**
 * 主函数（系统入口）
 * 功能：初始化硬件，循环检测按键和串口数据
 */
void main(void) 
{ 
	
    clear_LCD();       //初始化清屏
    init_LCD();        //初始化LCD
    intCon();          //初始化串口（与叫号机通信）
    jhao = 0;          //初始化已叫号数为0
    while(1) {
    if (K1==0) panduan_01();
    // 只有新号时才发
    if (qhao != last_qhao) {
        last_qhao = qhao;
        SBUF = qhao;
        while(!TI);
        TI = 0;
				}
    if (RI) {
        RI = 0;
        jhao = SBUF;
    }
    if (K2==0) panduan_02();
	}
}