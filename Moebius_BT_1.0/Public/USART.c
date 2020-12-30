#include "usart.h"

uint8_t Uart_Rx[21] = { 0 };
uint8_t Uart_Send_Buffer[64] = { 0 };
uint8_t uSendBuf[64] = { 0 };

void USART2_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//  配置GPIO的模式和IO口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                       // TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                 //复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);                          // 初始化串口输入IO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                       // RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;           // 模拟输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 初始化串口接收和发送函数
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_BaudRate = bound;

	// 初始化串口
	USART_Init(USART2, &USART_InitStructure);

	// DMA接收中断设置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// DMA发送中断设置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// DMA1通道7配置发送
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel7);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Send_Buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 64;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);

	// DMA1通道6配置接收
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel6);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Rx;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 21;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel6, ENABLE); // 使能通道6
	// 采用DMA方式发送
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	// 采用DMA方式接收
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	// 启动串口
	USART_Cmd(USART2, ENABLE);
}


void uart_dma_send_enable(uint16_t size)
{
	DMA1_Channel7->CNDTR = (uint16_t)size;
	DMA_Cmd(DMA1_Channel7, ENABLE);
}

void push(u8 chanel, float data)
{
	int i;

	for (i = 0; i < 4; i++) {
		uSendBuf[chanel * 4 + i] = *((uint8_t *)(&data) + i);;
	}
}

void sendDataToScope()
{
	u8 i, sum = 0;

	Uart_Send_Buffer[0] = 251;
	Uart_Send_Buffer[1] = 109;
	Uart_Send_Buffer[2] = 37;
	sum += (251);
	sum += (109);
	sum += (37);
	for (i = 0; i < 48; i++) {
		Uart_Send_Buffer[ i + 3] = uSendBuf[i];
		sum += uSendBuf[i];
	}
	Uart_Send_Buffer[63] = sum;
	uart_dma_send_enable(64);
}

void DMA1_Channel7_IRQHandler(void)
{
	if (DMA_GetFlagStatus(DMA1_FLAG_TC7) == SET) {
		DMA_ClearFlag(DMA1_FLAG_GL7);
		DMA_Cmd(DMA1_Channel7, DISABLE);
	}
}

void DMA1_Channel6_IRQHandler(void)
{
	int m, i, j, head;
	float d;
	int find_head = 0;
	uint8_t temp_data[4];
	DMA_InitTypeDef DMA_InitStructure;

	if (DMA_GetITStatus(DMA1_FLAG_TC6) == SET) {
		DMA_ClearFlag(DMA1_FLAG_GL6);
		DMA_Cmd(DMA1_Channel6, DISABLE);
		for (m = 0; m < 21; m++) {
			if (Uart_Rx[m] == 0xff && Uart_Rx[(m + 1) % 21] == 0x55 && Uart_Rx[(m + 2) % 21] == 0xaa && Uart_Rx[(m + 3) % 21] == 0x10) {
				head = m;
				find_head = 1;
			}
		}
		if (find_head == 1) {
			for (i = 0; i < 4; i++) {
				for (j = 0; j < 4; j++) {
					temp_data[j] = (Uart_Rx[(head + 4 + 4 * i + j) % 21]);
				}
				d = *((float *)(&temp_data));
				switch (i) {
				case 0:
					Move_Y = d;
				case 1:
					Move_X = d;
				case 2:
					Move_Z = d;
				default:
					break;
				}
			}
		}

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		DMA_DeInit(DMA1_Channel6);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Rx;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStructure.DMA_BufferSize = 21;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel6, &DMA_InitStructure);
		DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
		DMA_Cmd(DMA1_Channel6, ENABLE);
	}
}






