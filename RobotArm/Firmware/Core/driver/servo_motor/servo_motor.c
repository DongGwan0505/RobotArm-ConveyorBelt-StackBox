/*
 * servo_motor.c
 *
 *  Created on: Jan 7, 2026
 *      Author: 82109
 */

#include "servo_motor.h"

static float clampf(float x, float lo, float hi);
static int16_t clamp_i16 (int16_t x, int16_t lo, int16_t hi);
static void apply_pwm(ServoMotor_t *servo, float angle_deg);

static float clampf(float x, float lo, float hi)
{
	if(x < lo) return lo;
	if(x > hi) return hi;
	return x;
}

static int16_t clamp_i16 (int16_t x, int16_t lo, int16_t hi)
{
	if(x < lo) return lo;
	if(x > hi) return hi;
	return x;
}

static void apply_pwm(ServoMotor_t *servo, float angle_deg)
{
	angle_deg = clampf(angle_deg, 0.0f, 180.0f);

	float pulse_us = 500.0f + (angle_deg*(2000.0f/180.0f));
	uint32_t ccr = (uint32_t)(pulse_us + 0.5f);

	__HAL_TIM_SET_COMPARE(servo->htim, servo->channel, ccr);

	servo->cmd_angle_f = angle_deg;
	servo->current_angle = (int16_t)(angle_deg + 0.5f);
}


void Servo_Init(ServoMotor_t *servo, TIM_HandleTypeDef *htim, uint32_t channel)
{
	servo->htim = htim;
	servo->channel = channel;

	servo->slew_deg_s = 120.0f;
	servo->target_angle = 0;
	servo->cmd_angle_f = 0.0f;
	servo->current_angle = 0;

	HAL_TIM_PWM_Start(servo->htim, servo->channel);
	Set_Servo_Angle(servo,0);
//  Leagcy Code
//	servo->htim = htim;
//	servo->channel = channel;
//	servo->current_angle = 0;
//
//	HAL_TIM_PWM_Start(servo->htim, servo->channel);
//
//	Set_Servo_Angle(servo, 0);
}

void Servo_SetSlewRate(ServoMotor_t *servo, float deg_per_s)
{
	if(!servo) return;
	if(deg_per_s < 1.0f) deg_per_s = 1.0f;
	if(deg_per_s > 1000.0f) deg_per_s = 1000.0f;
	servo->slew_deg_s = deg_per_s;
}

void Set_Servo_Angle_Legacy(ServoMotor_t *servo, uint8_t angle )
{
	if (angle > 180) angle = 180;

		servo->current_angle = angle;

		uint32_t pulse_length = 500 + (angle * 2000/180);

		__HAL_TIM_SET_COMPARE(servo->htim, servo->channel, pulse_length);
}

void Set_Servo_Angle(ServoMotor_t *servo, uint8_t angle)
{
	if(!servo) return;
	if(angle > 180) angle = 180;

	servo->target_angle = (int16_t)angle;
	apply_pwm(servo,(float)angle);

//  Legacy Code
//	//angle<=0, angle>=180 방지
//	if (angle > 180) angle = 180;
//
//	servo->current_angle = angle;
//
//	uint32_t pulse_length = 500 + (angle * 2000/180);
//
//	__HAL_TIM_SET_COMPARE(servo->htim, servo->channel, pulse_length);
}


void Servo_Update(ServoMotor_t *servo , float dt_s)
{
	if(!servo) return;
	if(dt_s <= 0.0f) return;
	float target = (float)clamp_i16(servo->target_angle,0,180);
	float err = target - servo->cmd_angle_f;

	if(fabsf(err)<0.05f) return;

	float step_max = servo->slew_deg_s * dt_s;
	if(step_max<0.1f)step_max = 0.1f;

	float step = clampf(err, -step_max, step_max);
	apply_pwm(servo, servo->cmd_angle_f + step);
}

void ServoMotor_CCW(ServoMotor_t *servo, uint8_t steps)
{
	if(!servo) return;
	int16_t t = servo->target_angle + (int16_t)steps;
	servo->target_angle = clamp_i16(t,0,180);

//  Legacy Codee
//	// 0도 -> 180도
//	int16_t target_angle = servo->current_angle + steps;
//
//	if (target_angle > 180){
//		target_angle = 180;
//	}
//	Set_Servo_Angle(servo, (uint8_t)target_angle);
}

void ServoMotor_CW(ServoMotor_t *servo, uint8_t steps)
{
	if(!servo) return ;
	int16_t t = servo->target_angle -(int16_t)steps;
	servo->target_angle = clamp_i16(t, 0, 180);

//	// 180도 -> 0도
//	int16_t target_angle = servo->current_angle - steps;
//
//	if (target_angle < 0){
//		target_angle = 0;
//	}
//
//	Set_Servo_Angle(servo, (uint8_t)target_angle);
}

