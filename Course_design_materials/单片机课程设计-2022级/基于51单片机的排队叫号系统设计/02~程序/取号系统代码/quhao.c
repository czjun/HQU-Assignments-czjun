#include <reg51.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int  
sbit RS=P2^0; 
sbit RW=P2^1; 
sbit E=P2^2;                                     
sbit K1=P1^0;
sbit K2=P1^1;  
uint m=0,i=0; 
uchar data test[]="Welcome!";             //��ʼ��ʾ���ַ� 
uchar data prompt1[]="Your No.is 00!";    //1062��һ����ʾ���ַ� 
uchar data prompt8[]="00 is left"; 
uchar data prompt4[]="The queue is";      //1062��һ����ʾ���ַ� 
uchar data prompt5[]="full,please wait";  //1062�ڶ�����ʾ���ַ� 

unsigned long qhao=1,dhao=1,jhao=0;    //�ֱ�Ϊȡ�����������ȴ������� �е������� 
void intCon() 
 {  
	 EA=0;	   //�����ж�
	 SCON=0x50;    //0101 0000,SM0 SM1=01��ʾѡ������ʽ1��SM2 REN=01��ʾ���пڶ��ͨѶ����λ�����п�������գ����ⲿ�������ݣ�
	 PCON=0X00;   //��Դ���ƼĴ��� ���λΪSMOD��Ϊ0����ʾ�����ʲ��ӱ���Ϊ1����ʾ�����ʼӱ�
	 TMOD=0x20;   	//0010 0000,GATE=0,�����п���λTR������ʱ�������ö�ʱ1������ʽ2
	 TH1=0xfd;    //������Ϊ9600bit/s
	 TL1=0xfd;  
 	 TR1=1;  	 //����������
 } 
void delay(int n)   //��ʱ���� 
{ 
int k,j;   
for(k=0;k<=n;k++)   
for(j=0;j<=10;j++); 
} 
void delay1s(int n)   //��ʱ���� 
{
 int k,j;   
for(k=0;k<=n;k++)   
for(j=0;j<=120;j++); 
}       
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
 for(i=0;i<=7;i++)  
{  
 LCD_w_dat(test[i]); 
 } 
} 

// �ж�ȡ�ż�������
void panduan_01() 	
{  
   if(K1==0)  
    { delay(10);    //����
      if(K1==0)   
			{    
          if(qhao>99)  
					{
						EA=0;  
						delay1s(5000);      
						clear_LCD();      
						gotoxy(1,2);   
					for(m=0;m<=11;m++)   
					{    
						LCD_w_dat(prompt4[m]);      
					}    
					gotoxy(2,0);   
					for(m=0;m<=15;m++)   
					{    
						LCD_w_dat(prompt5[m]);  //��ʾ��������      
					}     
					} 	
					else
					{
						clear_LCD();     
						prompt1[11]=(qhao)/10+'0';       
						prompt1[12]=(qhao)%10+'0';    
						qhao++;    
						gotoxy(1,0);  
						for(m=0;m<=13;m++)   
						{   
							LCD_w_dat(prompt1[m]); //"Your No.is 00!"; 
						}   
					} 
					dhao=qhao-jhao;    //�ȴ�������=����-�е��� 
					prompt8[0]=(dhao-1)/10+'0';  
					prompt8[1]=(dhao-1)%10+'0';  
					gotoxy(2,0);				
					for(m=0;m<=9;m++)  
					{  
						LCD_w_dat(prompt8[m]);     
					} 
			}
	}
	 while(K1==0);   
	
}
//�жϵȴ��������������� ��������ʾ�ȴ����������ɿ�����ʾȡ���ĺ��� 
void panduan_02() 
{   
   if(K2==0)  
    { delay(10);   //����
      if(K2==0)   
			{ 
					dhao=qhao-jhao;    //�ȴ�������=����-�е���
					clear_LCD();  
					prompt8[0]=dhao/10+'0';  
					prompt8[1]=dhao%10+'0';  
					gotoxy(2,0);				
				for(m=0;m<=9;m++)  
				{  
					LCD_w_dat(prompt8[m]);     
				}    

	   }  
	 }   
	
}  
void main(void) 
{ 
    clear_LCD();       
		init_LCD();
    intCon(); 
		jhao=0;
    while(1)  
     {   
		if(K1==0)  
	    panduan_01();
			SBUF=qhao;		//ȡ���ͷ��ʹ洢��
		while(!TI);   	//��һ֡���ݷ������,��TI=1���ڽ� TI��Ϊ0�����䴦�ڽ���״̬
		TI=0;
		if(RI) //��������һ֡���ݣ���RI=1  
	    {    
		    RI=0; //��RI��0�����䴦�ڽ���״̬   
		    jhao=SBUF;  //�к����ӽ��մ洢����ȡ��  
	    }		
		 if(K2==0)	   
				panduan_02();  	   
	   } 	   
}