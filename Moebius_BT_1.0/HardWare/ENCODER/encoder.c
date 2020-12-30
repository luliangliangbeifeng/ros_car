#include "encoder.h"

void Encoder_Init_TIM2(void)
{
	AFIO->MAPR |= 1 << 8;
	RCC->APB1ENR |= 1 << 0;
	RCC->APB2ENR |= 1 << 2;
	RCC->APB2ENR |= 1 << 3;
	GPIOA->CRH &= 0X0FFFFFFF;
	GPIOA->CRH |= 0X40000000;

	GPIOB->CRL &= 0XFFFF0FFF;
	GPIOB->CRL |= 0X00004000;
	TIM2->DIER |= 1 << 0;
	TIM2->DIER |= 1 << 6;
	MY_NVIC_Init(1, 3, TIM2_IRQn, 1);
	TIM2->PSC = 0x0;
	TIM2->ARR = ENCODER_TIM_PERIOD;
	TIM2->CR1 &= ~(3 << 8);
	TIM2->CR1 &= ~(3 << 5);

	TIM2->CCMR1 |= 1 << 0;
	TIM2->CCMR1 |= 1 << 8;
	TIM2->CCER &= ~(1 << 1);
	TIM2->CCER &= ~(1 << 5);
	TIM2->SMCR |= 3 << 0;
	TIM2->CR1 |= 0x01;
}

void Encoder_Init_TIM3(void)
{
	RCC->APB1ENR |= 1 << 1;
	RCC->APB2ENR |= 1 << 3;
	GPIOA->CRL &= 0X00FFFFFF;
	GPIOA->CRL |= 0X44000000;
	TIM3->DIER |= 1 << 0;
	TIM3->DIER |= 1 << 6;
	MY_NVIC_Init(1, 3, TIM3_IRQn, 1);
	TIM3->PSC = 0x0;
	TIM3->ARR = ENCODER_TIM_PERIOD;
	TIM3->CR1 &= ~(3 << 8);
	TIM3->CR1 &= ~(3 << 5);

	TIM3->CCMR1 |= 1 << 0;
	TIM3->CCMR1 |= 1 << 8;
	TIM3->CCER &= ~(1 << 1);
	TIM3->CCER &= ~(1 << 5);
	TIM3->SMCR |= 3 << 0;
	TIM3->CR1 |= 0x01;
}

void Encoder_Init_TIM4(void)
{
	RCC->APB1ENR |= 1 << 2;
	RCC->APB2ENR |= 1 << 3;
	GPIOB->CRL &= 0X00FFFFFF;
	GPIOB->CRL |= 0X44000000;
	TIM4->DIER |= 1 << 0;
	TIM4->DIER |= 1 << 6;
	MY_NVIC_Init(1, 3, TIM4_IRQn, 1);
	TIM4->PSC = 0x0;
	TIM4->ARR = ENCODER_TIM_PERIOD;
	TIM4->CR1 &= ~(3 << 8);
	TIM4->CR1 &= ~(3 << 5);

	TIM4->CCMR1 |= 1 << 0;
	TIM4->CCMR1 |= 1 << 8;
	TIM4->CCER &= ~(1 << 1);
	TIM4->CCER &= ~(1 << 5);
	TIM4->SMCR |= 3 << 0;
	TIM4->CR1 |= 0x01;
}

void Encoder_Init_TIM5(void)
{
	RCC->APB1ENR |= 1 << 3;
	RCC->APB2ENR |= 1 << 2;
	GPIOA->CRL &= 0XFFFFFF00;
	GPIOA->CRL |= 0X00000044;
	TIM2->DIER |= 1 << 0;
	TIM2->DIER |= 1 << 6;
	MY_NVIC_Init(1, 3, TIM5_IRQn, 1);
	TIM5->PSC = 0x0;
	TIM5->ARR = ENCODER_TIM_PERIOD;
	TIM5->CR1 &= ~(3 << 8);
	TIM5->CR1 &= ~(3 << 5);

	TIM5->CCMR1 |= 1 << 0;
	TIM5->CCMR1 |= 1 << 8;
	TIM5->CCER &= ~(1 << 1);
	TIM5->CCER &= ~(1 << 5);
	TIM5->SMCR |= 3 << 0;
	TIM5->CR1 |= 0x01;
}

int Read_Encoder(u8 TIMX)
{
	int Encoder_TIM;

	switch (TIMX) {
	case 2:  Encoder_TIM = (short)TIM2->CNT;   TIM2->CNT = 0; break;
	case 3:  Encoder_TIM = (short)TIM3->CNT;   TIM3->CNT = 0; break;
	case 4:  Encoder_TIM = (short)TIM4->CNT;   TIM4->CNT = 0;  break;
	case 5:  Encoder_TIM = (short)TIM5->CNT;   TIM5->CNT = 0;  break;
	default: Encoder_TIM = 0;
	}
	return Encoder_TIM;
}

void TIM2_IRQHandler(void)
{
	if (TIM2->SR & 0X0001) {
	}
	TIM2->SR &= ~(1 << 0);
}

void TIM3_IRQHandler(void)
{
	if (TIM3->SR & 0X0001) {
	}
	TIM3->SR &= ~(1 << 0);
}

void TIM4_IRQHandler(void)
{
	if (TIM4->SR & 0X0001) {
	}
	TIM4->SR &= ~(1 << 0);
}

void TIM5_IRQHandler(void)
{
	if (TIM5->SR & 0X0001) {
	}
	TIM5->SR &= ~(1 << 0);
}
