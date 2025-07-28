#include <reg51.h>
#include <intrins.h>
#define uchar unsigned char 
#define uint unsigned int 
sbit RS=P2^0;     //1:数据输入,0:指令输入 
sbit RW=P2^1;    //1: 读出,0:写入 
sbit E=P2^2;      //1:有效,0:无效
sbit key0=P1^0; 
sbit key1=P1^1; 
sbit key2=P1^2; 
sbit key3=P1^3; 
sbit key4=P1^4; 
sbit key5=P1^5;
sbit key6=P1^6; 
sbit key7=P1^7; 
sbit BEEP=P2^6;
uint sev=0; 
uint sev1=0; 
uint sev2=0; 
uint sev3=0; 
uint sev4=0;
uint m=0,i=0;
uint jhao=1,qhao=0,dhao=0;                              //排队叫号                                 //窗口消的号码 
uint jz=0;
uchar data test[]="Hello!";                    //初始显示的字符 
uchar data prompt2[]="No.00 come to No";     //1062第一行显示的字符 
uchar data prompt3[]="0.window,Please!";     //1062第二行显示的字符 
uchar data prompt8[]="00 is left"; 
uchar data prompt9[]="All is 00"; 
uchar data prompt10[]="W1 - 00"; 
uchar data prompt11[]="W2 - 00";
uchar data prompt12[]="W3 - 00"; 
uchar data prompt13[]="W4 - 00"; 
                                  
void intCon()  
{    
	  EA=0;   	   //关总中断
	  SCON=0x50;   //0101 0000,SM0 SM1=01表示选择工作方式1；SM2 REN=01表示串行口多机通讯控制位，串行口允许接收（从外部接收数据）
	  PCON=0X00;   //电源控制寄存器 最高位为SMOD，为0，表示波特率不加倍，为1，表示波特率加倍
	  TMOD=0x20;   //0010 0000,GATE=0,以运行控制位TR启动定时器；采用定时1工作方式2
	  TH1=0xfd;    //波特率9600=2^smod*（11.05926*10^6）/(32*12*(2^8-X))
	  TL1=0xfd;  
	  TR1=1;  		 //计数器工作
  } 
//
 void delay(int n)       //延时子程序 
{ 
int k,j;  
 for(k=0;k<=n;k++)  
 for(j=0;j<=10;j++); 
} 
void delay1s(int n)     //延时子程序
 { 
int k,j;  
 for(k=0;k<=n;k++)  
 for(j=0;j<=120;j++); 
}  
void SPEAKER(uint x)   //蜂鸣器程序
 {   
for(i=0;i<=200;i++)    
 {        
delay(x);  
 BEEP=~BEEP;  
 }     
 BEEP=1; }
 /**************LCD程序**********/
 void delay1ms(unsigned int ms)   //延时的时间
{ 
uint k,j;  
 for(k=0;k<ms;k++)   
 for(j=0;j<100;j++); 
}
 void LCD_w_com(unsigned com)  // 写入指令 
 {  
RW=0;  
RS=0;
E=1;  
P0=com; 
delay1ms(40);  
E=0; 
RW=1;
} 
void LCD_w_dat(uchar dat)   // 写入数据 
{ 

 RW=0; 
 RS=1;  
E=1; 
P0=dat; 
delay1ms(40); 
E=0;  
RW=1; 
}  
void gotoxy(unsigned x,unsigned y)  {  
 if(x==1)  
LCD_w_com(0x80+y); 
 else 
LCD_w_com(0xC0+y); 
} 
void clear_LCD(void) 
{ 
LCD_w_com(0x01);       //清屏指令  
LCD_w_com(0x02);      // 光标归位
} 
void init_LCD(void)      // 初始化LCD 
{  
LCD_w_com(0x38);     // LCD为两行显示             
LCD_w_com(0x0c);     // 显示字符  关闭光标 
LCD_w_com(0x06);     // 输入方式设置 
 gotoxy(1,0); 
 for(i=0;i<=5;i++)  
{  
 LCD_w_dat(test[i]); 
 } 
} 
/*********叫号机 按一次一号键变量+1最大为10*************/   
void key()
{         
  if(qhao>jhao)     
{       
 SPEAKER(11); 
     delay(200);    
SPEAKER(15); 
clear_LCD();      

sev++;   
prompt2[3]=(jhao+1)/10+'0';               
prompt2[4]=(jhao+1)%10+'0';      
prompt3[0]=jz+'0';                
gotoxy(1,0);      
for(m=0;m<=15;m++)      
{      
LCD_w_dat(prompt2[m]);      
}      
gotoxy(2,0);      
for(m=0;m<=15;m++)      
{
LCD_w_dat(prompt3[m]);     
 }         
 jhao++; 
}
}

