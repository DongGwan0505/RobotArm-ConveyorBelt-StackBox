/*
 * Controller.c
 *
 *  Created on: Jan 5, 2026
 *      Author: kccistc
 */


#include "Controller.h"

void Controller_Init()
{
	Model_ModeInit();
	Controller_RP_Init();
}

void Controller_Excute()
{
	modeState_t modeState = Model_GetMode();

	Controller_CheckEventMode();

	switch (modeState)
	{
	case RP_MODE:
			Controller_RP_Execute();
			break;

	}
}

void Controller_CheckEventMode()
{
	osEvent evt = osMessageGet(modeEventMsgBox, 0);
	uint16_t evtState;

	if (evt.status == osEventMessage) {
		evtState = evt.value.v;
		if (evtState != EVENT_MODE) return;

		modeState_t state = Model_GetMode();
		if (state == RP_MODE) {
			Mode_SetMode(RP_MODE);
		}
	}
}
