/*
 * Listener_RackPinion.c
 *
 *  Created on: Jan 9, 2026
 *      Author: kccistc
 */

#include "Listener_RackPinion.h"

static Ir_t ir_sensors[NUM_RP];
static IrState_t prevStates[NUM_RP];

void Listener_RackPinion_Init() {
	Ir_Init(&ir_sensors[0], IR0_GPIO_Port, IR0_Pin);
	Ir_Init(&ir_sensors[1], IR1_GPIO_Port, IR1_Pin);
	Ir_Init(&ir_sensors[2], IR2_GPIO_Port, IR2_Pin);

	for (int i = 0; i < NUM_RP; i++) {
	        prevStates[i] = Ir_Check(&ir_sensors[i]);
	    }
}

void Listener_RackPinion_Excute() {
	for (int i = 0; i < NUM_RP; i++) {
	        IrState_t currState = Ir_Check(&ir_sensors[i]);

	        // 실시간 IR 상태를 Model에 ID별로 저장 (id 인자 추가됨)
	        Model_SetIrStatus(i, (currState == IR_DETECTED) ? 1 : 0);

	        // 해당 시스템(i)의 현재 상태를 가져옴
	        rpState_t sysState = Model_GetrpState(i);

	        // WAIT_BOX 상태에서 엣지 감지 (CLEAR → DETECTED)
	        if (sysState == RP_WAIT_BOX) {
	            if (prevStates[i] == IR_CLEAR && currState == IR_DETECTED) {
	                /* * 4. 데이터 패킹: 어떤 랙인지 구분하기 위해 ID 정보를 합쳐서 전송
	                 * 상위 8비트: 이벤트 종류 (EVENT_RP_BOX_IN)
	                 * 하위 8비트: 시스템 ID (i = 0, 1, 2)
	                 */
	                uint32_t eventData = (EVENT_RP_BOX_IN << 8) | i;
	                osMessagePut(rpEventMsgBox, eventData, 0);
	            }
	        }

	        // 이전 상태 업데이트
	        prevStates[i] = currState;
	    }
}
