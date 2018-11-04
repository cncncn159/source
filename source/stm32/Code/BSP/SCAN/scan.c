#include "stm32f10x.h"
#include "scan.h"
#include "sys.h"
#include "delay.h"
#include "usart.h" 

                              
const u8 byte_to_1_Num[]={
0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,1,2,2,3,
2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,1,2,2,3,2,3,3,4,
2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,
4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,
4,5,5,6,4,5,5,6,5,6,6,7,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,
4,5,5,6,5,6,6,7,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,
6,7,7,8};


static void Ax_out(u8 data);
static u8 Dx_in(void);
static u8 ScanLightNum = LGJLightNum;
static u8 ScanByteNum = LGJUpBound;
static u8 line_area=0;
static u8 LocalScanPoint[30];
float Coefficient_m2,Coefficient_yc,Coefficient_sc;
static void Hex_to_Bin(u8 hex,u8 *Bin);

void SCAN_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD,ENABLE);
	//A8 A9 A10 A11 A12	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;//A8		PA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//A9 A10 A11 A12		PC9 PC8 PC7 PC6
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//D0-7
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;//D0		PA15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//?
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;//D1 D2 D3		PC10 PC11 PC12
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;//D4		PD2
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);		//pb3 复用
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6;//D5 D6 D7		PB3 PB4 PB6
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


u8 SCAN_Read(u8 *point, u8 num , u8 last_byte)//point为读出的数据地址	num = LGJUpBound	返回0为正常读数 ff为输入异常
{
	u8 i,temp;
	//准备读入 
	//74ls541(读入8bit) 一直为使能
	//A12 拉低 74ls154 SA0-SA15使能 74ls138 SA16-SA22禁能
	OUT_A12 = 0;		//片选
	if(num >= 25)
		return 0xff;
	for(i=0;i<num;i++)
	{
		Ax_out(i);
		//delay_ms(1);
		*(point + i) = Dx_in();
	}
	if(last_byte<=8)
		temp = (u8)(~(0xff>>last_byte));		//temp为掩码 eg:180 =>last_byte=2 temp=1100 0000 
	else
	{
		return 0xff;
	}
	*(point + (num-1)) &= temp;
	return 0;
}

void Ax_out(u8 data)
{
	u8 byte = data;
	
	if(data > 0x1f)
		byte = 0;		//防止输入错误
	if((byte & 0x01) != 0) OUT_A8 = 1;
		else OUT_A8 = 0;
	if((byte & 0x02) != 0) OUT_A9 = 1;
		else OUT_A9 = 0;
	if((byte & 0x04) != 0) OUT_A10 = 1;
		else OUT_A10 = 0;
	if((byte & 0x08) != 0) OUT_A11 = 1;
		else OUT_A11 = 0;
	if((byte & 0x10) != 0) OUT_A12 = 1;
		else OUT_A12 = 0;
}

u8 Dx_in(void)
{
	u8 byte = 0;
	
	if(IN_D0) byte |= 0x01;
	if(IN_D1) byte |= 0x02;
	if(IN_D2) byte |= 0x04;
	if(IN_D3) byte |= 0x08;
	if(IN_D4) byte |= 0x10;
	if(IN_D5) byte |= 0x20;
	if(IN_D6) byte |= 0x40;
	if(IN_D7) byte |= 0x80;
	
	return byte;
}

