#include<reg51.h>
#include<intrins.h>
typedef unsigned int  uint;   
typedef unsigned char uchar;
sbit SDA1=P0^2;//串行数据输入,对应595的14脚SER
sbit SCL1=P0^1;//移位寄存器时钟输入,对应595的11脚SCK
sbit SCL2=P0^0;//存储寄存器时钟输入,对应595的12脚RCK
sbit W1=P0^3;
sbit W2=P0^4;
sbit k1=P1^0;
sbit k2=P1^1;
sbit k3=P1^2;
sbit k4=P1^3;
sbit k5=P1^4;
sbit k6=P1^5;
sbit k7=P1^6;
sbit D1=P2^6;//播放音乐模式
sbit D2=P2^7;//弹奏模式 
sbit k8=P3^2;//功能切换键
sbit k9=P3^3;//低音
sbit k10=P3^4;//中音
sbit k11=P3^5;//高音
sbit beep=P2^2;//蜂鸣器接口
uchar code table1[]={   
						0x3F,/*0*/
						0x06,/*1*/
						0x5B,/*2*/
						0x4F,/*3*/
						0x66,/*4*/
						0x6D,/*5*/
						0x7D,/*6*/
						0x07,/*7*/
						0x7F,/*8*/
						0x6F,/*9*/
						0x37,/*N*///中音	
						0x38,/*L*///低音
						0x76,/*H*///高音
						0x79 /*E*/
					};//共阴极数码管
uchar code table2[]={		 
						0xFC,0xFC,0xFD,0xFD,0xFD,0xFD,0xFE,//中音
						0xF9,0xF9,0xFA,0xFA,0xFB,0xFB,0xFC,//低音的高8位
						0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,
                	};//音阶频率表 低八位
uchar code table3[]={		
						0x8E,0xED,0x44,0x6B,0xB4,0xF4,0x2D,//中音
						0x21,0xDB,0x87,0xD7,0x68,0xE8,0x5B,//低音的低8位
						0x47,0x77,0xA2,0xB6,0xDA,0xFA,0x16,
					};
uchar code table4[]={
	13,1,2,3,5,3,1,2,13,	// 让我们荡起双桨
   1,2,3,5,5,6,2,3,   // 小船儿推开波浪
   3,5,6,5,6,15,7,6,5,6,3,   // 海面倒映着美丽白塔
   1,2,3,5,1,13,1,2,3,6,5,   // 四周
					}; 
uchar code table5[]={
	 1,1,1,4,1,1,4,4,    // 让我们荡起双桨
   1,1,1,4,2,2,3,5,  // 小船儿推开波浪
   2,2,3,3,2,2,1,1,2,2,3,     // 海面倒映着美丽白塔
   1,1,3,1,2,2,1,1,1,1,3,    // 四周环绕着绿树红墙
    
						
					};
