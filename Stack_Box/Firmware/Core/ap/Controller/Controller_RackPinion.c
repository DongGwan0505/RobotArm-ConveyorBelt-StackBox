/*
 * Controller_RackPinion.c
 *
 * Created on: Jan 9, 2026
 * Author: kccistc
 */

#include "Controller_RackPinion.h"
#include "../../driver/DCMOTOR/DCMOTOR.h"
#include <stdio.h>
#include <string.h>

extern UART_HandleTypeDef huart2;

#define DEBUG_MODE 0

#define MOTOR_SPEED_DEFAULT     1000
#define DONE_WAIT_TIME          3000

// --- Independent Parameters for each Rack (ID 0, 1, 2) ---

static uint32_t slotDurations[NUM_RP] = {3500, 2900, 1000};
static float returnCalibration[NUM_RP] = {0.9999f, 1.00f, 0.99f};

static uint32_t moveStartTime[NUM_RP] = {0,};
static uint32_t moveDuration[NUM_RP] = {0,};
static volatile uint8_t isMoving[NUM_RP] = {0,};
static uint8_t rpInitDone = 0;
static uint32_t totalMovedTime[NUM_RP] = {0,};


static void Debug_Print(const char *msg) {
#if DEBUG_MODE
    HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), 100);
#endif
}


static void StartDCMove(int id, uint32_t duration, int dir) {
    if (duration == 0 || id >= NUM_RP) return;

    char buf[80];
    snprintf(buf, sizeof(buf), "[RP %d MOTOR] Start: %lu ms, Dir: %s\r\n", id, duration, (dir == 0) ? "FWD" : "BWD");
    Debug_Print(buf);

    moveStartTime[id] = HAL_GetTick();
    moveDuration[id] = duration;
    isMoving[id] = 1;

    if (dir == 0) {
        totalMovedTime[id] += duration;
    }

    DCMotor_Speed(id, MOTOR_SPEED_DEFAULT);
    if (dir == 0) DCMotor_Forward(id);
    else DCMotor_Backward(id);
}


void Controller_RP_Init() {
    if (rpInitDone) return;
    rpInitDone = 1;

    Model_rpInit();
    DCMotor_Init();

    for (int i = 0; i < NUM_RP; i++) {
        Model_ResetrpBoxCount(i);
        Model_SetrpState(i, RP_WAIT_BOX);
        isMoving[i] = 0;
        totalMovedTime[i] = 0;
    }
    Debug_Print("[SYS] Multi Rack System Ready (ASCII Mode)\r\n");
}


void Controller_RP_Execute() {
    uint8_t pendingEvents[NUM_RP];
    memset(pendingEvents, 0xFF, sizeof(pendingEvents));

    osEvent evt;
    while ((evt = osMessageGet(rpEventMsgBox, 0)).status == osEventMessage) {
        uint8_t event = (evt.value.v >> 8) & 0xFF;
        uint8_t id = evt.value.v & 0xFF;
        if (id < NUM_RP) {
            pendingEvents[id] = event;
        }
    }

    for (int i = 0; i < NUM_RP; i++) {
        uint8_t currentEvent = pendingEvents[i];

        if (isMoving[i]) {
            if ((HAL_GetTick() - moveStartTime[i]) >= moveDuration[i]) {
                DCMotor_Stop(i);
                isMoving[i] = 0;
                currentEvent = EVENT_RP_MOVE_DONE;
            }
        }

       rpState_t currentState = Model_GetrpState(i);
        switch (currentState) {
            case RP_WAIT_BOX:    Controller_RP_Handle_WaitBox(i, currentEvent); break;
            case RP_MOVE_NEXT:   Controller_RP_Handle_MoveNext(i, currentEvent); break;
            case RP_DONE:        Controller_RP_Handle_Done(i, currentEvent); break;
            case RP_RETURN_HOME: Controller_RP_Handle_ReturnHome(i, currentEvent); break;
            default: break;
        }
    }
}


