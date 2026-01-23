/*
 * Model_RackPinion.h
 *
 * Created on: Jan 9, 2026
 * Author: kccistc
 */

#ifndef AP_MODEL_MODEL_RACKPINION_H_
#define AP_MODEL_MODEL_RACKPINION_H_

#include <stdint.h>
#include "cmsis_os.h"

#define NUM_RP    3

typedef enum {
    RP_WAIT_BOX,
    RP_MOVE_NEXT,
    RP_PUSH_OUT,
    RP_DONE,
    RP_RETURN_HOME
} rpState_t;

typedef enum {
    EVENT_RP_BOX_IN = 0,
    EVENT_RP_MOVE_DONE = 1
} rpEvent_t;


typedef struct __attribute__((packed)) {
    int8_t id;            // To identify which rack (0, 1, 2)
    int8_t irStatus;      // IR sensor status (0: None, 1: Detected)
    int8_t boxCount;      // Current box count
} rp_t;

extern osMessageQId rpEventMsgBox;
extern osPoolId poolrpData;
extern osMessageQId rpDataMsgBox;

void Model_rpInit(void);
void Model_SetrpState(int id, rpState_t state);
rpState_t Model_GetrpState(int id);
void Model_IncrementrpBoxCount(int id);
int Model_GetrpBoxCount(int id);
void Model_ResetrpBoxCount(int id);
void Model_SetIrStatus(int id, int status);
int Model_GetIrStatus(int id);

#endif /* AP_MODEL_MODEL_RACKPINION_H_ */
