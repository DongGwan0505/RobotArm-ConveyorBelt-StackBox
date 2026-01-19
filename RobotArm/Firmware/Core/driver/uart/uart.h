/*
 * uart.h
 *
 *  Created on: Jan 10, 2026
 *      Author: kccistc
 */

#ifndef DRIVER_UART_UART_H_
#define DRIVER_UART_UART_H_

#include "main.h"
#include <stdio.h>

#define UART_RX_BUF_SIZE 512

extern UART_HandleTypeDef huart2;

void UART_Init(void);
void UART_Transmit(uint8_t *pData, uint16_t size);
uint8_t UART_GetRxData(void);
int UART_IsRxAvailable(void);

int _write(int file, char *ptr, int len);

#endif /* DRIVER_UART_UART_H_ */
