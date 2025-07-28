#include <reg51.h>       //51��Ƭ������ͷ�ļ��������Ĵ�������
#include <intrins.h>     //����_nop_���ڲ�������������δֱ��ʹ�ã�
#define uchar unsigned char  //�궨�壺uchar����unsigned char���޷����ַ��ͣ�
#define uint unsigned int   //�궨�壺uint����unsigned int���޷������ͣ�



// ���п�������������Ŷ���
sbit RS=P2^6;      // LCD RS��P2^6������LCD���ƿڣ�
sbit RW=P2^5;      // LCD RW��P2^5
sbit E=P2^7;       // LCD E��P2^7

// �������
sbit P1_0 = P1^0;    // ��1
sbit P1_1 = P1^1;    // ��2
sbit P1_2 = P1^2;    // ��3
sbit P1_3 = P1^3;    // ��4
sbit P1_4 = P1^4;    // ��1
sbit P1_5 = P1^5;    // ��2
sbit P1_6 = P1^6;    // ��3
sbit P1_7 = P1^7;    // ��4

sbit P3_0 = P3^0;    //���п�
sbit P3_1 = P3^1;    // 

sbit BEEP=P2^5;    // ��������P2^5�����п�������������ţ�
static uchar last_jhao = 0;
static uchar last_qhao_rcv = 0;  // �ϴν��յ�ȡ��
//ϵͳ��������
uchar sev=0;      //�ܽкż��������ã� 
uchar sev1=0;     //1�Ŵ��ڽкż���
uchar sev2=0;     //2�Ŵ��ڽкż���
uchar sev3=0;     //3�Ŵ��ڽкż���
uchar sev4=0;     //4�Ŵ��ڽкż���
uchar m=0,i=0;    //ѭ������
uchar jhao=0;     //��ǰ�е��ĺ��루��ʼΪ1��
uchar qhao=0;     //ȡ����������ȡ�Ż�ͨ�����ڽ��գ�
uchar dhao=0;     //�ȴ�������qhao - jhao��
uchar jz=0;       //��ǰ�кŵĴ��ڣ�1-4��
uchar c=0;
//LCD��ʾ�ַ������壨��ʼ����ʾ��Ϣ��
uchar data test[]="Hello!";                    //LCD��ʼ����ʾ�ַ�
uchar data prompt2[]="No.00 come to No";      //�к���ʾ��һ�У�"No.XX come to No"��
uchar data prompt3[]="0.window,Please!";      //�к���ʾ�ڶ��У�"X.window,Please!"��
uchar data prompt8[]="00 is left";            //�ȴ�������ʾ��"XX is left"��
uchar data prompt9[]="All is 00";             //��ȡ������ʾ��"All is XX"��
uchar data prompt10[]="W1 - 00";              //1�Ŵ��ڽкż�¼
uchar data prompt11[]="W2 - 00";              //2�Ŵ��ڽкż�¼
uchar data prompt12[]="W3 - 00";              //3�Ŵ��ڽкż�¼
uchar data prompt13[]="W4 - 00";              //4�Ŵ��ڽкż�¼


/**
 * ���ڳ�ʼ��������������ȡ�Ż�ͨ�ţ�ͬ��ȡ�����ͽк�����
 */
void intCon()  
{    
    EA=0;        //�ر����жϣ��ݲ�ʹ���жϣ����ò�ѯ��ʽ�����ڣ�
    //���ڿ��ƼĴ������ã�������ʽ1��������գ�
    SCON=0x50;   //0101 0000��SM0=0,SM1=1��������ʽ1��10λ�첽�շ���8λ����+��ʼ/ֹͣλ��
    PCON=0X00;   //��Դ���ƼĴ�����SMOD=0�������ʲ��ӱ���
    //��ʱ��1���ã����ڲ������ڲ����ʣ�
    TMOD=0x20;   //0010 0000����ʱ��1�����ڷ�ʽ2��8λ�Զ���װ�أ��ȶ����������ʣ�
                  //������9600���㣺11.0592MHz�����£���ʽ2������=����Ƶ��/(32*12*(256-TH1))
    TH1=0xfd;    //װ���ֵ��9600�����ʶ�Ӧ��ֵ��0xfd��
    TL1=0xfd;    //��ʽ2��TL1������Զ�����TH1��ֵ
    TR1=1;       //������ʱ��1����ʼ���������ʣ�
	  P3_0=1;
	  P3_1=1;
} 

/**
 * ����ʱ���������ڰ����������򵥵ȴ���
 * @param n����ʱ������nԽ����ʱԽ����
 */
