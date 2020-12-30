#include "adc.h"

void  Adc_Init(void)
{
	RCC->APB2ENR |= 1 << 2;
	GPIOA->CRL &= 0XFF0FFFFF;
	RCC->APB2ENR |= 1 << 9;
	RCC->APB2RSTR |= 1 << 9;
	RCC->APB2RSTR &= ~(1 << 9);
	RCC->CFGR &= ~(3 << 14);

	RCC->CFGR |= 2 << 14;
	ADC1->CR1 &= 0XF0FFFF;
	ADC1->CR1 |= 0 << 16;
	ADC1->CR1 &= ~(1 << 8);
	ADC1->CR2 &= ~(1 << 1);
	ADC1->CR2 &= ~(7 << 17);
	ADC1->CR2 |= 7 << 17;
	ADC1->CR2 |= 1 << 20;
	ADC1->CR2 &= ~(1 << 11);
	ADC1->SQR1 &= ~(0XF << 20);
	ADC1->SQR1 &= 0 << 20;

	ADC1->SMPR2 &= 0XFF0FFFFF;
	ADC1->SMPR2 |= 7 << 15;

	ADC1->CR2 |= 1 << 0;
	ADC1->CR2 |= 1 << 3;
	while (ADC1->CR2 & 1 << 3);

	ADC1->CR2 |= 1 << 2;
	while (ADC1->CR2 & 1 << 2);
}

u16 Get_Adc(u8 ch)
{
	ADC1->SQR3 &= 0XFFFFFFE0;
	ADC1->SQR3 |= ch;
	ADC1->CR2 |= 1 << 22;
	while (!(ADC1->SR & 1 << 1));
	return ADC1->DR;
}


int Get_battery_volt(void)
{
	int Volt;

	Volt = Get_Adc(Battery_Ch) * 3.3 * 11.0 * 100 / 1.0 / 4096;
	return Volt;
}
