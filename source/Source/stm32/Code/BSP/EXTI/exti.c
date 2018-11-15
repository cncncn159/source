#include "exti.h"
#include "delay.h"
#include "scan.h"
#include "usart.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//�ⲿ�ж�0�������
static u16 Int_time;
//extern u8 ScanPoint[Arry_N][30];

void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;//PB13
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

	//GPIOB.13 �ж����Լ��жϳ�ʼ������   �½��ش���
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);

	EXTI_InitStructure.EXTI_Line=EXTI_Line13;	//INT0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ���ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	//��ռ���ȼ�3�� 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 
 
}

//�ⲿ�ж�0������� 
void EXTI15_10_IRQHandler(void)
{
	u8 i;
	u8 io_now;					//��ǰӦ�ô��ڵ�״̬
	static u8 last_io = 0xff;	//��һ�ε�״̬
	
	if(last_io != 0xff)		
	{		//��Ϊ��һ�ν���
		if(last_io == IO_READ)//���ϴ�һ����û�б仯 ������
			return;
		else				//�б仯
		{
			if(last_io == 0)
				io_now = 1;
			else
				io_now = 0;
		}
	}
	else	
	{		//��һ�ν���
		io_now = IO_READ;//��һ�ν��� ��ȡ������״̬
	}
	for(i=0;i<5;i++)
	{
		delay_us(10);//����
		if(io_now!=IO_READ)
			return;
	}
	if(Int_time>=Arry_N)
	{
		printf("err overflow\r\n");
	}
	(void)SCAN_Read(ScanPoint[Int_time],LGJUpBound, LGJLightNum);
	
	Int_time++;
	EXTI_ClearITPendingBit(EXTI_Line13); //���LINE0�ϵ��жϱ�־λ 
}


u8 Scan_Int(void)
{
	return Int_time;
}

u8 Int_Cnt(void)		//����0���� ff �������µ����
{
	u8 i,j;
	u8 err = 0;
	
	__disable_irq();
	Int_time --;
	
	if(Int_time>=Arry_N)			//�������� �����
	{
		return 0xff;
	}
	if(Int_time!=0)//�л���
	{
		for(i=0;i<Int_time;i++)
		{
			for(j=0;j<30;j++)
			ScanPoint[i][j]=ScanPoint[i+1][j];
		}
	}
	__enable_irq();
	return err;
}