void delay(int n)       
{ 
    int k,j;  
    for(k=0;k<=n;k++)  
        for(j=0;j<=10;j++);  //�ڲ�ѭ��Լ10�Σ�������ʱ�϶�
} 

/**
 * �ϳ���ʱ����������״̬������ʾ��
 * @param n����ʱ����
 */
void delay1s(int n)     
{ 
    int k,j;  
    for(k=0;k<=n;k++)  
        for(j=0;j<=120;j++);  //��delay��ʱ����
}  

/**
 * �����������������к�ʱ��ʾ��
 * @param x����ʱ����������Ƶ�ʣ�xԽСƵ��Խ�ߣ�
 */
void SPEAKER(uint x)   
{   
    for(i=0;i<=200;i++)    //����200�Σ����Ʒ���ʱ����
    {        
        delay(x);          //��ʱ��������
        BEEP=~BEEP;        //���������ŵ�ƽ��ת����������������
    }     
    BEEP=1;                //����������رշ��������ߵ�ƽ��������
}


/**************LCD1602��ʾ��غ���**********/

/**
 * ���뼶��ʱ����������LCD����ʱ������LCD��Ӧʱ�䣩
 * @param ms����ʱ��������ԼΪʵ�ʺ�������
 */
void delay1ms(unsigned int ms)   
{ 
    uint k,j;  
    for(k=0;k<ms;k++)   
        for(j=0;j<100;j++);  //ѭ��Լ100�Σ�����Լ10us������Լ1ms
}

/**
 * ��LCDд��ָ�����LCD����ģʽ��
 * @param com��ָ����루��������������ʾģʽ�ȣ�
 */
void LCD_w_com(unsigned com)  
{  
    RW=0;          //RW=0��д�����
    RS=0;          //RS=0��д��ָ��
    E=1;           //E=1��ʹ���ź���Ч��LCD��ȡ���ݣ�
    P0=com;        //P0�����ָ�����
    delay1ms(40);  //�ȴ�LCD��Ӧ�������輸ʮus��
    E=0;           //E=0������д��
    RW=1;          //�ָ�RWΪ1������״̬��
} 

/**
 * ��LCDд�����ݣ���ʾ�ַ���
 * @param dat��Ҫ��ʾ���ַ���ASCII�룩
 */
void LCD_w_dat(uchar dat)   
{ 
    RW=0;          //RW=0��д�����
    RS=1;          //RS=1��д������
    E=1;           //E=1��ʹ���ź���Ч
    P0=dat;        //P0������ַ�����
    delay1ms(40);  //�ȴ�LCD��Ӧ
    E=0;           //����д��
    RW=1;          //�ָ�����״̬
}  

/**
 * ����LCD���λ��
 * @param x���кţ�1����һ�У�2���ڶ��У�
 * @param y���кţ�0-15����Ӧ16�У�
 */
void gotoxy(unsigned x,unsigned y)  
{  
    if(x==1)                  //��һ����ʼ��ַΪ0x80
        LCD_w_com(0x80 + y);  //0x80 | �кţ���0x80+0��ʾ��һ�е�1�У�
    else                      //�ڶ�����ʼ��ַΪ0xC0
        LCD_w_com(0xC0 + y);  //0xC0 | �кţ���0xC0+0��ʾ�ڶ��е�1�У�
} 

/**
 * ���LCD��ʾ��������λ
 */
void clear_LCD(void) 
{ 
    LCD_w_com(0x01);  //����ָ�0x01�����������ʾ��
    LCD_w_com(0x02);  //����λָ�0x02�����ص���ʼλ�ã�
} 

/**
 * ��ʼ��LCD1602��������ʾģʽ�����صȣ�
 */
void init_LCD(void)      
{  
    LCD_w_com(0x38);  //0x38������16x2��ʾ��2�У�ÿ��16�ַ�����8λ���ݽӿ�
    LCD_w_com(0x0c);  //0x0c����ʾ�������أ�0x0c=00001100��D=1��ʾ����C=0���أ�
    LCD_w_com(0x06);  //0x06������Զ����ƣ���Ļ�����������뷽ʽ���ã�
    gotoxy(1,0);      //��λ����һ�е�1��
    //��ʾ��ʼ���ַ�"Hello!"
    for(i=0;i<=5;i++)  
    {  
        LCD_w_dat(test[i]); 
    } 
} 


/*********�кź����߼�����*********/

/**
 * �кŴ������������ڰ�������ʱ��ִ�нк��߼���
 * ���ܣ����½к���Ϣ��LCD��ʾ��������ʾ
 */
