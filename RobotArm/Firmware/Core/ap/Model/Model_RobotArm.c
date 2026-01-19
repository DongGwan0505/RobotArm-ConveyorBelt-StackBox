/*
 * Model_Robot.c
 *
 *  Created on: Jan 8, 2026
 *      Author: 82109
 */

#include "Model_RobotArm.h"

RobotArmState_t robotState = ROBOT_STATE_IDLE;

// RTOS에서 제공하는 Queue
osMessageQId robotEventMsgBox;
osMessageQDef(robotEventQue, 8, RobotArmEvent_t);

osMessageQId robotDataMsgBox;
osMessageQDef(robotDataQue, 16, RobotArm_t);

osMessageQId fpgaDataMsgBox;
osMessageQDef(fpgaDataQue, 16, RobotArm_t);

// RTOS에서 제공하는 동적메모리 할당
osPoolDef(poolRobotEvent, 16, RobotArmEvent_t);
osPoolId poolRobotEvent;
osPoolDef(poolRobotData, 8, RobotArm_t);
osPoolId poolRobotData;

void Model_RobotInit() {
    // 메모리 pool 생성
    poolRobotEvent = osPoolCreate(osPool(poolRobotEvent));
    poolRobotData = osPoolCreate(osPool(poolRobotData));

    // 메시지 que 생성
    robotEventMsgBox = osMessageCreate(osMessageQ(robotEventQue), NULL);
    robotDataMsgBox = osMessageCreate(osMessageQ(robotDataQue), NULL);
    fpgaDataMsgBox = osMessageCreate(osMessageQ(fpgaDataQue), NULL);

}

void Model_SetRobotState(RobotArmState_t state) {
    robotState = state;
}

RobotArmState_t Model_GetRobotState() {
    return robotState;
}

