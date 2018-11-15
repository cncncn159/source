#include "usart.h"
#include "exti.h"
#include "led.h"
#include "scan.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"

#define AREA_MIN 			5  		//��ʼ�� �������
#define INIT_SCAN_PRINT 	3		//��ʼ��ʱ������ ������ݴ�ӡ	��λ��

u8 ScanPoint[Arry_N][30];
//static void print_array(void);
static u8 current_line=0;
static u8 IsSheetBegin = 0; //�ſ�ʼ��־
static u16 accu_area;		//ÿ��Ƥ���ۼӵ���
static u32 SheetArea_m2=0; //ÿ��Ƥ���յ����
static u32 SheetArea_yc=0;
static u32 SheetArea_sc=0;
extern float Coefficient_m2,Coefficient_yc,Coefficient_sc;//�������ϵ�� = ������ *��ͬ����* ��λϵ��
static u8 NumSelect_Fractional = 0;//С��λ��
u32 AreaMin_Cal = AREA_MIN;

u8 print_temp = INIT_SCAN_PRINT;

float dis_m2,dis_sc,dis_yc;

int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	Uatr1_init(9600);
	Uatr2_init(9600);
	TIM3_Int_Init(9,7199);//10Khz�ļ���Ƶ�ʣ�������10Ϊ1ms
	delay_init();
	EXTIX_Init();			//�ⲿ�ж������䴥����Ϊ���ش��� �ж�ȥ�����ֻ�Ҫ���޸�һ�� 10/11
	LED_Init();
	SCAN_Init();
	CLI();
	AreaCoefficient();		//coefficient ϵ������
	LED1 = 0; //��
	printf("init success...\r\n");
	
	delay_ms(500);
	printf("Scan Init...\r\n");
	delay_ms(1000);
	printf_Scan_test();
	delay_ms(1000);
	while(MeasTestZero() == 0)
	{
		printf("Measure No Zero\r\n");
		printf_arry();
		delay_ms(1000);
		if(print_temp != 0)
			print_temp--;
		if(print_temp == 0)
		{
			print_temp = INIT_SCAN_PRINT;
			printf_arr();
		}
	}
	printf("Scan success\r\n");
	LED1 = 1;//��
	printf_Scan_test();
	delay_ms(1000);
	SEI();
	while(1)
	{
		if(Scan_Int())
		{
			if(Int_Cnt()==0xff)							//Int_Cnt������ڼ���ǰ
			{
				printf("Congestion overflow!!\r\n");
				delay_ms(500);
			}
			//print_array();
			current_line = 0;
			current_line = GetCullinearea(ScanPoint[0]);		
			//������������������� ScanPoint[]��ǰ���ж�Ϊ[0] Int_Cnt�лὫ�����Ƶ�[0]��
			//printf("current line: %d\r\n",current_line);		
			//***********������*************//
			if(current_line == 0)
			{
				LED1 = 0; //��
				if(IsSheetBegin==1)
				{
					IsSheetBegin = 0;
					printf("\r\n ***Sheet over***\r\n");
					printf("accu_area : %d\r\n",accu_area);
					if(accu_area < 10)
					{
						//break;
					}
					else
					{
						NumSelect_Fractional = 2; //ƽ����
						SheetArea_m2=CarryUint((Coefficient_m2 * accu_area),NumSelect_Fractional );//NumSelect_FractionalС��λ��
						NumSelect_Fractional = 1;//Ӣ��
						SheetArea_yc=CarryUint((Coefficient_yc * accu_area),NumSelect_Fractional );//NumSelect_FractionalС��λ��
						NumSelect_Fractional = 2; //�г�
						SheetArea_sc=CarryUint((Coefficient_sc * accu_area),NumSelect_Fractional );//NumSelect_FractionalС��λ��
						accu_area = 0;
						if(( SheetArea_m2 == 0 )&&( SheetArea_yc == 0 )&&( SheetArea_sc == 0 ))
						{
							printf("culture area is 0!!\r\n");
							delay_ms(100);
						//	return; 
						}
						if ((SheetArea_m2 < AreaMin_Cal)||(SheetArea_yc < AreaMin_Cal)||(SheetArea_sc < AreaMin_Cal))
						{
							printf("culture area is min!!\r\n");
							delay_ms(100);
						//	return;
						}
//						f_frist = SheetArea_m2/1000;
//						f_last = SheetArea_m2;
						printf("SheetArea_m2: %d\r\n",SheetArea_m2);
						dis_m2 = ((float)SheetArea_m2)*0.001;
						printf(" ƽ���� : %f \r\n",dis_m2);
						printf("SheetArea_yc: %d\r\n",SheetArea_yc);
						dis_yc = ((float)SheetArea_yc)*0.001;
						printf(" Ӣ��   : %f \r\n",dis_yc);
						printf("SheetArea_sc: %d\r\n",SheetArea_sc);
						dis_sc = ((float)SheetArea_sc)*0.001;
						printf(" �г�   : %f \r\n",dis_sc);
					}
				}
			}
			else
			{
				LED1 = 1;//��
				IsSheetBegin=1;		//==1���ſ�ʼ��־;
				accu_area=accu_area+current_line;	
			}
		}
		Uart2_handle();	//�������ݴ�����
	}
	
}

