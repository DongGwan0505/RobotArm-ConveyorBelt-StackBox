/*
 * IR.h
 *
 *  Created on: Jan 8, 2026
 *      Author: kccistc
 */

#ifndef DRIVER_IR_H_
#define DRIVER_IR_H_

#include "stm32f4xx_hal.h"

#define IR_GPIO		GPIOA
#define IR_PIN		GPIO_PIN_8

// 센서 상태
typedef enum {
	IR_CLEAR = 0,
	IR_DETECTED = 1
} IrState_t;

// 센서 객체 구조체
typedef struct {
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
	IrState_t state;
} Ir_t;

void Ir_Init(Ir_t *ir, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
IrState_t Ir_Check(Ir_t *ir);


#endif /* DRIVER_IR_H_ */
