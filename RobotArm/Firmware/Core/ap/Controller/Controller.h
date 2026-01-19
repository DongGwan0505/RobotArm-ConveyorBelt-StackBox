/*
 * Controller.h
 *
 *  Created on: Jan 8, 2026
 *      Author: 82109
 */

#ifndef AP_CONTROLLER_CONTROLLER_H_
#define AP_CONTROLLER_CONTROLLER_H_

#include <stdint.h>
#include "cmsis_os.h"
#include "Controller_RobotArm.h"
#include "../Model/Model_Mode.h"

void Controller_Init();
void Controller_Excute();
void Controller_CheckEventMode();

#endif /* AP_CONTROLLER_CONTROLLER_H_ */
