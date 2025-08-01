#include <reg51.h>       //51单片机核心头文件，包含寄存器定义
#include <intrins.h>     //包含_nop_等内部函数（本代码未直接使用）
#define uchar unsigned char  //宏定义：uchar代替unsigned char（无符号字符型）
#define uint unsigned int   //宏定义：uint代替unsigned int（无符号整型）



// 普中开发板适配后引脚定义
sbit RS=P2^6;      // LCD RS接P2^6（普中LCD控制口）
sbit RW=P2^5;      // LCD RW接P2^5
sbit E=P2^7;       // LCD E接P2^7

// 矩阵键盘
sbit P1_0 = P1^0;    // 行1
sbit P1_1 = P1^1;    // 行2
sbit P1_2 = P1^2;    // 行3
sbit P1_3 = P1^3;    // 行4
sbit P1_4 = P1^4;    // 列1
sbit P1_5 = P1^5;    // 列2
sbit P1_6 = P1^6;    // 列3
sbit P1_7 = P1^7;    // 列4

sbit P3_0 = P3^0;    //串行口
sbit P3_1 = P3^1;    // 

sbit BEEP=P2^5;    // 蜂鸣器接P2^5（普中开发板蜂鸣器引脚）
static uchar last_jhao = 0;
static uchar last_qhao_rcv = 0;  // 上次接收的取号
//系统变量定义
uchar sev=0;      //总叫号计数（备用） 
uchar sev1=0;     //1号窗口叫号计数
uchar sev2=0;     //2号窗口叫号计数
uchar sev3=0;     //3号窗口叫号计数
uchar sev4=0;     //4号窗口叫号计数
uchar m=0,i=0;    //循环变量
uchar jhao=0;     //当前叫到的号码（初始为1）
uchar qhao=0;     //取号总数（从取号机通过串口接收）
uchar dhao=0;     //等待人数（qhao - jhao）
uchar jz=0;       //当前叫号的窗口（1-4）
uchar c=0;
//LCD显示字符串定义（初始及提示信息）
uchar data test[]="Hello!";                    //LCD初始化显示字符
uchar data prompt2[]="No.00 come to No";      //叫号提示第一行（"No.XX come to No"）
uchar data prompt3[]="0.window,Please!";      //叫号提示第二行（"X.window,Please!"）
uchar data prompt8[]="00 is left";            //等待人数提示（"XX is left"）
uchar data prompt9[]="All is 00";             //总取号数提示（"All is XX"）
uchar data prompt10[]="W1 - 00";              //1号窗口叫号记录
uchar data prompt11[]="W2 - 00";              //2号窗口叫号记录
uchar data prompt12[]="W3 - 00";              //3号窗口叫号记录
uchar data prompt13[]="W4 - 00";              //4号窗口叫号记录


/**
 * 串口初始化函数（用于与取号机通信，同步取号数和叫号数）
 */
void intCon()  
{    
    EA=0;        //关闭总中断（暂不使用中断，仅用查询方式处理串口）
    //串口控制寄存器配置（工作方式1，允许接收）
    SCON=0x50;   //0101 0000：SM0=0,SM1=1（工作方式1，10位异步收发：8位数据+起始/停止位）
    PCON=0X00;   //电源控制寄存器：SMOD=0（波特率不加倍）
    //定时器1配置（用于产生串口波特率）
    TMOD=0x20;   //0010 0000：定时器1工作在方式2（8位自动重装载，稳定产生波特率）
                  //波特率9600计算：11.0592MHz晶振下，方式2波特率=晶振频率/(32*12*(256-TH1))
    TH1=0xfd;    //装入初值（9600波特率对应初值：0xfd）
    TL1=0xfd;    //方式2下TL1溢出后自动重载TH1的值
    TR1=1;       //启动定时器1（开始产生波特率）
	  P3_0=1;
	  P3_1=1;
} 

/**
 * 短延时函数（用于按键防抖、简单等待）
 * @param n：延时倍数（n越大，延时越长）
 */
void delay(int n)       
{ 
    int k,j;  
    for(k=0;k<=n;k++)  
        for(j=0;j<=10;j++);  //内层循环约10次，整体延时较短
} 

/**
 * 较长延时函数（用于状态保持显示）
 * @param n：延时倍数
 */
void delay1s(int n)     
{ 
    int k,j;  
    for(k=0;k<=n;k++)  
        for(j=0;j<=120;j++);  //比delay延时更长
}  

