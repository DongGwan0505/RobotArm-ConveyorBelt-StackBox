/*
 * uart.c
 *
 *  Created on: Jan 10, 2026
 *      Author: kccistc
 */

#include "uart.h"

static uint8_t rxBuffer[UART_RX_BUF_SIZE];
static volatile uint16_t rxHead = 0;
static volatile uint16_t rxTail = 0;
static uint8_t rxChar;

void UART_Init(void)
{
	HAL_UART_Receive_IT(&huart2, &rxChar, 1);
}

void UART_Transmit(uint8_t *pData, uint16_t size)
{
	HAL_UART_Transmit(&huart2, pData, size, 100);
}

int UART_IsRxAvailable(void)
{
	return (rxHead != rxTail);
}

uint8_t UART_GetRxData(void)
{
	uint8_t data = 0;

	if (rxHead != rxTail)
	{
		data = rxBuffer[rxTail];
		rxTail = (rxTail + 1) % UART_RX_BUF_SIZE;
	}

	return data;
}

void HAL_UART_RXCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		uint16_t nextHead = (rxHead + 1) % UART_RX_BUF_SIZE;

		if (nextHead != rxTail)
		{
			rxBuffer[rxHead] = rxChar;
			rxHead = nextHead;
		}

		HAL_UART_Receive_IT(&huart2, &rxChar, 1);
	}
}


