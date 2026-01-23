/*
 * IR.h
 *
 *  Created on: Jan 9, 2026
 *      Author: kccistc
 */

#ifndef DRIVER_IR_RACKPINION_IR_RACKPINION_H_
#define DRIVER_IR_RACKPINION_IR_RACKPINION_H_

#include "stm32f4xx_hal.h"


#define IR0_Pin GPIO_PIN_8
#define IR0_GPIO_Port GPIOA
#define IR1_Pin GPIO_PIN_10
#define IR1_GPIO_Port GPIOB
#define IR2_Pin GPIO_PIN_4
#define IR2_GPIO_Port GPIOB
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
	IrState_t prevState;
	uint8_t edgeDetected; // 엣지 감지 플래그
} Ir_t;

void Ir_Init(Ir_t *ir, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
IrState_t Ir_Check(Ir_t *ir);
IrState_t Ir_GetCurrentState(Ir_t *ir);  // 현재 상태만 반환 (엣지 무관)
uint8_t Ir_IsRisingEdge(Ir_t *ir);       // 상승 엣지 감지 (CLEAR -> DETECTED)

#endif /* DRIVER_IR_RACKPINION_IR_RACKPINION_H_ */
