/*
 * Listener.c
 *
 *  Created on: Jan 5, 2026
 *      Author: kccistc
 */


#include "Listener.h"


void Listener_Init()
{
	Listener_RackPinion_Init();
}

void Listener_Excute()
{
	modeState_t modeState = Model_GetMode();

	Listener_CheckEvent();
	switch (modeState)
	{
	case RP_MODE:
		Listener_RackPinion_Excute();
		break;
	}
}

void Listener_CheckEvent()
{
//	if (Button_GetState(&hbtnMode) == ACT_RELEASED){
//		osMessagePut(modeEventMsgBox, EVENT_MODE, 0);
//	}
}
