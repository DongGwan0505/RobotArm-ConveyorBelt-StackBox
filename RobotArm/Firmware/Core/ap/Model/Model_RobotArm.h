/*
 * Model_Robot.h
 *
 *  Created on: Jan 8, 2026
 *      Author: 82109
 */

#ifndef AP_MODEL_MODEL_ROBOTARM_H_
#define AP_MODEL_MODEL_ROBOTARM_H_

#include <stdint.h>
#include "cmsis_os.h"

// 로봇의 동작 상태
typedef enum {
    ROBOT_STATE_IDLE,    // 대기
    ROBOT_STATE_READY,    // FPGA에서 받은 좌표 계산
    ROBOT_STATE_FINDnGRAP,    // 로봇팔 이동
    ROBOT_STATE_LIFT_UP,    // 로봇팔 집기
	ROBOT_STATE_PUT_DOWN_READY, // 로봇팔 놓기 전 준비
    ROBOT_STATE_PUT_DOWN, // 로봇팔 놓기
} RobotArmState_t;

// 외부(UART/Task)에서 발생하는 이벤트
typedef enum {
    // FPGA Auto
    EVENT_COORD_RECEIVED,
    EVENT_WORK_DONE,

    // Button Manual Control
    EVENT_BASE_CW,      EVENT_BASE_CCW,
    EVENT_SHOULDER_CW,  EVENT_SHOULDER_CCW,
    EVENT_ELBOW_CW,     EVENT_ELBOW_CCW,
    EVENT_GRIPPER_CW,   EVENT_GRIPPER_CCW,
	EVENT_STOP
} RobotArmEvent_t;

typedef struct {
    // FPGA 수신된 좌표
    uint8_t targetX0;      // 회전축 서보
    uint8_t targetX1;
    uint8_t targetY0;      // 어께 서보
    uint8_t targetY1;

    float currentZ_Height; //로봇팔 높이 제어

    // 계산된 서보모터 각도 (0 ~ 180도)
    uint8_t angleBase;    // X축 (Base)
    uint8_t angleShoulder;// Y축 (Shoulder)
    uint8_t angleElbow;   // Z축 (Elbow)
    uint8_t angleGripper; // 그리퍼 (Grab)

    // 잡기 여부
    uint8_t isGripped;    // 0: 놓음, 1: 잡음
} RobotArm_t;

extern RobotArmState_t robotState;
extern osMessageQId robotEventMsgBox;
extern osMessageQId robotDataMsgBox;
extern osMessageQId fpgaDataMsgBox;
extern osPoolId poolRobotData;

void Model_RobotInit();
void Model_SetRobotState(RobotArmState_t state);
RobotArmState_t Model_GetRobotState();

#endif /* AP_MODEL_MODEL_ROBOTARM_H_ */
