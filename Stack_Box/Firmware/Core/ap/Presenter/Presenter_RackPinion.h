/*
 * Presenter_RackPinion.h
 *
 *  Created on: Jan 9, 2026
 *      Author: kccistc
 */

#ifndef AP_PRESENTER_PRESENTER_RACKPINION_H_
#define AP_PRESENTER_PRESENTER_RACKPINION_H_

#include "cmsis_os.h"
#include "../Model/Model_RackPinion.h"
#include <stdio.h>
#include <string.h>
#include "usart.h"

void Presenter_RP_Init(void);
void Presenter_RP_Execute(void);

#endif /* AP_PRESENTER_PRESENTER_RACKPINION_H_ */
