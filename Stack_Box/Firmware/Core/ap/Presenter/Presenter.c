/*
 * Presender.c
 *
 *  Created on: Jan 5, 2026
 *      Author: kccistc
 */


#include "Presenter.h"

void Presenter_Init()
{
	Presenter_RP_Init();
}

void Presenter_Excute()
{

	modeState_t modeState = Model_GetMode();
	switch (modeState)
	{
	case RP_MODE:
		Presenter_RP_Execute();
		break;

	}
}