uchar i;
uchar key,aa=0;
uchar bb,cc;
bit flag=0;
//延时子函数
void delay(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=340;y>0;y--);		
}
void in(uchar Data)
{
	uchar i;
	for(i=0;i<8;i++)   //循环8次，刚好移完8位
	{
      	Data<<=1;
	  	SCL1=CY;
	  	SDA1=1;        //先将移位寄存器控制引脚置为低    
      	_nop_();
	  	_nop_();
      	SDA1=0;
	}
}
void out()
{
	SCL2=0;   //先将存储寄存器引脚置为低
	_nop_(); 
	SCL2=1;    //再置为高，产生移位时钟上升沿，上升沿时移位寄存器的数据进入数据存储寄存器，更新显示数据。
	_nop_(); 
	SCL2=0;
}
//初始化子函数
void init()
{
	beep=0;
	D1=1;
	D2=0;
	EA=1;//开总中断
	TCON=0x01;//外部中断0设置为边沿触发
	EX0=1;//开外部中断0
	ET0=1;
	ET1=1;
	TMOD=0x11;//定时器0,1工作在定时状态，均为方式1
}
//数码管显示子函数
void display1()
{
	in(table1[aa]);  //再传段码
	out();
	W1=0;;
	delay(1);             //延迟时间2ms以内
	W1=1;
	in(table1[cc+1]);  //再传段码
	out();
	W2=0;
	delay(1);             //延迟时间2ms以内
	W2=1; 
}
//高低音选择子函数
void yinjie()
{
	if(k10==0)
	{	
		delay(5);
		if(k10==0)
		{   
			aa=10;
			bb=0;//返回10为seg[10]显示C
		}
	}
	if(k9==0)
	{	
		delay(5);
		if(k9==0)
		{	
			aa=11;
			bb=1;//返回11为seg[11]显示L
		}
	}
	if(k11==0)
	{	
		delay(5);
		if(k11==0)
		{	
			aa=12;
			bb=2;//返回12为seg[12]显示H
		}
	}
	if(aa==0)
	{
		aa=13;
	}
}
//播放音乐子函数
void display_music()
{	
	TH0=table2[table4[i]-1];
	TL0=table3[table4[i]-1];
	while(flag==0)
	{	
		if(i<38)
		{
			TR0=1;
			delay(100*table5[i]);
			i++;
		}
		if(i==38)
		{
			i=0;
		}
	}	
}
//演奏模式子函数
void display_play()
{
	TR0=0;
	TR1=0;
	yinjie();
	in(table1[aa]);  //再传段码
	out();
	W1=0;;
	delay(1);             //延迟时间2ms以内
	W1=1;
	if(aa!=13&&flag==1)
	{	
		if(k1==0)
		{	
			TH1=table2[7*bb+cc];
			TL1=table3[7*bb+cc];
			TR1=1;	
			while(k1==0)
			{	
				cc=0;
				display1();
			}
		}
		if(k2==0)
		{	
			TH1=table2[7*bb+cc];
			TL1=table3[7*bb+cc];
			TR1=1;
			while(k2==0)
			{	
				cc=1;
				display1();
			}
		}	
		if(k3==0)
		{		
			TH1=table2[7*bb+cc];
			TL1=table3[7*bb+cc];
			TR1=1;
			while(k3==0)
			{	
				cc=2;
				display1();
			}
		}
		if(k4==0)
		{
			TH1=table2[7*bb+cc];
			TL1=table3[7*bb+cc];
			TR1=1;
			while(k4==0)
			{	
				cc=3;
				display1();		
			}
		}
		if(k5==0)
		{	
			TH1=table2[7*bb+cc];
			TL1=table3[7*bb+cc];
			TR1=1;
			while(k5==0)
			{
				cc=4;
				display1();
			}
		}
		if(k6==0)
		{	
			TH1=table2[7*bb+cc];
			TL1=table3[7*bb+cc];
			TR1=1;
			while(k6==0)
			{
				cc=5;
				display1();							
			}
		}
		if(k7==0)
		{		
			TH1=table2[7*bb+cc];
			TL1=table3[7*bb+cc];
			TR1=1;	
			while(k7==0)
			{
				cc=6;
				display1();	
			}
		}		
	}
}
//主函数
void main()
{
	init();
	while(1)
	{	
		if(flag==0)
			display_music();
		else
			display_play();
	}
}
//外部0中断子函数
void wb0() interrupt 0 
{
	if(k8==0)
	{
		delay(5);
		while(k8==0);
		flag=~flag;
		D1=~D1;
		D2=~D2;
	}		
}
//定时器0中断子函数
void t0() interrupt 1
{	
	TR0=0;
	TH0=table2[table4[i]-1];
	TL0=table3[table4[i]-1];
	beep=~beep;
	TR0=1;
}
//定时器1中断子函数
void t1() interrupt 3
{	
	TR1=0;
	TH1=table2[7*bb+cc];
	TL1=table3[7*bb+cc];
	beep=~beep;
	TR1=1;
}
