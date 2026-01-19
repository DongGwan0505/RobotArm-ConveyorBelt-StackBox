/*
 * FpgaUartDrv.h
 *
 *  Created on: Jan 11, 2026
 *      Author: 82109
 */

#ifndef DRIVER_FPGAUARTDRV_FPGAUARTDRV_H_
#define DRIVER_FPGAUARTDRV_FPGAUARTDRV_H_

#include <stdint.h>
#include <string.h>
#include "usart.h"
#include "stm32f4xx_hal.h"

#define UART_RX_DMA_BUFFER_SIZE 128


void FpgaUartDrvInit();
void FpgaUartDrvOnIdleFromISR(void);
uint16_t FpgaUartDrvGetAvailableBytes();
uint16_t FpgaUartDrvReadBytes(uint8_t *pBuf, uint16_t MaxCount);

#endif /* DRIVER_FPGAUARTDRV_FPGAUARTDRV_H_ */