/**
 * 蜂鸣器发声函数（叫号时提示）
 * @param x：延时参数（控制频率，x越小频率越高）
 */
void SPEAKER(uint x)   
{   
    for(i=0;i<=200;i++)    //发声200次（控制发声时长）
    {        
        delay(x);          //延时控制周期
        BEEP=~BEEP;        //蜂鸣器引脚电平翻转（产生方波发声）
    }     
    BEEP=1;                //发声结束后关闭蜂鸣器（高电平不发声）
}


/**************LCD1602显示相关函数**********/

/**
 * 毫秒级延时函数（用于LCD操作时序，满足LCD响应时间）
 * @param ms：延时毫秒数（约为实际毫秒数）
 */
void delay1ms(unsigned int ms)   
{ 
    uint k,j;  
    for(k=0;k<ms;k++)   
        for(j=0;j<100;j++);  //循环约100次，单次约10us，整体约1ms
}

/**
 * 向LCD写入指令（配置LCD工作模式）
 * @param com：指令代码（如清屏、设置显示模式等）
 */
void LCD_w_com(unsigned com)  
{  
    RW=0;          //RW=0：写入操作
    RS=0;          //RS=0：写入指令
    E=1;           //E=1：使能信号有效（LCD读取数据）
    P0=com;        //P0口输出指令代码
    delay1ms(40);  //等待LCD响应（至少需几十us）
    E=0;           //E=0：结束写入
    RW=1;          //恢复RW为1（空闲状态）
} 

/**
 * 向LCD写入数据（显示字符）
 * @param dat：要显示的字符（ASCII码）
 */
void LCD_w_dat(uchar dat)   
{ 
    RW=0;          //RW=0：写入操作
    RS=1;          //RS=1：写入数据
    E=1;           //E=1：使能信号有效
    P0=dat;        //P0口输出字符数据
    delay1ms(40);  //等待LCD响应
    E=0;           //结束写入
    RW=1;          //恢复空闲状态
}  

/**
 * 设置LCD光标位置
 * @param x：行号（1：第一行；2：第二行）
 * @param y：列号（0-15：对应16列）
 */
void gotoxy(unsigned x,unsigned y)  
{  
    if(x==1)                  //第一行起始地址为0x80
        LCD_w_com(0x80 + y);  //0x80 | 列号（如0x80+0表示第一行第1列）
    else                      //第二行起始地址为0xC0
        LCD_w_com(0xC0 + y);  //0xC0 | 列号（如0xC0+0表示第二行第1列）
} 

/**
 * 清空LCD显示并将光标归位
 */
void clear_LCD(void) 
{ 
    LCD_w_com(0x01);  //清屏指令（0x01：清除所有显示）
    LCD_w_com(0x02);  //光标归位指令（0x02：光标回到初始位置）
} 

/**
 * 初始化LCD1602（设置显示模式、开关等）
 */
void init_LCD(void)      
{  
    LCD_w_com(0x38);  //0x38：设置16x2显示（2行，每行16字符），8位数据接口
    LCD_w_com(0x0c);  //0x0c：显示开，光标关（0x0c=00001100：D=1显示开，C=0光标关）
    LCD_w_com(0x06);  //0x06：光标自动右移，屏幕不滚动（输入方式设置）
    gotoxy(1,0);      //定位到第一行第1列
    //显示初始化字符"Hello!"
    for(i=0;i<=5;i++)  
    {  
        LCD_w_dat(test[i]); 
    } 
} 


/*********叫号核心逻辑函数*********/

/**
 * 叫号处理函数（当窗口按键按下时，执行叫号逻辑）
 * 功能：更新叫号信息、LCD显示、蜂鸣提示
 */
void key()
{         
    //只有当取号数大于当前叫号数时，才能叫号（避免叫号超过取号）
    if(qhao > jhao)     
    {       
        SPEAKER(11);    //蜂鸣器短鸣（提示叫号）
        delay(200);     
        SPEAKER(15);    //蜂鸣器再鸣一次（不同频率）
        clear_LCD();    //清屏准备显示新叫号信息

        sev++;          //总叫号计数+1
        //更新叫号提示第一行的号码（"No.XX..."）
        prompt2[3] = (jhao + 1)/10 + '0';  //十位数字（如12的十位是1）
        prompt2[4] = (jhao + 1)%10 + '0';  //个位数字（如12的个位是2）
        //更新叫号提示第二行的窗口号（"X.window..."）
        prompt3[0] = jz + '0';             //窗口号（1-4）
        //显示第一行提示
        gotoxy(1,0);      
        for(m=0;m<=15;m++)      //prompt2共16个字符（填满第一行）
        {      
            LCD_w_dat(prompt2[m]);      
        }      
        //显示第二行提示
        gotoxy(2,0);      
        for(m=0;m<=15;m++)     //prompt3共16个字符（填满第二行）
        {
            LCD_w_dat(prompt3[m]);     
        }         
        jhao++;  //当前叫号数+1（完成一次叫号）
    }
}

