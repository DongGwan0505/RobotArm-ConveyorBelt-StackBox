/*
 * IR.c
 *
 *  Created on: Jan 9, 2026
 *      Author: kccistc
 */


#include "../IR_RackPinion/IR_RackPinion.h"

void Ir_Init(Ir_t *ir, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	ir->GPIOx = GPIOx;
	ir->GPIO_Pin = GPIO_Pin;
	ir->state = IR_CLEAR;
	ir->prevState = IR_CLEAR;
	ir->edgeDetected = 0;

	// 초기 상태 읽기
	GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
	if (pinState == GPIO_PIN_RESET) {
		ir->state = IR_DETECTED;
		ir->prevState = IR_DETECTED;
	}
}

IrState_t Ir_Check(Ir_t *ir)
{
	GPIO_PinState pinState = HAL_GPIO_ReadPin(ir->GPIOx, ir->GPIO_Pin);

	ir->prevState = ir->state;

	if (pinState == GPIO_PIN_RESET) {
		ir->state = IR_DETECTED;
	}
	else {
		ir->state = IR_CLEAR;
	}

	// 엣지 감지 (CLEAR -> DETECTED)
	if (ir->prevState == IR_CLEAR && ir->state == IR_DETECTED) {
		ir->edgeDetected = 1;
	}

	return ir->state;
}

// 현재 상태만 반환 (엣지 상관없이)
IrState_t Ir_GetCurrentState(Ir_t *ir)
{
	GPIO_PinState pinState = HAL_GPIO_ReadPin(ir->GPIOx, ir->GPIO_Pin);

	if (pinState == GPIO_PIN_RESET) {
		return IR_DETECTED;
	}
	return IR_CLEAR;
}

// 상승 엣지 감지 확인 후 플래그 클리어
uint8_t Ir_IsRisingEdge(Ir_t *ir)
{
	if (ir->edgeDetected) {
		ir->edgeDetected = 0;
		return 1;
	}
	return 0;
}
