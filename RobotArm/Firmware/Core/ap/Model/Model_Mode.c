/*
 * Model_Mode.c
 *
 *  Created on: Jan 9, 2026
 *      Author: 82109
 */

#include "Model_Mode.h"

modeState_t modeState = MODE_AUTO;

osMessageQId modeEventMsgBox;
osMessageQDef(modeEventQueue, 4, uint16_t);

void Model_ModeInit()
{
    modeState = MODE_AUTO;
    modeEventMsgBox = osMessageCreate(osMessageQ(modeEventQueue), NULL);
}

void Model_SetMode(modeState_t mode)
{
    modeState = mode;
}

modeState_t Model_GetMode()
{
    return modeState;
}
