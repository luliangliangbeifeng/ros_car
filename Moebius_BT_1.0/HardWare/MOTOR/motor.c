#include "motor.h"

void MiniBalance_Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);                                // 关闭JTAG调试接口

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	INA1 = 0; INB1 = 0; INC1 = 0; IND1 = 0;
	INA2 = 0; INB2 = 0; INC2 = 0; IND2 = 0;

	Set_MotorPWMA(0);
	Set_MotorPWMB(0);
	Set_MotorPWMC(0);
	Set_MotorPWMD(0);
}

void MiniBalance_PWM_Init(u16 arr, u16 psc)
{
	MiniBalance_Motor_Init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	RCC->APB2ENR |= 1 << 13;
	RCC->APB2ENR |= 1 << 4;
	GPIOC->CRH &= 0XFFFFFF00;
	GPIOC->CRH |= 0X000000BB;

	GPIOC->CRL &= 0X00FFFFFF;
	GPIOC->CRL |= 0XBB000000;
	TIM8->ARR = arr;
	TIM8->PSC = psc;
	TIM8->CCMR1 |= 6 << 4;
	TIM8->CCMR1 |= 6 << 12;
	TIM8->CCMR2 |= 6 << 4;
	TIM8->CCMR2 |= 6 << 12;

	TIM8->CCMR1 |= 1 << 3;
	TIM8->CCMR1 |= 1 << 11;
	TIM8->CCMR2 |= 1 << 3;
	TIM8->CCMR2 |= 1 << 11;

	TIM8->CCER |= 1 << 0;
	TIM8->CCER |= 1 << 4;
	TIM8->CCER |= 1 << 8;
	TIM8->CCER |= 1 << 12;
	TIM8->CCR1 = 0;
	TIM8->CCR2 = 0;
	TIM8->CCR3 = 0;
	TIM8->CCR4 = 0;
	TIM8->BDTR |= 1 << 15;
	TIM8->CR1 = 0x8000;
	TIM8->CR1 |= 0x01;
}

