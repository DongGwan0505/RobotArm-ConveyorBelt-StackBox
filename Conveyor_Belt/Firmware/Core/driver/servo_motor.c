/*
 * servo_motor.c
 *
 *  Created on: Jan 8, 2026
 *      Author: kccistc
 */
#include "servo_motor.h"


void Servo_Init(ServoMotor_t *servo, TIM_HandleTypeDef *htim, uint32_t channel)
{
	servo->htim = htim;
	servo->channel = channel;
	servo->current_angle = 0;

	HAL_TIM_PWM_Start(servo->htim, servo->channel);

	Set_Servo_Angle(servo, 0);
}

void Set_Servo_Angle(ServoMotor_t *servo, uint8_t angle)
{
	if (angle > 180) angle = 180;

	servo->current_angle = angle;

	uint32_t pulse_length = 500 + (angle * 2000/180);

	__HAL_TIM_SET_COMPARE(servo->htim, servo->channel, pulse_length);
}

void ServoMotor_CCW(ServoMotor_t *servo, uint8_t steps)
{
	int16_t target_angle = servo->current_angle + steps;

	if (target_angle > 180){
		target_angle = 180;
	}
	Set_Servo_Angle(servo, (uint8_t)target_angle);
}

void ServoMotor_CW(ServoMotor_t *servo, uint8_t steps)
{
	// 180도 -> 0도
	int16_t target_angle = servo->current_angle + steps;

	if (target_angle > 0){
		target_angle = 0;
	}

	Set_Servo_Angle(servo, (uint8_t)target_angle);
}

