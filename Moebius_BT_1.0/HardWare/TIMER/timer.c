#include "timer.h"

u16 TIM3CH3_CAPTURE_STA, TIM3CH3_CAPTURE_VAL;

void TIM3_Cap_Init(u16 arr, u16 psc)
{
	RCC->APB1ENR |= 1 << 1;
	RCC->APB2ENR |= 1 << 3;
	GPIOB->CRL &= 0XFFFFFF00;
	GPIOB->CRL |= 0X00000028;

	TIM3->ARR = arr;
	TIM3->PSC = psc;
	TIM3->CCMR2 |= 1 << 0;
	TIM3->CCMR2 |= 0 << 4;
	TIM3->CCMR2 |= 0 << 2;

	TIM3->CCER |= 0 << 9;
	TIM3->CCER |= 1 << 8;

	TIM3->DIER |= 1 << 3;
	TIM3->DIER |= 1 << 0;
	TIM3->CR1 |= 0x01;
	MY_NVIC_Init(1, 3, TIM3_IRQn, 1);
}

void Read_Distane(void)
{
	PBout(1) = 1;
	delay_us(15);
	PBout(1) = 0;
	if (TIM3CH3_CAPTURE_STA & 0X80) {
		Distance = TIM3CH3_CAPTURE_STA & 0X3F;
		Distance *= 65536;
		Distance += TIM3CH3_CAPTURE_VAL;
		Distance = Distance * 170 / 1000;
		printf("%d \r\n", Distance);
		TIM3CH3_CAPTURE_STA = 0;
	}
}

void TIM3_IRQHandler(void)
{
	u16 tsr;

	tsr = TIM3->SR;
	if ((TIM3CH3_CAPTURE_STA & 0X80) == 0) {
		if (tsr & 0X01) {
			if (TIM3CH3_CAPTURE_STA & 0X40) {
				if ((TIM3CH3_CAPTURE_STA & 0X3F) == 0X3F) {
					TIM3CH3_CAPTURE_STA |= 0X80;
					TIM3CH3_CAPTURE_VAL = 0XFFFF;
				} else {
					TIM3CH3_CAPTURE_STA++;
				}
			}
		}
		if (tsr & 0x08) {
			if (TIM3CH3_CAPTURE_STA & 0X40) {
				TIM3CH3_CAPTURE_STA |= 0X80;
				TIM3CH3_CAPTURE_VAL = TIM3->CCR3;
				TIM3->CCER &= ~(1 << 9);
			} else {
				TIM3CH3_CAPTURE_STA = 0;
				TIM3CH3_CAPTURE_VAL = 0;
				TIM3CH3_CAPTURE_STA |= 0X40;
				TIM3->CNT = 0;
				TIM3->CCER |= 1 << 9;
			}
		}
	}
	TIM3->SR = 0;
}
