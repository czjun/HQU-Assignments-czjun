#include <reg51.h>       //51��Ƭ������ͷ�ļ�
#include <intrins.h>     //�����ڲ�����
#define uchar unsigned char  //�궨���޷����ַ���
#define uint unsigned int   //�궨���޷�������


static uint last_qhao = 0;

// ���п�������������Ŷ���
sbit RS=P2^6;      // LCD RS��P2^6
sbit RW=P2^5;      // LCD RW��P2^5
sbit E=P2^7;       // LCD E��P2^7
sbit K1=P3^2;      // ȡ�ż�K1��P3^2�����ж�������1��
sbit K2=P3^3;      // ��ѯ��K2��P3^3�����ж�������2��
 
sbit P3_0 = P3^0;    //���п�
sbit P3_1 = P3^1;    // 

uint m=0,i=0;  //ѭ������
//LCD��ʾ�ַ�������ʼ����ʾ��Ϣ��
uchar data test[]="Please Take K3!";             //��ʼ����ʾ"Welcome!"
uchar data prompt1[]="Your No.is 00!";    //ȡ�ųɹ���ʾ��"Your No.is XX!"��
uchar data prompt8[]="00 is left";        //�ȴ�������ʾ��"XX is left"��
uchar data prompt4[]="The queue is";      //��������ʾ��һ��
uchar data prompt5[]="full,please wait";  //��������ʾ�ڶ���

//ϵͳ���ı���
unsigned  qhao=1;  //ȡ����������1��ʼ��ÿȡһ��+1��
uint dhao=1;           //�ȴ�������qhao - jhao��
uint jhao=0;           //�ѽк������ӽкŻ�ͨ�����ڽ��գ�


/**
 * ���ڳ�ʼ����������кŻ�ͨ�ţ�ͬ�����ݣ�
 * ˵��������ͬ�кŻ���ȷ�������ʺ͹�����ʽһ�£���������ͨ�ţ�
 */
void intCon() 
{  
    EA=0;         //�ر����жϣ���ѯ��ʽ�����ڣ�
    SCON=0x50;    //0101 0000��������ʽ1��10λ�첽�շ������������
    PCON=0X00;    //SMOD=0�������ʲ��ӱ���
    TMOD=0x20;    //��ʱ��1��ʽ2���Զ���װ�أ����������ʣ�
    TH1=0xfd;     //9600�����ʳ�ֵ��ͬ�кŻ���ȷ��ͨ��һ�£�
    TL1=0xfd;  
    TR1=1;        //������ʱ��1�����������ʣ�
		P3_0=1;
	  P3_1=1;
} 

/**
 * ����ʱ���������ڰ���������
 * @param n����ʱ����
 */
void delay(int n)   
{ 
    int k,j;   
    for(k=0;k<=n;k++)   
        for(j=0;j<=10;j++); 
} 

/**
 * ����ʱ����������״̬���֣�
 * @param n����ʱ����
 */
void delay1s(int n)   
{
    int k,j;   
    for(k=0;k<=n;k++)   
        for(j=0;j<=120;j++); 
}       


/**************LCD1602��غ�����ͬ�кŻ��������߼���**********/

/**
 * ���뼶��ʱ������LCDʱ��
 * @param ms����ʱ������
 */
void delay1ms(unsigned int ms)   
{ 
    uint k,j;  
    for(k=0;k<ms;k++)   
        for(j=0;j<100;j++); 
}

/**
 * ��LCDд��ָ��
 * @param com��ָ�����
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
 * ��LCDд�����ݣ���ʾ�ַ���
 * @param dat���ַ�ASCII��
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
 * ����LCD���λ��
 * @param x���кţ�1/2����@param y���кţ�0-15��
 */
void gotoxy(unsigned x,unsigned y)  
{  
    if(x==1)  
        LCD_w_com(0x80 + y); 
    else 
        LCD_w_com(0xC0 + y); 
} 

/**
 * ���LCD��ʾ
 */
void clear_LCD(void) 
{ 
    LCD_w_com(0x01);  //����
    LCD_w_com(0x02);  //����λ
} 

/**
 * ��ʼ��LCD1602
 */
