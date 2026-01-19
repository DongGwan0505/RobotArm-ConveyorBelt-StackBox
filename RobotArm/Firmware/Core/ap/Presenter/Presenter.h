/*
 * Presenter.h
 *
 *  Created on: Jan 8, 2026
 *      Author: 82109
 */

#ifndef AP_PRESENTER_PRESENTER_H_
#define AP_PRESENTER_PRESENTER_H_

#include "Presenter_RobotArm.h"

#include "../Model/Model_Mode.h"

#include "i2c.h" // LCD가 I2C를 쓴다면 필요

void Presenter_Init();
void Presenter_Excute();


#endif /* AP_PRESENTER_PRESENTER_H_ */
