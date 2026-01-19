/*
 * Listener.h
 *
 *  Created on: Jan 8, 2026
 *      Author: 82109
 */

#ifndef AP_LISTENER_LISTENER_H_
#define AP_LISTENER_LISTENER_H_

#include "Listener_RobotArm.h"
#include "Listener_UART.h"
#include "../Model/Model_Mode.h"
#include "../Model/Model_RobotArm.h"
#include "../../driver/button/button.h"

#define BTN_MODE_GPIO GPIOC
#define BTN_MODE_PIN  GPIO_PIN_12

void Listener_Init();
void Listener_Excute();
void Listener_CheckEvent();

#endif /* AP_LISTENER_LISTENER_H_ */