/**
 * 显示等待人数函数（对应某个按键，按下时显示当前等待人数）
 */
void k5()
{
    clear_LCD();                  //清屏
    dhao = qhao - jhao;           //等待人数=总取号数-已叫号数
    prompt8[0] = dhao / 10 + '0'; //等待人数十位
    prompt8[1] = dhao % 10 + '0'; //等待人数个位
    gotoxy(1,0);                  //定位第一行
    for(m=0;m<=9;m++)             //显示"XX is left"（共10个字符）
    {  
        LCD_w_dat(prompt8[m]);     
    }       
}

/**
 * 显示总取号数函数（对应某个按键，按下时显示总取号数）
 */
void k6()
{     
    clear_LCD();                  //清屏
    prompt9[7] = sev / 10 + '0';  //总叫号数十位
    prompt9[8] = sev % 10 + '0';  //总叫号数个位
    gotoxy(1,0);                  //定位第一行
    for(m=0;m<=8;m++)             //显示"All is XX"（共9个字符）
    {   
        LCD_w_dat(prompt9[m]); 
    }  
}

/**
 * 显示各窗口叫号记录函数（对应某个按键，按下时显示4个窗口的叫号数）
 */
void k7()
{
    clear_LCD();                  //清屏
    //更新各窗口叫号记录的显示（"W1 - XX"等）
    prompt10[5] = sev1 / 10 + '0';    
    prompt10[6] = sev1 % 10 + '0';    
    prompt11[5] = sev2 / 10 + '0';    
    prompt11[6] = sev2 % 10 + '0';    
    prompt12[5] = sev3 / 10 + '0';    
    prompt12[6] = sev3 % 10 + '0';    
    prompt13[5] = sev4 / 10 + '0';    
    prompt13[6] = sev4 % 10 + '0';     
    //第一行显示W1和W2（分列显示，避免重叠）
    gotoxy(1,0);                     //第一行第1列
    for(m=0;m<=6;m++)                //显示"W1 - XX"（7个字符）
    {    
        LCD_w_dat(prompt10[m]);    
    }        
    gotoxy(1,8);                     //第一行第9列（留出间隔）
    for(m=0;m<=6;m++)                //显示"W2 - XX"
    {    
        LCD_w_dat(prompt11[m]);    
    }    
    //第二行显示W3和W4
    gotoxy(2,0);                     //第二行第1列
    for(m=0;m<=6;m++)                //显示"W3 - XX"
    {    
        LCD_w_dat(prompt12[m]);    
    } 
    gotoxy(2,8);                     //第二行第9列
    for(m=0;m<=6;m++)                //显示"W4 - XX"
    {    
        LCD_w_dat(prompt13[m]);    
    }    
} 


