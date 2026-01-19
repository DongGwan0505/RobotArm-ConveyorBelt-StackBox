/*
 * Controller_RobotArm.c
 *
 *  Created on: Jan 9, 2026
 *      Author: 82109
 */

#include "Controller_RobotArm.h"

RobotArm_t robotArmData;

void Controller_RobotArm_Init() {
	//robotArmData.angleBase = 90;
	//robotArmData.angleShoulder = 90;
	//robotArmData.angleElbow = 60;
	robotArmData.angleBase = 0;
	robotArmData.angleShoulder = 100;
	robotArmData.angleElbow = 30;
	robotArmData.angleGripper = 0;
	robotArmData.isGripped = 0;
	robotArmData.currentZ_Height = -7.9;//-7.9
	//robotArmData.currentZ_Height = 10;

	//초기 데이터를 Que로 (Presenter는 초기 위치로 이동시킴)
	RobotArm_t *pData = osPoolAlloc(poolRobotData);
	if (pData != NULL) {
		memcpy(pData, &robotArmData, sizeof(RobotArm_t));
		osMessagePut(robotDataMsgBox, (uint32_t)pData, 0);
	}
}

void Controller_RobotArm_Excute() {
	Controller_RobotArm_Manual();

}

static int currentAction = EVENT_STOP;

//버튼 제어
void Controller_RobotArm_Manual() {

	osEvent evt = osMessageGet(robotEventMsgBox, 0);

	if (evt.status == osEventMessage) {
		currentAction = (RobotArmEvent_t)evt.value.v;
	}

	int isChanged = 0;

	if (currentAction != EVENT_STOP) {

		switch (currentAction) {
		// --- Base ---
		case EVENT_BASE_CW:
			if (robotArmData.angleBase < 180) {
				robotArmData.angleBase++;
				isChanged = 1;
			}
			break;
		case EVENT_BASE_CCW:
			if (robotArmData.angleBase > 0) {
				robotArmData.angleBase--;
				isChanged = 1;
			}
			break;

			// --- Shoulder ---
		case EVENT_SHOULDER_CW:
			if (robotArmData.angleShoulder < 180) {
				robotArmData.angleShoulder++;
				isChanged = 1;
			}
			break;
		case EVENT_SHOULDER_CCW:
			if (robotArmData.angleShoulder > 0) {
				robotArmData.angleShoulder--;
				isChanged = 1;
			}
			break;

			// --- Elbow ---
		case EVENT_ELBOW_CW:
			if (robotArmData.angleElbow < 180) {
				robotArmData.angleElbow++;
				isChanged = 1;
			}
			break;
		case EVENT_ELBOW_CCW:
			if (robotArmData.angleElbow > 0) {
				robotArmData.angleElbow--;
				isChanged = 1;
			}
			break;

			// --- Gripper ---
		case EVENT_GRIPPER_CW:
			if (robotArmData.angleGripper < 180) {
				robotArmData.angleGripper++;
				robotArmData.isGripped = 1;
				isChanged = 1;
			}
			break;
		case EVENT_GRIPPER_CCW:
			if (robotArmData.angleGripper > 0) {
				robotArmData.angleGripper--;
				robotArmData.isGripped = 0;
				isChanged = 1;
			}
			break;
		}
	}

	if (isChanged) {
		RobotArm_t *pData = osPoolAlloc(poolRobotData);
		if (pData != NULL) {
			memcpy(pData, &robotArmData, sizeof(RobotArm_t));
			osStatus stat = osMessagePut(robotDataMsgBox, (uint32_t)pData, 0);
			if (stat != osOK) {
				osPoolFree(poolRobotData, pData);
			}
		}
		osDelay(5);
	}
}

//FPGA 좌표로 동작
void Controller_RobotArm_Auto() {
	osEvent evt = osMessageGet(robotEventMsgBox, 10); //이벤트 확인 (Listener_UART가 보낸 신호 대기-10ms)

	if (evt.status == osEventMessage){ //이벤트 확인
		if(evt.value.v == EVENT_COORD_RECEIVED){ //좌표 수신 이벤트인지
			osEvent dataEvt = osMessageGet(fpgaDataMsgBox, 0); //데이터 큐에서 좌표 패키지 꺼내기

			if (dataEvt.status == osEventMessage){
				RobotArm_t *pReceivedData = (RobotArm_t *)dataEvt.value.p; //포인터 형변환

				pReceivedData->currentZ_Height = robotArmData.currentZ_Height;
				pReceivedData->isGripped = 0;

				//X,Y좌표(FPGA) -> X`, Y`, Z` 변환(로봇팔)하는 로직
				Controller_CalcIK(pReceivedData);
				//로직 끝

				//계산결과 반영
				robotArmData = *pReceivedData;

				osPoolFree(poolRobotData, pReceivedData);

				RobotArm_t *pNewState = osPoolAlloc(poolRobotData);
				if(pNewState != NULL){
					memcpy(pNewState, &robotArmData, sizeof(RobotArm_t));
					osStatus stat = osMessagePut(robotDataMsgBox, (uint32_t)pNewState, 0);
					if (stat != osOK) {
						osPoolFree(poolRobotData, pNewState);
					}
				}

			}
		}

	}
}