void k5()
{
clear_LCD();  
dhao=qhao-jhao;
prompt8[0]=dhao/10+'0';  
prompt8[1]=dhao%10+'0';  
gotoxy(1,0);  
for(m=0;m<=9;m++)  
{  
LCD_w_dat(prompt8[m]);     
}       
}

void k6()
 {     
clear_LCD();   
prompt9[7]=(sev)/10+'0';   
prompt9[8]=(sev)%10+'0';   
gotoxy(1,0);   
for(m=0;m<=8;m++)   
{   
LCD_w_dat(prompt9[m]); 
}  
}

void k7()
{
clear_LCD();        
prompt10[5]=sev1/10+'0';    
prompt10[6]=sev1%10+'0';    
prompt11[5]=sev2/10+'0';    
prompt11[6]=sev2%10+'0';    
prompt12[5]=sev3/10+'0';    
prompt12[6]=sev3%10+'0';    
prompt13[5]=sev4/10+'0';    
prompt13[6]=sev4%10+'0';     
gotoxy(1,0);    
for(m=0;m<=6;m++)    
{    
LCD_w_dat(prompt10[m]);    
}        
gotoxy(1,8);    
for(m=0;m<=6;m++)    
{    
LCD_w_dat(prompt11[m]);    
}    
gotoxy(2,0);    
for(m=0;m<=6;m++)    
{    
LCD_w_dat(prompt12[m]);    
} 
 gotoxy(2,8);    
for(m=0;m<=6;m++)    
{    
LCD_w_dat(prompt13[m]);    
}    
} 

 //矩阵键盘扫描函数
void matrixkeyscan()
{	
	unsigned char temp;	
	P1=0xff; //先向P1 口写1；端口读状态
	P1=0xf0; //列送di电平，行送gao电平
	temp=P1;
	if(temp!=0xf0)//判断是否有键按下，若有键按下则temp不等于0xf0
	{
		delay(10);	// 防抖动
		if(temp!=0xf0)  
		{
			P1=0xef;  //扫描第一行
			temp=P1;
			switch(temp)
			{
		 		case(0xe7):{jz=1;sev1++; key();} ;break; //第一行第一列
				case(0xeb):{jz=2;sev2++; key();} ;break; //第一行第二列
				case(0xed):{jz=3;sev3++; key();} ;break;  //第一行第三列
				case(0xee):{jz=4;sev4++; key();} ;break;  //第一行第四列
			    
             }
			P1=0xdf;
			temp=P1;
			switch(temp)   //扫描第二行
			{
				case(0xd7):k5();break;	//第二行第一列

				case(0xdb):k6();break;	//第二行第二列

				case(0xdd):k7();break;	//第二行第三列
				
			}
		}
	}
} 
void main() 
{ 
    intCon(); 
    clear_LCD();   
    init_LCD(); 
		jhao=0;
   while(1)  //一直执行
  {   
      
    matrixkeyscan(); 
	  SBUF=jhao;  //叫号送发送存储器  
	  while(!TI);  	//若一帧数据发送完毕,则TI=1，在将 TI置为0，让其处于接收状态  
	  TI=0;     
      if(RI)   //若接收完一帧数据，则RI=1
	  {    
	    RI=0;  //将RI置0，让其处于接收状态  
	    qhao=SBUF;//取号数从接收存储器中取得  
	  }	  
   } 
}