void matrixkeyscan()
{
    unsigned char KeyNumber = 0;
    
    // 扫描第一列（P1.7置低）
    P1 = 0xFF;
    P1_7 = 0;
    
    if(P1_3 == 0)  {delay(20); while(P1_3 == 0); delay(20); KeyNumber = 1;}  // 1号窗口
    if(P1_2 == 0)  {delay(20); while(P1_2 == 0); delay(20); KeyNumber = 2;}  // 2号窗口
    if(P1_1 == 0)  {delay(20); while(P1_1 == 0); delay(20); KeyNumber = 3;}  // 3号窗口
    if(P1_0 == 0)  {delay(20); while(P1_0 == 0); delay(20); KeyNumber = 4;}  // 4号窗口
    
    // 扫描第二列（P1.6置低）
    P1 = 0xFF;
    P1_6 = 0;
    
    if(P1_3 == 0)  {delay(20); while(P1_3 == 0); delay(20); KeyNumber = 5;}  
    if(P1_2 == 0)  {delay(20); while(P1_2 == 0); delay(20); KeyNumber = 6;}  
    if(P1_1 == 0)  {delay(20); while(P1_1 == 0); delay(20); KeyNumber = 7;}  
    if(P1_0 == 0)  {delay(20); while(P1_0 == 0); delay(20); KeyNumber = 8;}  
    
    // 扫描第三列（P1.5置低）
    P1 = 0xFF;
    P1_5 = 0;
    
    if(P1_3 == 0)  {delay(20); while(P1_3 == 0); delay(20); KeyNumber = 9;}  
    if(P1_2 == 0)  {delay(20); while(P1_2 == 0); delay(20); KeyNumber = 10;} 
    if(P1_1 == 0)  {delay(20); while(P1_1 == 0); delay(20); KeyNumber = 11;} 
    if(P1_0 == 0)  {delay(20); while(P1_0 == 0); delay(20); KeyNumber = 12;} 
    
    // 扫描第四列（P1.4置低）
    P1 = 0xFF;
    P1_4 = 0;
    
    if(P1_3 == 0)  {delay(20); while(P1_3 == 0); delay(20); KeyNumber = 13;} 
    if(P1_2 == 0)  {delay(20); while(P1_2 == 0); delay(20); KeyNumber = 14;} 
    if(P1_1 == 0)  {delay(20); while(P1_1 == 0); delay(20); KeyNumber = 15;}  
    if(P1_0 == 0)  {delay(20); while(P1_0 == 0); delay(20); KeyNumber = 16;} 
    
    // 根据按键值执行对应操作
    switch(KeyNumber)
    {
        case 1: jz = 1; sev1++; key(); break;  // 1号窗口叫号
        case 2: jz = 2; sev2++; key(); break;  // 2号窗口叫号
        case 3: jz = 3; sev3++; key(); break;  // 3号窗口叫号
        case 4: jz = 4; sev4++; key(); break;  // 4号窗口叫号
        case 5: k7(); break;                  // 显示窗口记录
        case 6: k6(); break;                  // 显示总叫号数
        case 7: k5(); break;                  // 显示等待人数
        default: break;
    }
}

 

///**
// * 主函数（测试用）
// * 功能：初始化硬件，循环检测按键和串口数据
// **/
//void main(void) {
//	  
//    intCon();       // 初始化串口
//	  clear_LCD();      //初始化前清屏
//    init_LCD();     // 初始化LCD
//    jhao = 0;       // 当前叫号数初始化为0
//    qhao = 0;       // 取号总数初始化为0
//    
//    // 初始显示
//    gotoxy(1, 0);
//    LCD_w_dat('S');
//    LCD_w_dat(':');
//    LCD_w_dat('0');
//    LCD_w_dat('0');
//    
//    gotoxy(2, 0);
//    LCD_w_dat('R');
//    LCD_w_dat(':');
//    LCD_w_dat('0');
//    LCD_w_dat('0');

//    while (1) {
//        matrixkeyscan();    // 检测叫号按键

//        // 仅当叫号数变化时才发送数据
//        
//        if (jhao != last_jhao) {
//            last_jhao = jhao;
//            
//            // 更新显示
//            gotoxy(1, 2);
//            LCD_w_dat(jhao / 10 + '0');
//            LCD_w_dat(jhao % 10 + '0');
//            
//            // 发送数据
//            SBUF = jhao;     // 发送号码
//            while (!TI);     // 等待发送完成
//            TI = 0;          // 清除发送标志
//        }

//        // 接收数据（仅当RI=1时处理）
//        if (RI) {
//            RI = 0;          // 清除接收标志
//            qhao = SBUF;     // 读取接收数据
//            
//            // 更新显示
//            gotoxy(2, 2);
//            LCD_w_dat(qhao / 10 + '0');
//            LCD_w_dat(qhao % 10 + '0');
//        }

//        delay1ms(200); // 适当延时，
//    }
//}

/**
 * 主函数（系统入口）
 * 功能：初始化硬件，循环检测按键和串口数据
 */
void main() 
{ 
	
    intCon();
    clear_LCD();
    init_LCD();
    jhao = qhao = dhao = sev = sev1 = sev2 = sev3 = sev4 = 0;
    last_jhao = last_qhao_rcv = 0;

    while (1) {
        matrixkeyscan();
        if (jhao != last_jhao) {
            last_jhao = jhao;
            SBUF = jhao;
            while(!TI); TI = 0;
        }
        if (RI) {
            RI = 0;
            c = SBUF;
            if (c <= 99 && c >= last_qhao_rcv && c - last_qhao_rcv <= 20) {
                qhao = c;
                last_qhao_rcv = c;
            }
        }
    }
}