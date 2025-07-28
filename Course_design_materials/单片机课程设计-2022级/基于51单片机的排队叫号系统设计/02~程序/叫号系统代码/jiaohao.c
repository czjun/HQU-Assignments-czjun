#include <reg51.h>
#include <intrins.h>
#define uchar unsigned char 
#define uint unsigned int 
sbit RS=P2^0;     //1:��������,0:ָ������ 
sbit RW=P2^1;    //1: ����,0:д�� 
sbit E=P2^2;      //1:��Ч,0:��Ч
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
uint jhao=1,qhao=0,dhao=0;                              //�Ŷӽк�                                 //�������ĺ��� 
uint jz=0;
uchar data test[]="Hello!";                    //��ʼ��ʾ���ַ� 
uchar data prompt2[]="No.00 come to No";     //1062��һ����ʾ���ַ� 
uchar data prompt3[]="0.window,Please!";     //1062�ڶ�����ʾ���ַ� 
uchar data prompt8[]="00 is left"; 
uchar data prompt9[]="All is 00"; 
uchar data prompt10[]="W1 - 00"; 
uchar data prompt11[]="W2 - 00";
uchar data prompt12[]="W3 - 00"; 
uchar data prompt13[]="W4 - 00"; 
                                  
void intCon()  
{    
	  EA=0;   	   //�����ж�
	  SCON=0x50;   //0101 0000,SM0 SM1=01��ʾѡ������ʽ1��SM2 REN=01��ʾ���пڶ��ͨѶ����λ�����п�������գ����ⲿ�������ݣ�
	  PCON=0X00;   //��Դ���ƼĴ��� ���λΪSMOD��Ϊ0����ʾ�����ʲ��ӱ���Ϊ1����ʾ�����ʼӱ�
	  TMOD=0x20;   //0010 0000,GATE=0,�����п���λTR������ʱ�������ö�ʱ1������ʽ2
	  TH1=0xfd;    //������9600=2^smod*��11.05926*10^6��/(32*12*(2^8-X))
	  TL1=0xfd;  
	  TR1=1;  		 //����������
  } 
//
 void delay(int n)       //��ʱ�ӳ��� 
{ 
int k,j;  
 for(k=0;k<=n;k++)  
 for(j=0;j<=10;j++); 
} 
void delay1s(int n)     //��ʱ�ӳ���
 { 
int k,j;  
 for(k=0;k<=n;k++)  
 for(j=0;j<=120;j++); 
}  
void SPEAKER(uint x)   //����������
 {   
for(i=0;i<=200;i++)    
 {        
delay(x);  
 BEEP=~BEEP;  
 }     
 BEEP=1; }
 /**************LCD����**********/
 void delay1ms(unsigned int ms)   //��ʱ��ʱ��
{ 
uint k,j;  
 for(k=0;k<ms;k++)   
 for(j=0;j<100;j++); 
}
 void LCD_w_com(unsigned com)  // д��ָ�� 
 {  
RW=0;  
RS=0;
E=1;  
P0=com; 
delay1ms(40);  
E=0; 
RW=1;
} 
void LCD_w_dat(uchar dat)   // д������ 
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
LCD_w_com(0x01);       //����ָ��  
LCD_w_com(0x02);      // ����λ
} 
void init_LCD(void)      // ��ʼ��LCD 
{  
LCD_w_com(0x38);     // LCDΪ������ʾ             
LCD_w_com(0x0c);     // ��ʾ�ַ�  �رչ�� 
LCD_w_com(0x06);     // ���뷽ʽ���� 
 gotoxy(1,0); 
 for(i=0;i<=5;i++)  
{  
 LCD_w_dat(test[i]); 
 } 
} 
/*********�кŻ� ��һ��һ�ż�����+1�����Ϊ10*************/   
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

 //�������ɨ�躯��
void matrixkeyscan()
{	
	unsigned char temp;	
	P1=0xff; //����P1 ��д1���˿ڶ�״̬
	P1=0xf0; //����di��ƽ������gao��ƽ
	temp=P1;
	if(temp!=0xf0)//�ж��Ƿ��м����£����м�������temp������0xf0
	{
		delay(10);	// ������
		if(temp!=0xf0)  
		{
			P1=0xef;  //ɨ���һ��
			temp=P1;
			switch(temp)
			{
		 		case(0xe7):{jz=1;sev1++; key();} ;break; //��һ�е�һ��
				case(0xeb):{jz=2;sev2++; key();} ;break; //��һ�еڶ���
				case(0xed):{jz=3;sev3++; key();} ;break;  //��һ�е�����
				case(0xee):{jz=4;sev4++; key();} ;break;  //��һ�е�����
			    
             }
			P1=0xdf;
			temp=P1;
			switch(temp)   //ɨ��ڶ���
			{
				case(0xd7):k5();break;	//�ڶ��е�һ��

				case(0xdb):k6();break;	//�ڶ��еڶ���

				case(0xdd):k7();break;	//�ڶ��е�����
				
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
   while(1)  //һֱִ��
  {   
      
    matrixkeyscan(); 
	  SBUF=jhao;  //�к��ͷ��ʹ洢��  
	  while(!TI);  	//��һ֡���ݷ������,��TI=1���ڽ� TI��Ϊ0�����䴦�ڽ���״̬  
	  TI=0;     
      if(RI)   //��������һ֡���ݣ���RI=1
	  {    
	    RI=0;  //��RI��0�����䴦�ڽ���״̬  
	    qhao=SBUF;//ȡ�����ӽ��մ洢����ȡ��  
	  }	  
   } 
}