void init_LCD(void)      
{  
    LCD_w_com(0x38);  //16x2��ʾ��8λ�ӿ�
    LCD_w_com(0x0c);  //��ʾ��������
    LCD_w_com(0x06);  //����Զ�����
    gotoxy(1,0);      //��λ��һ��
    //��ʾ��ʼ����ӭ��"Welcome!"
    for(i=0;i<=14;i++)  
    {  
        LCD_w_dat(test[i]); 
    } 
} 


/**
 * ȡ�ż���������K1����ʱִ�У�
 * ���ܣ������º��룬��ʾȡ�Ž��������������ʾ������
 */
void panduan_01() 	
{  
    if(K1 == 0)          //��⵽K1����
    {  
        delay(10);       //10ms���������ⶶ�����У�
        if(K1 == 0)      //�ٴμ�⣬ȷ�ϰ���
        {    
            //�ж�ȡ�����Ƿ񳬹����ޣ�99��������λ�������
            if(qhao > 99)  
            {
                EA=0;    //�ر��жϣ����ã�
                delay1s(5000);  //��ʱ������ʾ
                clear_LCD();    //����
                gotoxy(1,2);    //��һ�е�3�У�������ʾ��
                //��ʾ��������ʾ��һ��"The queue is"
                for(m=0;m<=11;m++)   
                {    
                    LCD_w_dat(prompt4[m]);      
                }    
                gotoxy(2,0);    //�ڶ���
                //��ʾ��������ʾ�ڶ���"full,please wait"
                for(m=0;m<=15;m++)   
                {    
                    LCD_w_dat(prompt5[m]);      
                }     
            } 	
            else  //δ�������ޣ�����ȡ��
            {
                clear_LCD();     //����
                //����ȡ����ʾ�ĺ��루"Your No.is XX!"��
                prompt1[11] = (qhao)/10 + '0';  //ʮλ
                prompt1[12] = (qhao)%10 + '0';  //��λ
                qhao++;        //ȡ����+1����һ����ȡ���ã�
                gotoxy(1,0);   //��λ��һ��
                //��ʾȡ�Ž����"Your No.is XX!"��
                for(m=0;m<=13;m++)   
                {   
                    LCD_w_dat(prompt1[m]); 
                }   
            } 
            //����ȴ���������ǰȡ���� - �ѽк�����
            dhao = qhao - jhao;    
            //��ʾ�ȴ���������ǰȡ����ǰ��ĵȴ�������
            prompt8[0] = (dhao - 1)/10 + '0';  //ʮλ
            prompt8[1] = (dhao - 1)%10 + '0';  //��λ
            gotoxy(2,0);                       //��λ�ڶ���
            for(m=0;m<=9;m++)                  //��ʾ"XX is left"
            {  
                LCD_w_dat(prompt8[m]);     
            } 
        }
    }
    while(K1 == 0);  //�ȴ������ɿ������ⳤ���ظ�������
}



/**
 * ��ѯ����������K2����ʱִ�У�
 * ���ܣ���ʾ��ǰ�ȴ�������ȡ������ - �ѽк�����
 */
void panduan_02() 
{   
    if(K2 == 0)      //��⵽K2����
    {  
        delay(10);   //10ms����
        if(K2 == 0)  //ȷ�ϰ���
        { 
            dhao = qhao - jhao;    //����ȴ�����
            clear_LCD();           //����
            //���µȴ�������ʾ
            prompt8[0] = dhao / 10 + '0';  //ʮλ
            prompt8[1] = dhao % 10 + '0';  //��λ
            gotoxy(2,0);                  //��λ�ڶ���
            for(m=0;m<=9;m++)             //��ʾ"XX is left"
            {  
                LCD_w_dat(prompt8[m]);     
            }    
        }  
    }   
}  


/**
 * ��������ϵͳ��ڣ�
 * ���ܣ���ʼ��Ӳ����ѭ����ⰴ���ʹ�������
 */
void main(void) 
{ 
	
    clear_LCD();       //��ʼ������
    init_LCD();        //��ʼ��LCD
    intCon();          //��ʼ�����ڣ���кŻ�ͨ�ţ�
    jhao = 0;          //��ʼ���ѽк���Ϊ0
    while(1) {
    if (K1==0) panduan_01();
    // ֻ���º�ʱ�ŷ�
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