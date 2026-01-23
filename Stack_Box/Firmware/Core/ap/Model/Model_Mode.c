/*
 * Model_mode.c
 *
 *  Created on: Jan 9, 2026
 *      Author: kccistc
 */


#include "Model_Mode.h"

modeState_t modeState = RP_MODE;

osMessageQId modeEventMsgBox;
osMessageQDef (modeEventQueue, 4, uint16_t);

void Model_ModeInit()
{
	modeState = RP_MODE;
	modeEventMsgBox = osMessageCreate(osMessageQ(modeEventQueue), NULL);
}

void Mode_SetMode(modeState_t mode)
{
	modeState = mode;
}

modeState_t Model_GetMode()
{
	return modeState;
}

