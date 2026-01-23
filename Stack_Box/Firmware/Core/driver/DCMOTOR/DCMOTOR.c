/*
 * DCMOTOR.c
 *
 * Created on: Jan 13, 2026
 * Author: kccistc
 */
#include "DCMOTOR.h"

static uint32_t motor_channels[NUM_MOTORS] = {
    DC0_PWM_CHANNEL, // Motor 0
    DC1_PWM_CHANNEL, // Motor 1
    DC2_PWM_CHANNEL  // Motor 2
};

void DCMotor_Init(void)
{
    for (int i = 0; i < NUM_MOTORS; i++) {
        HAL_TIM_PWM_Start(&htim3, motor_channels[i]);
        DCMotor_Speed(i, 0);
        DCMotor_Stop(i);
    }
}

void DCMotor_Speed(int id, uint16_t speed)
{
    if (id >= NUM_MOTORS) return;
    if (speed > 1000) speed = 1000;
    __HAL_TIM_SET_COMPARE(&htim3, motor_channels[id], speed);
}

void DCMotor_Forward(int id)
{
    if (id == 0) {
        HAL_GPIO_WritePin(DC0_IN1_GPIO_Port, DC0_IN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(DC0_IN2_GPIO_Port, DC0_IN2_Pin, GPIO_PIN_RESET);
    }
    else if (id == 1) {
        HAL_GPIO_WritePin(DC1_IN1_GPIO_Port, DC1_IN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(DC1_IN2_GPIO_Port, DC1_IN2_Pin, GPIO_PIN_RESET);
    }
    else if (id == 2) {
        HAL_GPIO_WritePin(DC2_IN1_GPIO_Port, DC2_IN1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(DC2_IN2_GPIO_Port, DC2_IN2_Pin, GPIO_PIN_RESET);
    }
}

void DCMotor_Backward(int id)
{
    if (id == 0) {
        HAL_GPIO_WritePin(DC0_IN1_GPIO_Port, DC0_IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DC0_IN2_GPIO_Port, DC0_IN2_Pin, GPIO_PIN_SET);
    }
    else if (id == 1) {
        HAL_GPIO_WritePin(DC1_IN1_GPIO_Port, DC1_IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DC1_IN2_GPIO_Port, DC1_IN2_Pin, GPIO_PIN_SET);
    }
    else if (id == 2) {
        HAL_GPIO_WritePin(DC2_IN1_GPIO_Port, DC2_IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DC2_IN2_GPIO_Port, DC2_IN2_Pin, GPIO_PIN_SET);
    }
}

void DCMotor_Stop(int id)
{
    if (id == 0) {
        HAL_GPIO_WritePin(DC0_IN1_GPIO_Port, DC0_IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DC0_IN2_GPIO_Port, DC0_IN2_Pin, GPIO_PIN_RESET);
    }
    else if (id == 1) {
        HAL_GPIO_WritePin(DC1_IN1_GPIO_Port, DC1_IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DC1_IN2_GPIO_Port, DC1_IN2_Pin, GPIO_PIN_RESET);
    }
    else if (id == 2) {
        HAL_GPIO_WritePin(DC2_IN1_GPIO_Port, DC2_IN1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DC2_IN2_GPIO_Port, DC2_IN2_Pin, GPIO_PIN_RESET);
    }
}
