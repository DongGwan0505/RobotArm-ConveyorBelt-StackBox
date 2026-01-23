/*
 * DCMOTOR.h
 *
 *  Created on: Jan 13, 2026
 *      Author: kccistc
 */

#ifndef DRIVER_DCMOTOR_DCMOTOR_H_
#define DRIVER_DCMOTOR_DCMOTOR_H_

#include "stm32f4xx_hal.h"
#include "tim.h" // PWM용 타이머 (htim3)

#define NUM_MOTORS 3

// --- Motor 0 (ID: 0) ---
#define DC0_IN1_Pin        GPIO_PIN_1
#define DC0_IN1_GPIO_Port  GPIOB
#define DC0_IN2_Pin        GPIO_PIN_15
#define DC0_IN2_GPIO_Port  GPIOB
#define DC0_PWM_CHANNEL    TIM_CHANNEL_1

// --- Motor 1 (ID: 1) ---
#define DC1_IN1_Pin        GPIO_PIN_13
#define DC1_IN1_GPIO_Port  GPIOB
#define DC1_IN2_Pin        GPIO_PIN_14
#define DC1_IN2_GPIO_Port  GPIOB
#define DC1_PWM_CHANNEL    TIM_CHANNEL_2

// --- Motor 2 (ID: 2) ---
#define DC2_IN1_Pin        GPIO_PIN_3
#define DC2_IN1_GPIO_Port  GPIOB
#define DC2_IN2_Pin        GPIO_PIN_10
#define DC2_IN2_GPIO_Port  GPIOA
#define DC2_PWM_CHANNEL    TIM_CHANNEL_3

void DCMotor_Init(void);
void DCMotor_Speed(int id, uint16_t speed);
void DCMotor_Forward(int id);
void DCMotor_Backward(int id);
void DCMotor_Stop(int id);

#endif
