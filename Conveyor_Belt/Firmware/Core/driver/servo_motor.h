/*
 * servo_motor.h
 *
 *  Created on: Jan 8, 2026
 *      Author: kccistc
 */

#ifndef DRIVER_SERVO_MOTOR_H_
#define DRIVER_SERVO_MOTOR_H_

#include <stdint.h>
#include <stdio.h>
#include "tim.h"

typedef struct
{
	TIM_HandleTypeDef *htim;
	uint32_t channel;
	int16_t  current_angle;
}ServoMotor_t;


void Servo_Init(ServoMotor_t *servo, TIM_HandleTypeDef *htim, uint32_t channel);
void Set_Servo_Angle(ServoMotor_t *servo, uint8_t angle);
void ServoMotor_CCW(ServoMotor_t *servo, uint8_t steps);
void ServoMotor_CW(ServoMotor_t *servo, uint8_t steps);



#endif /* DRIVER_SERVO_MOTOR_H_ */