void Controller_RP_Handle_WaitBox(int id, uint8_t event) {
    static uint32_t delayStartTick[NUM_RP] = {0,};
    static uint8_t isDelaying[NUM_RP] = {0,};

    if (event == EVENT_RP_BOX_IN && isDelaying[id] == 0) {
        Model_IncrementrpBoxCount(id);
        Controller_RP_UpdateToPresenter(id);

        delayStartTick[id] = HAL_GetTick();
        isDelaying[id] = 1;

        char buf[80];
        snprintf(buf, sizeof(buf), "[RP %d BOX] Count: %d/5 (Debounce Start)\r\n", id, Model_GetrpBoxCount(id));
        Debug_Print(buf);
    }

    if (isDelaying[id]) {
        if ((HAL_GetTick() - delayStartTick[id]) >= 1000) {
            isDelaying[id] = 0;

            if (Model_GetrpBoxCount(id) >= 5) {
                Model_SetrpState(id, RP_DONE);
            } else {
                Model_SetrpState(id, RP_MOVE_NEXT);
               StartDCMove(id, slotDurations[id], 0);
            }
        }
    }
}


void Controller_RP_Handle_MoveNext(int id, uint8_t event) {
    if (event == EVENT_RP_MOVE_DONE) {
        Model_SetrpState(id, RP_WAIT_BOX);
    }
}


void Controller_RP_Handle_Done(int id, uint8_t event) {
    static uint32_t doneStartTick[NUM_RP] = {0,};
    static uint8_t waitStarted[NUM_RP] = {0,};

    if (!waitStarted[id]) {
        waitStarted[id] = 1;
        doneStartTick[id] = HAL_GetTick();

        char buf[60];
        snprintf(buf, sizeof(buf), "[RP %d] Status: FULL. Ready to return home.\r\n", id);
        Debug_Print(buf);
    }

    if ((HAL_GetTick() - doneStartTick[id]) > DONE_WAIT_TIME) {
        waitStarted[id] = 0;
        Model_SetrpState(id, RP_RETURN_HOME);

        uint32_t calibratedTime = (uint32_t)(totalMovedTime[id] * returnCalibration[id]);
        StartDCMove(id, calibratedTime, 1);
    }
}

void Controller_RP_Handle_ReturnHome(int id, uint8_t event) {
    if (event == EVENT_RP_MOVE_DONE) {
        Model_ResetrpBoxCount(id);
        totalMovedTime[id] = 0; // Reset distance tracking
        Model_SetrpState(id, RP_WAIT_BOX);

        char buf[60];
        snprintf(buf, sizeof(buf), "[RP %d] Home reached. System Restarted.\r\n", id);
        Debug_Print(buf);
    }
}

/*
void Controller_RP_UpdateToPresenter(int id) {
    rp_t *pData = (rp_t *)osPoolAlloc(poolrpData);

    if (pData != NULL) {
        pData->id = (int8_t)id;
        pData->irStatus = (int8_t)Model_GetIrStatus(id);
        pData->boxCount = (int8_t)Model_GetrpBoxCount(id);

        if (osMessagePut(rpDataMsgBox, (uint32_t)pData, 0) != osOK) {
            osPoolFree(poolrpData, pData);
        }
    }
}
*/

void Controller_RP_UpdateToPresenter(int id) {
    rp_t *pData = (rp_t *)osPoolAlloc(poolrpData);

    if (pData != NULL) {
        if (id == 0) {
            pData->id = 3;
        } else {
            pData->id = (int8_t)id;
        }

        pData->irStatus = (int8_t)Model_GetIrStatus(id);
        pData->boxCount = (int8_t)Model_GetrpBoxCount(id);

        if (osMessagePut(rpDataMsgBox, (uint32_t)pData, 0) != osOK) {
            osPoolFree(poolrpData, pData);
        }
    }
}
