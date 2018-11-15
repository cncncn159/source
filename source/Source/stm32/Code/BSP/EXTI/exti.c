#include "exti.h"
#include "delay.h"
#include "scan.h"
#include "usart.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//外部中断0服务程序
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

	//GPIOB.13 中断线以及中断初始化配置   下降沿触发
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);

	EXTI_InitStructure.EXTI_Line=EXTI_Line13;	//INT0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	//抢占优先级3， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 
 
}

//外部中断0服务程序 
void EXTI15_10_IRQHandler(void)
{
	u8 i;
	u8 io_now;					//当前应该处于的状态
	static u8 last_io = 0xff;	//上一次的状态
	
	if(last_io != 0xff)		
	{		//不为第一次进入
		if(last_io == IO_READ)//和上次一样，没有变化 （消抖
			return;
		else				//有变化
		{
			if(last_io == 0)
				io_now = 1;
			else
				io_now = 0;
		}
	}
	else	
	{		//第一次进入
		io_now = IO_READ;//第一次进如 则取读到的状态
	}
	for(i=0;i<5;i++)
	{
		delay_us(10);//消抖
		if(io_now!=IO_READ)
			return;
	}
	if(Int_time>=Arry_N)
	{
		printf("err overflow\r\n");
	}
	(void)SCAN_Read(ScanPoint[Int_time],LGJUpBound, LGJLightNum);
	
	Int_time++;
	EXTI_ClearITPendingBit(EXTI_Line13); //清除LINE0上的中断标志位 
}


u8 Scan_Int(void)
{
	return Int_time;
}

u8 Int_Cnt(void)		//返回0正常 ff 阻塞导致的溢出
{
	u8 i,j;
	u8 err = 0;
	
	__disable_irq();
	Int_time --;
	
	if(Int_time>=Arry_N)			//大于数组 溢出！
	{
		return 0xff;
	}
	if(Int_time!=0)//有缓存
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
