#ifndef __SCAN_H
#define __SCAN_H
#include "sys.h"

//***********************************��������ͺ�************************************************
// 	#define	LGJType 140		//��������ͺ�		//�ر�˵����140�͵�ͼƬ����150
// 	#define	LGJType 150		//��������ͺ�
	#define	LGJType 180		//��������ͺ�		//Ĭ��Ӳ���汾
//	#define	LGJType 200		//��������ͺ�
//	#define	LGJType 220		//��������ͺ�
//	#define	LGJType 240		//��������ͺ�
//	#define	LGJType 256		//��������ͺ�
//	#define	LGJType 280		//��������ͺ�
//	#define	LGJType 300		//��������ͺ�
//	#define	LGJType 320		//��������ͺ�

#define	LGJLight (LGJType/2)		//������Ĺ��ĵƵ���Ŀ
#define	LGJUpBound ((LGJLight+7)/8)		//������Ĺ��ĵƵ���Ŀ��Ӧ���ֽ������С
#define	LGJLightNum  ((LGJType/2)%8)		//���ʱҪ�����Ĺ��ĵƵ���Ŀ

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
