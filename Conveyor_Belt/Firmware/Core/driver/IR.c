/*
 * IR.c
 *
 *  Created on: Jan 8, 2026
 *      Author: kccistc
 */

#include "IR.h"

void Ir_Init(Ir_t *ir, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	ir->GPIOx = GPIOx;
	ir->GPIO_Pin = GPIO_Pin;
	ir->state = IR_CLEAR;
}

IrState_t Ir_Check(Ir_t *ir)
{
	GPIO_PinState pinState = HAL_GPIO_ReadPin(ir->GPIOx, ir->GPIO_Pin);

	if (pinState == GPIO_PIN_RESET) {
		ir->state = IR_DETECTED;
	}
	else {
		ir->state = IR_CLEAR;
	}

	return ir->state;
}
