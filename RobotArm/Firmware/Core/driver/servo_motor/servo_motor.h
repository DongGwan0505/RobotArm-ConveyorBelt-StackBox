/*
 * servo_motor.h
 *
 *  Created on: Jan 7, 2026
 *      Author: 82109
 */

#ifndef DRIVER_SERVO_MOTOR_H_
#define DRIVER_SERVO_MOTOR_H_

#include "stm32f4xx_hal.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "tim.h"

typedef struct
{
	TIM_HandleTypeDef *htim;
	uint32_t channel;
	int16_t  current_angle;

	float cmd_angle_f; // current seted angle -> real pwm angle
	uint16_t target_angle;
	float slew_deg_s; // max degree differ per sec
}ServoMotor_t;


void Servo_Init(ServoMotor_t *servo, TIM_HandleTypeDef *htim, uint32_t channel);
void Servo_SetSlewRate(ServoMotor_t *servo, float deg_per_s);
void Set_Servo_Angle_Legacy(ServoMotor_t *servo, uint8_t angle );
void Set_Servo_Angle(ServoMotor_t *servo, uint8_t angle);
void Servo_Update(ServoMotor_t *servo , float dt_s);
void ServoMotor_CCW(ServoMotor_t *servo, uint8_t steps);
void ServoMotor_CW(ServoMotor_t *servo, uint8_t steps);

#endif /* DRIVER_SERVO_MOTOR_H_ */
