#ifndef __SCAN_H
#define __SCAN_H
#include "sys.h"

//***********************************量革机的型号************************************************
// 	#define	LGJType 140		//量革机的型号		//特别说明：140型的图片借用150
// 	#define	LGJType 150		//量革机的型号
	#define	LGJType 180		//量革机的型号		//默认硬件版本
//	#define	LGJType 200		//量革机的型号
//	#define	LGJType 220		//量革机的型号
//	#define	LGJType 240		//量革机的型号
//	#define	LGJType 256		//量革机的型号
//	#define	LGJType 280		//量革机的型号
//	#define	LGJType 300		//量革机的型号
//	#define	LGJType 320		//量革机的型号

#define	LGJLight (LGJType/2)		//量革机的光电的灯的数目
#define	LGJUpBound ((LGJLight+7)/8)		//量革机的光电的灯的数目对应的字节数组大小
#define	LGJLightNum  ((LGJType/2)%8)		//与掉时要保留的光电的灯的数目

#define OUT_A8		PAout(8)
#define OUT_A9		PCout(9)
#define OUT_A10		PCout(8)
#define OUT_A11		PCout(7)
#define OUT_A12		PCout(6)

#define IN_D0		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)
#define IN_D1		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)
#define IN_D2		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_11)
#define IN_D3		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)
#define IN_D4		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)
#define IN_D5		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define IN_D6		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)
#define IN_D7		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)

#define H_CrimpySelect 0
#define H_SynchronalSelect 19700

u8 SCAN_Read(u8 *point, u8 num , u8 last_byte);
void SCAN_Init(void);
u8 MeasTestZero(void);
u16 CarryUint(float oldFloat,u8 carrytype);
void AreaCoefficient(void);
u8 GetCullinearea(u8 *arry);
//extern u8 ScanPoint[5][30];
void printf_arry(void);
void printf_Scan_test(void);
void printf_arr(void);


#endif
