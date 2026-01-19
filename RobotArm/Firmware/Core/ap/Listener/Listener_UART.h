/*
 * Listener_UART.h
 *
 *  Created on: Jan 11, 2026
 *      Author: 82109
 */

#ifndef AP_LISTENER_LISTENER_UART_H_
#define AP_LISTENER_LISTENER_UART_H_

#include <stdio.h>
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "../../driver/FpgaUartDrv/FpgaUartDrv.h"
#include "../Model/Model_RobotArm.h"

void Listener_Uart_Init();
void Listener_Uart_Execute();

#endif /* AP_LISTENER_LISTENER_UART_H_ */
