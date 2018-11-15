#include "usart.h"
#include "exti.h"
#include "led.h"
#include "scan.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"

#define AREA_MIN 			5  		//初始化 面积下限
#define INIT_SCAN_PRINT 	3		//初始化时光电检测板 检测数据打印	单位秒

u8 ScanPoint[Arry_N][30];
//static void print_array(void);
static u8 current_line=0;
static u8 IsSheetBegin = 0; //张开始标志
static u16 accu_area;		//每张皮的累加点数
static u32 SheetArea_m2=0; //每张皮最终的面积
static u32 SheetArea_yc=0;
static u32 SheetArea_sc=0;
extern float Coefficient_m2,Coefficient_yc,Coefficient_sc;//面积换算系数 = 皱缩率 *外同步率* 单位系数
static u8 NumSelect_Fractional = 0;//小数位数
u32 AreaMin_Cal = AREA_MIN;

u8 print_temp = INIT_SCAN_PRINT;

float dis_m2,dis_sc,dis_yc;

int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	Uatr1_init(9600);
	Uatr2_init(9600);
	TIM3_Int_Init(9,7199);//10Khz的计数频率，计数到10为1ms
	delay_init();
	EXTIX_Init();			//外部中断由跳变触发改为边沿触发 中断去抖部分还要再修改一下 10/11
	LED_Init();
	SCAN_Init();
	CLI();
	AreaCoefficient();		//coefficient 系数计算
	LED1 = 0; //亮
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
	LED1 = 1;//暗
	printf_Scan_test();
	delay_ms(1000);
	SEI();
	while(1)
	{
		if(Scan_Int())
		{
			if(Int_Cnt()==0xff)							//Int_Cnt必须放在计算前
			{
				printf("Congestion overflow!!\r\n");
				delay_ms(500);
			}
			//print_array();
			current_line = 0;
			current_line = GetCullinearea(ScanPoint[0]);		
			//先由数组计算行行数据 ScanPoint[]当前的行都为[0] Int_Cnt中会将缓存移到[0]中
			//printf("current line: %d\r\n",current_line);		
			//***********主测量*************//
			if(current_line == 0)
			{
				LED1 = 0; //亮
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
						NumSelect_Fractional = 2; //平方米
						SheetArea_m2=CarryUint((Coefficient_m2 * accu_area),NumSelect_Fractional );//NumSelect_Fractional小数位数
						NumSelect_Fractional = 1;//英尺
						SheetArea_yc=CarryUint((Coefficient_yc * accu_area),NumSelect_Fractional );//NumSelect_Fractional小数位数
						NumSelect_Fractional = 2; //市尺
						SheetArea_sc=CarryUint((Coefficient_sc * accu_area),NumSelect_Fractional );//NumSelect_Fractional小数位数
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
						printf(" 平方米 : %f \r\n",dis_m2);
						printf("SheetArea_yc: %d\r\n",SheetArea_yc);
						dis_yc = ((float)SheetArea_yc)*0.001;
						printf(" 英尺   : %f \r\n",dis_yc);
						printf("SheetArea_sc: %d\r\n",SheetArea_sc);
						dis_sc = ((float)SheetArea_sc)*0.001;
						printf(" 市尺   : %f \r\n",dis_sc);
					}
				}
			}
			else
			{
				LED1 = 1;//暗
				IsSheetBegin=1;		//==1：张开始标志;
				accu_area=accu_area+current_line;	
			}
		}
		Uart2_handle();	//串口数据处理函数
	}
	
}

