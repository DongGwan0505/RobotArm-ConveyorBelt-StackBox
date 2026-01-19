/*
 * Listener_RobotArm.h
 *
 *  Created on: Jan 9, 2026
 *      Author: 82109
 */

#ifndef AP_LISTENER_LISTENER_ROBOTARM_H_
#define AP_LISTENER_LISTENER_ROBOTARM_H_

#include <stdio.h>
#include "cmsis_os.h"
#include "../driver/button/button.h"
#include "../Model/Model_RobotArm.h"

#define BTN_ROBOTARM_BASE_G_GPIO     GPIOC
#define BTN_ROBOTARM_BASE_G_PIN      GPIO_PIN_0
#define BTN_ROBOTARM_BASE_R_GPIO     GPIOC
#define BTN_ROBOTARM_BASE_R_PIN      GPIO_PIN_1

#define BTN_ROBOTARM_SHOULDER_G_GPIO GPIOC
#define BTN_ROBOTARM_SHOULDER_G_PIN  GPIO_PIN_2
#define BTN_ROBOTARM_SHOULDER_R_GPIO GPIOC
#define BTN_ROBOTARM_SHOULDER_R_PIN  GPIO_PIN_3

#define BTN_ROBOTARM_ELBOW_G_GPIO    GPIOC
#define BTN_ROBOTARM_ELBOW_G_PIN     GPIO_PIN_4
#define BTN_ROBOTARM_ELBOW_R_GPIO    GPIOC
#define BTN_ROBOTARM_ELBOW_R_PIN     GPIO_PIN_5

#define BTN_ROBOTARM_GRIPPER_G_GPIO  GPIOC
#define BTN_ROBOTARM_GRIPPER_G_PIN   GPIO_PIN_10
#define BTN_ROBOTARM_GRIPPER_R_GPIO  GPIOC
#define BTN_ROBOTARM_GRIPPER_R_PIN   GPIO_PIN_11

void Listener_RobotArm_Init();
void Listener_RobotArm_Excute();
void Listener_RobotArm_CheckButton();


#endif /* AP_LISTENER_LISTENER_ROBOTARM_H_ */
