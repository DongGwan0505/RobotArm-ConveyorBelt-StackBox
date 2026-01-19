/*
 * Model_Mode.h
 *
 *  Created on: Jan 9, 2026
 *      Author: 82109
 */

#ifndef AP_MODEL_MODEL_MODE_H_
#define AP_MODEL_MODEL_MODE_H_

#include <stdint.h>
#include "cmsis_os.h"

// 로봇 팔 운전 모드 정의
typedef enum {
    MODE_AUTO,    // FPGA 데이터로 자동 동작
    MODE_MANUAL,  // 버튼으로 테스트용
	MODE_STANBY
} modeState_t;

// 모드 변경 이벤트
typedef enum {
    EVENT_CHANGE_MODE = 1
} modeEvent_t;

extern osMessageQId modeEventMsgBox;

void Model_ModeInit();
void Model_SetMode(modeState_t mode);
modeState_t Model_GetMode();

#endif /* AP_MODEL_MODEL_MODE_H_ */
