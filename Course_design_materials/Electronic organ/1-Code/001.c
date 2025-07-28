#include<reg51.h>
#include<intrins.h>
typedef unsigned int  uint;   
typedef unsigned char uchar;
sbit SDA1=P0^2;//������������,��Ӧ595��14��SER
sbit SCL1=P0^1;//��λ�Ĵ���ʱ������,��Ӧ595��11��SCK
sbit SCL2=P0^0;//�洢�Ĵ���ʱ������,��Ӧ595��12��RCK
sbit W1=P0^3;
sbit W2=P0^4;
sbit k1=P1^0;
sbit k2=P1^1;
sbit k3=P1^2;
sbit k4=P1^3;
sbit k5=P1^4;
sbit k6=P1^5;
sbit k7=P1^6;
sbit D1=P2^6;//��������ģʽ
sbit D2=P2^7;//����ģʽ 
sbit k8=P3^2;//�����л���
sbit k9=P3^3;//����
sbit k10=P3^4;//����
sbit k11=P3^5;//����
sbit beep=P2^2;//�������ӿ�
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
						0x37,/*N*///����	
						0x38,/*L*///����
						0x76,/*H*///����
						0x79 /*E*/
					};//�����������
uchar code table2[]={		 
						0xFC,0xFC,0xFD,0xFD,0xFD,0xFD,0xFE,//����
						0xF9,0xF9,0xFA,0xFA,0xFB,0xFB,0xFC,//�����ĸ�8λ
						0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFF,
                	};//����Ƶ�ʱ� �Ͱ�λ
uchar code table3[]={		
						0x8E,0xED,0x44,0x6B,0xB4,0xF4,0x2D,//����
						0x21,0xDB,0x87,0xD7,0x68,0xE8,0x5B,//�����ĵ�8λ
						0x47,0x77,0xA2,0xB6,0xDA,0xFA,0x16,
					};
uchar code table4[]={
						1,2,3,1,      
						1,2,3,1,      
						3,4,5,    
						3,4,5,     
						5,6,5,4,3,1,    
						5,6,5,4,3,1,   
						1,12/*����5*/,1,     
						1,12,1
					}; 
uchar code table5[]={
						4,4,4,4,   
						4,4,4,4,     
						4,4,8,    
						4,4,8,     
						2,1,2,1,4,4,    
						2,1,2,1,4,4,   
						4,4,6,      
						4,4,6
					};
uchar i;
uchar key,aa=0;
uchar bb,cc;
bit flag=0;
//��ʱ�Ӻ���
void delay(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=340;y>0;y--);		
}
void in(uchar Data)
{
	uchar i;
	for(i=0;i<8;i++)   //ѭ��8�Σ��պ�����8λ
	{
      	Data<<=1;
	  	SCL1=CY;
	  	SDA1=1;        //�Ƚ���λ�Ĵ�������������Ϊ��    
      	_nop_();
	  	_nop_();
      	SDA1=0;
	}
}
void out()
{
	SCL2=0;   //�Ƚ��洢�Ĵ���������Ϊ��
	_nop_(); 
	SCL2=1;    //����Ϊ�ߣ�������λʱ�������أ�������ʱ��λ�Ĵ��������ݽ������ݴ洢�Ĵ�����������ʾ���ݡ�
	_nop_(); 
	SCL2=0;
}
//��ʼ���Ӻ���
void init()
{
	beep=0;
	D1=1;
	D2=0;
	EA=1;//�����ж�
	TCON=0x01;//�ⲿ�ж�0����Ϊ���ش���
	EX0=1;//���ⲿ�ж�0
	ET0=1;
	ET1=1;
	TMOD=0x11;//��ʱ��0,1�����ڶ�ʱ״̬����Ϊ��ʽ1
}
//�������ʾ�Ӻ���
void display1()
{
	in(table1[aa]);  //�ٴ�����
	out();
	W1=0;;
	delay(1);             //�ӳ�ʱ��2ms����
	W1=1;
	in(table1[cc+1]);  //�ٴ�����
	out();
	W2=0;
	delay(1);             //�ӳ�ʱ��2ms����
	W2=1; 
}
//�ߵ���ѡ���Ӻ���
void yinjie()
{
	if(k10==0)
	{	
		delay(5);
		if(k10==0)
		{   
			aa=10;
			bb=0;//����10Ϊseg[10]��ʾC
		}
	}
	if(k9==0)
	{	
		delay(5);
		if(k9==0)
		{	
			aa=11;
			bb=1;//����11Ϊseg[11]��ʾL
		}
	}
	if(k11==0)
	{	
		delay(5);
		if(k11==0)
		{	
			aa=12;
			bb=2;//����12Ϊseg[12]��ʾH
		}
	}
	if(aa==0)
	{
		aa=13;
	}
}
//���������Ӻ���
void display_music()
{	
	TH0=table2[table4[i]-1];
	TL0=table3[table4[i]-1];
	while(flag==0)
	{	
		if(i<32)
		{
			TR0=1;
			delay(57*table5[i]);
			i++;
		}
		if(i==32)
		{
			i=0;
		}
	}	
}
//����ģʽ�Ӻ���
void display_play()
{
	TR0=0;
	TR1=0;
	yinjie();
	in(table1[aa]);  //�ٴ�����
	out();
	W1=0;;
	delay(1);             //�ӳ�ʱ��2ms����
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
//������
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
//�ⲿ0�ж��Ӻ���
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
//��ʱ��0�ж��Ӻ���
void t0() interrupt 1
{	
	TR0=0;
	TH0=table2[table4[i]-1];
	TL0=table3[table4[i]-1];
	beep=~beep;
	TR0=1;
}
//��ʱ��1�ж��Ӻ���
void t1() interrupt 3
{	
	TR1=0;
	TH1=table2[7*bb+cc];
	TL1=table3[7*bb+cc];
	beep=~beep;
	TR1=1;
}
