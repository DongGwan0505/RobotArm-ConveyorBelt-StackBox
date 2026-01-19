/*
 * Controller_RobotArm.h
 *
 *  Created on: Jan 9, 2026
 *      Author: 82109
 */

#ifndef AP_CONTROLLER_CONTROLLER_ROBOTARM_H_
#define AP_CONTROLLER_CONTROLLER_ROBOTARM_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "cmsis_os.h"
#include "usart.h"
#include "../Model/Model_RobotArm.h"

//로봇팔 길이
//#define LINK_1_LENGTH 8.0f //Shoulder ~ Elbow 길이(cm)
//#define LINK_2_LENGTH 14.0f  //Elbow    ~ Gripper 길이 (cm)
#define LINK_1_LENGTH 8.4f //Shoulder ~ Elbow 길이(cm)
#define LINK_2_LENGTH 14.7f  //Elbow    ~ Gripper 길이 (cm)

//수학 상수
#define RAD_TO_DEG    57.2957795f
#define PI            3.14159265f

//각도 제한 (모터 보호용)
#define BASE_MIN 0
#define BASE_MAX 180
#define SHOULDER_MIN 10
#define SHOULDER_MAX 170
#define ELBOW_MIN 90
#define ELBOW_MAX 160
#define GRIPPER_MIN 0 //잡기
#define GRIPPER_MAX 130//놓기

//로봇팔이 움직일 필드의 크기 = 카메라로 촬영되는 구역의 사이즈
#define REAL_FIELD_WIDTH_X   28.0f  // 가로 실제 길이
#define REAL_FIELD_HEIGHT_Y  21.5f  // 세로 실제 길이 21.5

#define CAMERA_OFFSET_X  -15.2f  // 로봇 기준으로 카메라 0점이 어디인지 -14.6
//#define CAMERA_OFFSET_Y  5.8f  //이전 베스트
//#define CAMERA_OFFSET_Y  3.5f  //현재 2nd (자르고 난 뒤) - 왼쪽 멀리 실패 (7)
//#define CAMERA_OFFSET_Y  3.6f    //멀리 있는게 더 멀리 있다고 판단 (5) -> 로봇이 더 가까이 있다고 생각하게 만들어야 할 듯
#define CAMERA_OFFSET_Y  3.4f    //현재 best (8) - 단 고무줄 필요할 듯

// FPGA 좌표 최대값
#define FPGA_MAX_X           320.0f
#define FPGA_MAX_Y           240.0f

//유틸리티 매크로
#define CLAMP(x, min, max)((x) < (min) ? (min) : ((x)>(max) ? (max) : (x)))


void Controller_RobotArm_Init();
void Controller_RobotArm_Excute();

void Controller_RobotArm_Manual();
void Controller_RobotArm_Auto();
void Controller_CalcIK(RobotArm_t *pRobot);

#endif /* AP_CONTROLLER_CONTROLLER_ROBOTARM_H_ */