void key()
{         
    //ֻ�е�ȡ�������ڵ�ǰ�к���ʱ�����ܽкţ�����кų���ȡ�ţ�
    if(qhao > jhao)     
    {       
        SPEAKER(11);    //��������������ʾ�кţ�
        delay(200);     
        SPEAKER(15);    //����������һ�Σ���ͬƵ�ʣ�
        clear_LCD();    //����׼����ʾ�½к���Ϣ

        sev++;          //�ܽкż���+1
        //���½к���ʾ��һ�еĺ��루"No.XX..."��
        prompt2[3] = (jhao + 1)/10 + '0';  //ʮλ���֣���12��ʮλ��1��
        prompt2[4] = (jhao + 1)%10 + '0';  //��λ���֣���12�ĸ�λ��2��
        //���½к���ʾ�ڶ��еĴ��ںţ�"X.window..."��
        prompt3[0] = jz + '0';             //���ںţ�1-4��
        //��ʾ��һ����ʾ
        gotoxy(1,0);      
        for(m=0;m<=15;m++)      //prompt2��16���ַ���������һ�У�
        {      
            LCD_w_dat(prompt2[m]);      
        }      
        //��ʾ�ڶ�����ʾ
        gotoxy(2,0);      
        for(m=0;m<=15;m++)     //prompt3��16���ַ��������ڶ��У�
        {
            LCD_w_dat(prompt3[m]);     
        }         
        jhao++;  //��ǰ�к���+1�����һ�νкţ�
    }
}

/**
 * ��ʾ�ȴ�������������Ӧĳ������������ʱ��ʾ��ǰ�ȴ�������
 */
void k5()
{
    clear_LCD();                  //����
    dhao = qhao - jhao;           //�ȴ�����=��ȡ����-�ѽк���
    prompt8[0] = dhao / 10 + '0'; //�ȴ�����ʮλ
    prompt8[1] = dhao % 10 + '0'; //�ȴ�������λ
    gotoxy(1,0);                  //��λ��һ��
    for(m=0;m<=9;m++)             //��ʾ"XX is left"����10���ַ���
    {  
        LCD_w_dat(prompt8[m]);     
    }       
}

/**
 * ��ʾ��ȡ������������Ӧĳ������������ʱ��ʾ��ȡ������
 */
void k6()
{     
    clear_LCD();                  //����
    prompt9[7] = sev / 10 + '0';  //�ܽк���ʮλ
    prompt9[8] = sev % 10 + '0';  //�ܽк�����λ
    gotoxy(1,0);                  //��λ��һ��
    for(m=0;m<=8;m++)             //��ʾ"All is XX"����9���ַ���
    {   
        LCD_w_dat(prompt9[m]); 
    }  
}

/**
 * ��ʾ�����ڽкż�¼��������Ӧĳ������������ʱ��ʾ4�����ڵĽк�����
 */
void k7()
{
    clear_LCD();                  //����
    //���¸����ڽкż�¼����ʾ��"W1 - XX"�ȣ�
    prompt10[5] = sev1 / 10 + '0';    
    prompt10[6] = sev1 % 10 + '0';    
    prompt11[5] = sev2 / 10 + '0';    
    prompt11[6] = sev2 % 10 + '0';    
    prompt12[5] = sev3 / 10 + '0';    
    prompt12[6] = sev3 % 10 + '0';    
    prompt13[5] = sev4 / 10 + '0';    
    prompt13[6] = sev4 % 10 + '0';     
    //��һ����ʾW1��W2��������ʾ�������ص���
    gotoxy(1,0);                     //��һ�е�1��
    for(m=0;m<=6;m++)                //��ʾ"W1 - XX"��7���ַ���
    {    
        LCD_w_dat(prompt10[m]);    
    }        
    gotoxy(1,8);                     //��һ�е�9�У����������
    for(m=0;m<=6;m++)                //��ʾ"W2 - XX"
    {    
        LCD_w_dat(prompt11[m]);    
    }    
    //�ڶ�����ʾW3��W4
    gotoxy(2,0);                     //�ڶ��е�1��
    for(m=0;m<=6;m++)                //��ʾ"W3 - XX"
    {    
        LCD_w_dat(prompt12[m]);    
    } 
    gotoxy(2,8);                     //�ڶ��е�9��
    for(m=0;m<=6;m++)                //��ʾ"W4 - XX"
    {    
        LCD_w_dat(prompt13[m]);    
    }    
} 


