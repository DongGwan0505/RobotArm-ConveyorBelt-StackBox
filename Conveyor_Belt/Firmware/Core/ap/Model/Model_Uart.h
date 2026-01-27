/*
 * Model.h
 *
 *  Created on: Jan 7, 2026
 *      Author: kccistc
 */

#ifndef AP_MODEL_MODEL_UART_H_
#define AP_MODEL_MODEL_UART_H_

#include <stdint.h>
#include "cmsis_os.h"

typedef struct {
  uint8_t byte;
} uart_evt_t;

extern osMessageQId uartEvtMsgBox;
extern osPoolId poolUartEvt;

void Model_UartInit(void);
int  Model_UartPushByte(uint8_t b);
int  Model_UartPopByte(uint8_t *out, uint32_t timeout_ms);


#endif /* AP_MODEL_MODEL_UART_H_ */
