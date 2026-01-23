/*
 * Listener_RackPinion.h
 *
 *  Created on: Jan 9, 2026
 *      Author: kccistc
 */

#ifndef AP_LISTENER_LISTENER_RACKPINION_H_
#define AP_LISTENER_LISTENER_RACKPINION_H_

#include "cmsis_os.h"

#include "../../driver/IR_RackPinion/IR_RackPinion.h"
#include "../Model/Model_RackPinion.h"

void Listener_RackPinion_Init(void);
void Listener_RackPinion_Excute(void);

#endif /* AP_LISTENER_LISTENER_RACKPINION_H_ */