void matrixkeyscan()
{
    unsigned char KeyNumber = 0;
    
    // ɨ���һ�У�P1.7�õͣ�
    P1 = 0xFF;
    P1_7 = 0;
    
    if(P1_3 == 0)  {delay(20); while(P1_3 == 0); delay(20); KeyNumber = 1;}  // 1�Ŵ���
    if(P1_2 == 0)  {delay(20); while(P1_2 == 0); delay(20); KeyNumber = 2;}  // 2�Ŵ���
    if(P1_1 == 0)  {delay(20); while(P1_1 == 0); delay(20); KeyNumber = 3;}  // 3�Ŵ���
    if(P1_0 == 0)  {delay(20); while(P1_0 == 0); delay(20); KeyNumber = 4;}  // 4�Ŵ���
    
    // ɨ��ڶ��У�P1.6�õͣ�
    P1 = 0xFF;
    P1_6 = 0;
    
    if(P1_3 == 0)  {delay(20); while(P1_3 == 0); delay(20); KeyNumber = 5;}  
    if(P1_2 == 0)  {delay(20); while(P1_2 == 0); delay(20); KeyNumber = 6;}  
    if(P1_1 == 0)  {delay(20); while(P1_1 == 0); delay(20); KeyNumber = 7;}  
    if(P1_0 == 0)  {delay(20); while(P1_0 == 0); delay(20); KeyNumber = 8;}  
    
    // ɨ������У�P1.5�õͣ�
    P1 = 0xFF;
    P1_5 = 0;
    
    if(P1_3 == 0)  {delay(20); while(P1_3 == 0); delay(20); KeyNumber = 9;}  
    if(P1_2 == 0)  {delay(20); while(P1_2 == 0); delay(20); KeyNumber = 10;} 
    if(P1_1 == 0)  {delay(20); while(P1_1 == 0); delay(20); KeyNumber = 11;} 
    if(P1_0 == 0)  {delay(20); while(P1_0 == 0); delay(20); KeyNumber = 12;} 
    
    // ɨ������У�P1.4�õͣ�
    P1 = 0xFF;
    P1_4 = 0;
    
    if(P1_3 == 0)  {delay(20); while(P1_3 == 0); delay(20); KeyNumber = 13;} 
    if(P1_2 == 0)  {delay(20); while(P1_2 == 0); delay(20); KeyNumber = 14;} 
    if(P1_1 == 0)  {delay(20); while(P1_1 == 0); delay(20); KeyNumber = 15;}  
    if(P1_0 == 0)  {delay(20); while(P1_0 == 0); delay(20); KeyNumber = 16;} 
    
    // ���ݰ���ִֵ�ж�Ӧ����
    switch(KeyNumber)
    {
        case 1: jz = 1; sev1++; key(); break;  // 1�Ŵ��ڽк�
        case 2: jz = 2; sev2++; key(); break;  // 2�Ŵ��ڽк�
        case 3: jz = 3; sev3++; key(); break;  // 3�Ŵ��ڽк�
        case 4: jz = 4; sev4++; key(); break;  // 4�Ŵ��ڽк�
        case 5: k7(); break;                  // ��ʾ���ڼ�¼
        case 6: k6(); break;                  // ��ʾ�ܽк���
        case 7: k5(); break;                  // ��ʾ�ȴ�����
        default: break;
    }
}

 

///**
// * �������������ã�
// * ���ܣ���ʼ��Ӳ����ѭ����ⰴ���ʹ�������
// **/
//void main(void) {
//	  
//    intCon();       // ��ʼ������
//	  clear_LCD();      //��ʼ��ǰ����
//    init_LCD();     // ��ʼ��LCD
//    jhao = 0;       // ��ǰ�к�����ʼ��Ϊ0
//    qhao = 0;       // ȡ��������ʼ��Ϊ0
//    
//    // ��ʼ��ʾ
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
//        matrixkeyscan();    // ���кŰ���

//        // �����к����仯ʱ�ŷ�������
//        
//        if (jhao != last_jhao) {
//            last_jhao = jhao;
//            
//            // ������ʾ
//            gotoxy(1, 2);
//            LCD_w_dat(jhao / 10 + '0');
//            LCD_w_dat(jhao % 10 + '0');
//            
//            // ��������
//            SBUF = jhao;     // ���ͺ���
//            while (!TI);     // �ȴ��������
//            TI = 0;          // ������ͱ�־
//        }

//        // �������ݣ�����RI=1ʱ����
//        if (RI) {
//            RI = 0;          // ������ձ�־
//            qhao = SBUF;     // ��ȡ��������
//            
//            // ������ʾ
//            gotoxy(2, 2);
//            LCD_w_dat(qhao / 10 + '0');
//            LCD_w_dat(qhao % 10 + '0');
//        }

//        delay1ms(200); // �ʵ���ʱ��
//    }
//}

/**
 * ��������ϵͳ��ڣ�
 * ���ܣ���ʼ��Ӳ����ѭ����ⰴ���ʹ�������
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