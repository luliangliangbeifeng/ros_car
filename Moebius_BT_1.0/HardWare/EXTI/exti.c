#include "exti.h"

void EXTI15_Init(void)
{
  RCC->APB2ENR |= 1 << 3;
  GPIOB->CRH &= 0X0FFFFFFF;
  GPIOB->CRH |= 0X80000000;
  GPIOB->ODR |= 1 << 15;
  Ex_NVIC_Config(GPIO_B, 15, FTIR);
  MY_NVIC_Init(2, 1, EXTI15_10_IRQn, 2);
}
