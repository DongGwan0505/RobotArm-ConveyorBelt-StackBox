/*
 * Controller_RackPinion.h
 *
 * Created on: Jan 9, 2026
 * Author: kccistc
 */

#ifndef AP_CONTROLLER_CONTROLLER_RACKPINION_H_
#define AP_CONTROLLER_CONTROLLER_RACKPINION_H_

#include <stdint.h>
#include "cmsis_os.h"
#include "../Model/Model_RackPinion.h"
#include "../../driver/DCMOTOR/DCMOTOR.h"
#include "usart.h"

void Controller_RP_Init(void);
void Controller_RP_Execute(void);

void Controller_RP_Handle_WaitBox(int id, uint8_t event);
void Controller_RP_Handle_MoveNext(int id, uint8_t event);
void Controller_RP_Handle_PushOut(int id, uint8_t event); // 사용 시 id 추가
void Controller_RP_Handle_Done(int id, uint8_t event);
void Controller_RP_Handle_ReturnHome(int id, uint8_t event);
void Controller_RP_UpdateToPresenter(int id);


#endif
