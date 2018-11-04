#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"

#define CLI() 	__set_PRIMASK(1) 
#define SEI() 	__set_PRIMASK(0) 
#define Arry_N	20 //检测的缓存长度

#define IO_READ GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)

extern u8 ScanPoint[Arry_N][30];

void EXTIX_Init(void);//外部中断初始化		 	

u8 Scan_Int(void);
u8 Int_Cnt(void);

#endif

