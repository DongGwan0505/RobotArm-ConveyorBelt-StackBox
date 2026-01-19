/*
 * FpgaUartDrv.c
 *
 *  Created on: Jan 11, 2026
 *      Author: 82109
 */

#include "FpgaUartDrv.h"
// FpgaUartDrv.c v2 2026-01-10 15:32 (uint8_t ReadBytes)

static uint8_t rx_dma[UART_RX_DMA_BUFFER_SIZE];
static volatile uint16_t rx_rd = 0;
static inline uint16_t DmaWrPTr();
static inline uint16_t DmaWrPTr()
{
	return (uint16_t)(UART_RX_DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx));
}

void FpgaUartDrvInit()
{
	rx_rd = 0;
	HAL_UART_Receive_DMA(&huart1, rx_dma, UART_RX_DMA_BUFFER_SIZE);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
}

uint16_t FpgaUartDrvGetAvailableBytes()
{
	uint16_t wr = DmaWrPTr();
	uint16_t rd = rx_rd;

	if(wr >= rd) return (wr - rd);
	return (UART_RX_DMA_BUFFER_SIZE - rd) + wr;

}

uint16_t FpgaUartDrvReadBytes(uint8_t *pBuf, uint16_t MaxCount)
{
	uint16_t Available = FpgaUartDrvGetAvailableBytes();
	uint16_t n = (Available < MaxCount) ? Available:MaxCount;

	for(uint16_t i = 0; i<n; i++){
		pBuf[i] = rx_dma[rx_rd++];
		if(rx_rd >= UART_RX_DMA_BUFFER_SIZE) rx_rd = 0;
	}
	return n;
}