u8 MeasTestZero(void)				//1 为正常 0 为错误
{
	u8 i,index;
	
	SCAN_Read(LocalScanPoint,ScanByteNum,ScanLightNum);
	line_area = 0;
	for (i=0;i<LGJUpBound;i++)
	{
		index = LocalScanPoint[i];
		line_area = line_area + byte_to_1_Num[index];
	}
	if (line_area==0) 
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

u8 GetCullinearea(u8 *arry)
{
	u8 line=0;
	u8 index;
	u8 i;
	
	for (i=0;i<LGJUpBound;i++)
	{
		index = arry[i];
		line= line + byte_to_1_Num[index];
	}
	return line;
}

u16 CarryUint(float oldFloat,u8 carrytype)	//carrytype=1为0.25进，=2为0.1，=3为0.01，=4为0.001 返回×1000
{
	u32 longtemp;
	u32 val;
	switch (carrytype)
	{
		case	1:			//carrytype=1为0.25进
			oldFloat=(oldFloat*4)+0.5; 
			longtemp=(unsigned long )oldFloat; 
	 		val = (250*longtemp);
		 	break;					
		case	2:			//carrytype=2为0.1
			oldFloat=(oldFloat*10)+0.5; 
			longtemp=(unsigned long )oldFloat;
	 		val = ( 100*longtemp);
			break;
		case	3:			//carrytype=3为0.01
			oldFloat=(oldFloat*100)+0.5; 
			longtemp=(unsigned long )oldFloat;
	 		val = (10*longtemp);
			break;
		case	4:			//carrytype=4为0.001
			oldFloat=(oldFloat*1000)+0.5; 
			longtemp=(unsigned long )oldFloat;
	 		val = ( longtemp);
			break;
		default:
			break;
	}
	return val;
}

void AreaCoefficient(void)
{
	u16 CrimpySelect = H_CrimpySelect;
	u16 SynchronalSelect = H_SynchronalSelect;
	
	Coefficient_m2 =0.00001* (CrimpySelect+1000) * (100 +SynchronalSelect*0.01)*0.0001 ;//平方米
	Coefficient_yc =0.00001* (CrimpySelect+1000) * (100 +SynchronalSelect*0.01)*0.001076;//平方英尺
	Coefficient_sc =0.00001* (CrimpySelect+1000) * (100 +SynchronalSelect*0.01)*0.0009 ;//平方市尺 
}

void printf_arry(void)
{
	u8 i;
	
	printf("LocalScanPoint: ");
	for(i=0;i<LGJUpBound;i++)
	{
		printf(" [%X]",LocalScanPoint[i]);
	}
	printf("\r\n");
}

void printf_Scan_test(void)	//光电板测试 输出二进制光电板测量数据
{
	u8 temp[8];
	u8 i;
	
	SCAN_Read(LocalScanPoint,LGJUpBound,ScanLightNum);
	printf(" \r\n***************SCAN TEST*****************\r\n");
	for(i=0;i<(LGJUpBound+1)/2;i++)
	{
		Hex_to_Bin(LocalScanPoint[i*2],temp);
		printf("L%d:",i);
		printf(" %d %d %d %d %d %d %d %d	",temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6],temp[7]);
		Hex_to_Bin(LocalScanPoint[1*2+1],temp);
		printf(" %d %d %d %d %d %d %d %d\r\n",temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6],temp[7]);
	}
	printf(" ******************END********************\r\n");
}

static void Hex_to_Bin(u8 hex,u8 *Bin)		//将 16进制输入 2进制输入到8组的数组中 方便打印显示
{
	Bin[0] = hex&0x01;
	Bin[1] = (hex>>1)&0x01;
	Bin[2] = (hex>>2)&0x01;
	Bin[3] = (hex>>3)&0x01;
	Bin[4] = (hex>>4)&0x01;
	Bin[5] = (hex>>5)&0x01;
	Bin[6] = (hex>>6)&0x01;
	Bin[7] = (hex>>7)&0x01;
}

void printf_arr(void)
{
	u8 temp[8],i;
	
	SCAN_Read(LocalScanPoint,LGJUpBound,ScanLightNum);
	printf("\r\n");
	for(i=0;i<LGJUpBound;i++)
	{

		Hex_to_Bin(LocalScanPoint[i],temp);
		printf("%d%d%d%d %d%d%d%d ",temp[7],temp[6],temp[5],temp[4],temp[3],temp[2],temp[1],temp[0]);
	}
	printf("\r\n");
}
