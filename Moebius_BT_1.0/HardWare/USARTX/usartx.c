
#include "usartx.h"

void usart3_send(u8 data)
{
	USART3->DR = data;
	while ((USART3->SR & 0x40) == 0);
}

void uart3_init(u32 pclk2, u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;

	temp = (float)(pclk2 * 1000000) / (bound * 16);
	mantissa = temp;
	fraction = (temp - mantissa) * 16;
	mantissa <<= 4;
	mantissa += fraction;

	RCC->APB2ENR |= 1 << 0;
	RCC->APB2ENR |= 1 << 4;
	RCC->APB1ENR |= 1 << 18;
	GPIOC->CRH &= 0XFFFF00FF;
	GPIOC->CRH |= 0X00008B00;
	GPIOC->ODR |= 1 << 10;
	AFIO->MAPR |= 1 << 4;

	RCC->APB1RSTR |= 1 << 18;
	RCC->APB1RSTR &= ~(1 << 18);

	USART3->BRR = mantissa;
	USART3->CR1 |= 0X200C;

	USART3->CR1 |= 1 << 8;
	USART3->CR1 |= 1 << 5;
	MY_NVIC_Init(0, 1, USART3_IRQn, 2);
}


int USART3_IRQHandler(void)
{
	if (USART3->SR & (1 << 5)) {
		u8 temp;
		static u8 count, last_data, last_last_data, Usart_ON_Count;
		if (Usart_ON_Flag == 0) {
			if (++Usart_ON_Count > 10) {
				Usart_ON_Flag = 1;
			}
		}
		temp = USART3->DR;
		Show_Data_Mb = temp;
		if (Usart_Flag == 0) {
			if (last_data == 0xfe && last_last_data == 0xff) {
				Usart_Flag = 1, count = 0;
			}
		}
		if (Usart_Flag == 1) {
			Urxbuf[count] = temp;
			count++;
			if (count == 8) {
				Usart_Flag = 0;
			}
		}
		last_last_data = last_data;
		last_data = temp;
	}
	return 0;
}
