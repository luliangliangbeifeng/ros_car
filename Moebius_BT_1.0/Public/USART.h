#ifndef _USART_H_
#define _USART_H_

#include "Sys.h"
#include "stdio.h"

#define UART_TX_DATA_LEN     17
#define UART_RX_LEN          128

void USART2_Init(u32 bound);
void sendDataToScope(void);
void push(u8 chanel, float data);
void usart_dma_init(void);
void uart_dma_send_enable(uint16_t size);

#endif


