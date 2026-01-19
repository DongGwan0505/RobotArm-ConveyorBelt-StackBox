/*
 * Listener.c
 *
 *  Created on: Jan 8, 2026
 *      Author: 82109
 */

#include "Listener.h"

//FPGA에서 받는 경우와 테스트용 버튼 값 받는 모드 전환용
hBtn hbtnMode;

void Listener_Init()
{
	Button_Init(&hbtnMode, BTN_MODE_GPIO, BTN_MODE_PIN);
	Listener_RobotArm_Init();
	Listener_Uart_Init();
}

void Listener_Excute()
{
	modeState_t modeState = Model_GetMode();


	Listener_CheckEvent();
	switch (modeState)
	{
	case MODE_AUTO:
		Listener_Uart_Execute();
		break;
	case MODE_MANUAL:
		Listener_RobotArm_Excute();
		break;
	case MODE_STANBY:
		//대기
		break;
	}
}

void Listener_CheckEvent()
{
	if (Button_GetState(&hbtnMode) == ACT_RELEASED){
		osMessagePut(modeEventMsgBox, EVENT_CHANGE_MODE, 0);
	}
}