void Controller_CalcIK(RobotArm_t *pRobot)
{
	if (pRobot == NULL) return;

	float raw_x = (float)(((uint16_t)pRobot->targetX1 << 8) | (uint16_t)pRobot->targetX0);
	float raw_y = (float)(((uint16_t)pRobot->targetY1 << 8) | (uint16_t)pRobot->targetY0);


	// cm 단위로 변환
	// 화면상의 상대적 위치 (예시0cm ~ 30cm)
	// [수정된 좌표계 반영]
	    // 기준: (0,0)이 우측 하단.
	    // X: 0(우) -> 320(좌) : 값이 커질수록 왼쪽으로 감
	    // Y: 0(하) -> 240(상) : 값이 커질수록 멀어짐
	float rel_x_cm = (1.0f - (raw_x / FPGA_MAX_X)) * REAL_FIELD_WIDTH_X;
	float rel_y_cm = (raw_y / FPGA_MAX_Y) * REAL_FIELD_HEIGHT_Y;

	//로봇 절대 좌표 변환 (로봇팔의 0,0) 기준의 실제 좌표 (예시 로봇 팔 화면 아래 중앙, 카메라 Y축은 위->아래로 증가하는 경우)
	float x = rel_x_cm + CAMERA_OFFSET_X;
	float y = rel_y_cm + CAMERA_OFFSET_Y;

	float z = (float)pRobot->currentZ_Height;
	// [디버깅] 여기서 x, y가 제대로 들어왔는지 출력
	//printf("IK Input: x=%.2f, y=%.2f\n", x, y)

	float z_offset = z; //높이 보정용 (만약을 대비)

	//Base 각도 (X-Y 평면 회전)
	float thetaBase = atan2f(y, x) * RAD_TO_DEG;
	if (thetaBase < 0) thetaBase += 180.0f; //0~180 범위로 보정

	//Shoulder & Elbow
	float r = sqrtf(x*x + y*y); //원점 to 수평
	float d_sq = r*r + z*z;     //목표까지 직선거리 제공
	float d = sqrtf(d_sq);

	// 물리적 한계 제한 (팔 길이보다 멀면 닿을 수 있는 최대치로 제한)
	if (d > (LINK_1_LENGTH + LINK_2_LENGTH)) {
		d = LINK_1_LENGTH + LINK_2_LENGTH;
		d_sq = d * d;
	}

	// 제2 코사인 법칙
	float cos_alpha = (LINK_1_LENGTH*LINK_1_LENGTH + d_sq - LINK_2_LENGTH*LINK_2_LENGTH) / (2.0f * LINK_1_LENGTH * d);
	float cos_beta  = (LINK_1_LENGTH*LINK_1_LENGTH + LINK_2_LENGTH*LINK_2_LENGTH - d_sq) / (2.0f * LINK_1_LENGTH * LINK_2_LENGTH);

	cos_alpha = CLAMP(cos_alpha, -1.0f, 1.0f);
	cos_beta  = CLAMP(cos_beta,  -1.0f, 1.0f);

	float alpha = acosf(cos_alpha);
	float beta  = acosf(cos_beta);

	// 각도 계산 (RAD -> DEG)
	// atan2f(z, r)은 목표 높이에 따른 어깨 기본 각도
	float thetaShoulder = (atan2f(z_offset, r) + alpha) * RAD_TO_DEG;
	float thetaElbow    = beta * RAD_TO_DEG;

	float reversedShoulder = 180.0f - thetaShoulder;
//	float reversedElbow    = 180.0f - thetaElbow;
//	float finalShoulder = 90.0f + thetaShoulder;

	// 결과 반영 (float -> uint8_t)
	pRobot->angleBase     = (uint8_t)CLAMP(thetaBase, BASE_MIN, BASE_MAX);
	//pRobot->angleShoulder = (uint8_t)CLAMP(thetaShoulder, SHOULDER_MIN, SHOULDER_MAX);
	pRobot->angleShoulder = (uint8_t)CLAMP(reversedShoulder, SHOULDER_MIN, SHOULDER_MAX);
	//pRobot->angleShoulder = (uint8_t)CLAMP(reversedShoulder, SHOULDER_MIN, SHOULDER_MAX);
	pRobot->angleElbow    = (uint8_t)CLAMP(thetaElbow, ELBOW_MIN, ELBOW_MAX);
//	pRobot->angleElbow    = (uint8_t)CLAMP(reversedElbow, ELBOW_MIN, ELBOW_MAX);

	// Gripper 처리
	if (pRobot->isGripped) {
		pRobot->angleGripper = GRIPPER_MIN;
	} else {
		pRobot->angleGripper = GRIPPER_MAX;
	}

	printf("Pos(%d, %d, %d) -> B:%02d S:%02d E:%02d <-> r = %02d\r\n",
				(int)x, (int)y, (int)z,
				pRobot->angleBase, pRobot->angleShoulder, pRobot->angleElbow, (int)r);
}
