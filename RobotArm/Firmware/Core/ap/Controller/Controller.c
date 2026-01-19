/*
 * Controller.c
 *
 * Created on: Jan 9, 2026
 * Author: 82109
 */

#include "Controller.h"

void Controller_Init()
{
    Controller_RobotArm_Init();
}

void Controller_Excute()
{
    modeState_t modeState = Model_GetMode();

    Controller_CheckEventMode(); // 모드 변경 버튼 확인

    switch (modeState)
    {
    case MODE_MANUAL://수동 모드
        Controller_RobotArm_Manual();
        break;

    case MODE_AUTO://자동 모드 (FPGA)
        Controller_RobotArm_Auto();
        break;

    case MODE_STANBY:
        // 대기 상태 (필요하다면 LED 깜빡임 등 처리)
        break;
    }
}

void Controller_CheckEventMode()
{
    osEvent evt = osMessageGet(modeEventMsgBox, 0);

    if (evt.status == osEventMessage) {
        // 이벤트 값 확인 (여기서는 1가지만 쓰므로 단순 확인)
        if (evt.value.v == EVENT_CHANGE_MODE) {
            modeState_t current = Model_GetMode();

            // 모드 순환: STANDBY -> MANUAL -> AUTO -> STANDBY ...
            if (current == MODE_STANBY) {
                Model_SetMode(MODE_MANUAL);
            } else if (current == MODE_MANUAL) {
                Model_SetMode(MODE_AUTO);
            } else {
                Model_SetMode(MODE_STANBY);
            }
        }
    }
}
