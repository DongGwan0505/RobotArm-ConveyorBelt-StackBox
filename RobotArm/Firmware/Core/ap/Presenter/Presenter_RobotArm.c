/*
 * Presenter_RobotArm.c
 *
 * Created on: Jan 9, 2026
 * Author: 82109
 */

#include "Presenter_RobotArm.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

ServoMotor_t servoBase;
ServoMotor_t servoShoulder;
ServoMotor_t servoElbow;
ServoMotor_t servoGripper;
RobotArmState_t Robot_State;

static uint8_t s_entry;
static uint32_t s_t0;

static uint8_t s_idle_tick_sent;
static uint32_t s_idle_tick_t0;

static RobotArm_t s_pickCmd;
static uint8_t s_has_pickCmd;

static uint8_t s_sub;

static uint8_t s_tick_active;
static uint32_t s_tick_end_ms;

static void FSM_Goto(RobotArmState_t next);
static void FSM_Step_Auto(uint8_t has_new_cmd, const RobotArm_t *cmd);
static void Manual_ApplyLatest(uint8_t has_new_cmd, const RobotArm_t *cmd);

static uint8_t DrainRobotArm(RobotArm_t *out_last);
static void MotorService_10ms(void);

static uint8_t Servo_IsAtTarget(const ServoMotor_t *s, float tol_deg);
static uint8_t Arm_AllAtTarget(float tol_deg);

static void FpgaTick_Start(void);
static void FpgaTick_Service(void);

void Presenter_RobotArm_Init() {
	//[UART] 부팅 메시지
	char *bootMsg = "\r\n\r\n[SYSTEM] Robot Arm Control Started.\r\n";
	HAL_UART_Transmit(&huart2, (uint8_t*) bootMsg, strlen(bootMsg), 100);

	//모터 초기화
	Servo_Init(&servoBase, &htim3, TIM_CHANNEL_1);
	Servo_Init(&servoShoulder, &htim3, TIM_CHANNEL_3);
	Servo_Init(&servoElbow, &htim3, TIM_CHANNEL_2);
	Servo_Init(&servoGripper, &htim3, TIM_CHANNEL_4);

	//초기 각도 설정
	//Set_Servo_Angle(&servoBase, 90);
	//Set_Servo_Angle(&servoShoulder, 90);
	//Set_Servo_Angle(&servoElbow, 60);
	//Set_Servo_Angle(&servoGripper, 0);
	Set_Servo_Angle(&servoBase, 0);
//	Set_Servo_Angle_Legacy(&servoShoulder, 170);
	Set_Servo_Angle(&servoShoulder, 90);
	Set_Servo_Angle(&servoElbow, 120);
	Set_Servo_Angle(&servoGripper, 0);

	// 초당 이동 각도의 최댓값
	//Servo_SetSlewRate(&servoBase, 140.0f);
	//Servo_SetSlewRate(&servoShoulder, 90.0f);
	//Servo_SetSlewRate(&servoElbow, 110.0f);
	//Servo_SetSlewRate(&servoGripper, 220.0f);
	Servo_SetSlewRate(&servoBase, 140.0f);
	Servo_SetSlewRate(&servoShoulder, 200.0f);
	Servo_SetSlewRate(&servoElbow, 180.0f);
	Servo_SetSlewRate(&servoGripper, 220.0f);
	Robot_State = ROBOT_STATE_IDLE;
	s_entry = 1;
	s_t0 = HAL_GetTick();

	s_idle_tick_sent = 0;
	s_idle_tick_t0 = 0;

	s_has_pickCmd = 0;
	s_sub = 0;

	//	HAL_GPIO_WritePin(FPGA_TICK_GPIO_Port, FPGA_TICK_Pin, GPIO_PIN_RESET);
	//	s_tick_active = 0;
	//	s_tick_end_ms = 0;
}

void Presenter_RobotArm_Excute() {
#define DEBUG_HEARTBEAT  0

#if DEBUG_HEARTBEAT
	static uint32_t lastUpdateTick = 0;
	if (HAL_GetTick() - lastUpdateTick > 200) {
		lastUpdateTick = HAL_GetTick();
		Presenter_RobotArm_DispMode();
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
#endif

	RobotArm_t last;
	uint8_t has = DrainRobotArm(&last);

	FSM_Step_Auto(has, has ? &last : NULL);
	FpgaTick_Service();
	MotorService_10ms();

	// Legacy Code 260113
	//	static uint32_t lastUpdateTick = 0;
	//
	//	if (HAL_GetTick() - lastUpdateTick > 200)
	//	{
	//		lastUpdateTick = HAL_GetTick();
	//
	//		// 모드 변경 확인 및 UART 출력
	//		Presenter_RobotArm_DispMode();
	//
	//		// 정상 동작 확인용 LED
	//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	//	}
	//	RobotArm_t last;
	//	uint8_t has = 0;
	//
	//	for (;;)
	//	{
	//		osEvent evt = osMessageGet(robotDataMsgBox, 0);
	//		if(evt.status != osEventMessage) break;
	//
	//		RobotArm_t* pRobotData = (RobotArm_t*)evt.value.p;
	//		if(pRobotData)
	//		{
	//			last = *pRobotData;
	//			has = 1;
	//			osPoolFree(poolRobotData, pRobotData);
	//		}
	//	}
	//	if(has)
	//	{
	//		servoBase.target_angle = last.angleBase;
	//		servoShoulder.target_angle = last.angleShoulder;
	//		servoElbow.target_angle = last.angleElbow;
	//		servoGripper.target_angle = last.angleGripper;
	//	}
	//
	//
	//	// 초당 이동 속도 -> 엄밀히 말하면 update 주기-> slew 속도 만큼 움직이고 그 속도를 update마다 적용.
	//	static uint32_t div = 0;
	//	div ++;
	//	if(div >= 10)
	//	{
	//		div = 0;
	//		const float dt = 0.01f;
	//		Servo_Update(&servoBase, dt);
	//		Servo_Update(&servoShoulder, dt);
	//		Servo_Update(&servoElbow, dt);
	//		Servo_Update(&servoGripper, dt);
	//	}
	//
	//
	//
	// Legacy Code 20260112
	//	// 시간 측정을 위한 정적 변수
	//	static uint32_t lastUpdateTick = 0;
	//
	//	if (HAL_GetTick() - lastUpdateTick > 200)
	//	{
	//		lastUpdateTick = HAL_GetTick();
	//
	//		// 모드 변경 확인 및 UART 출력
	//		Presenter_RobotArm_DispMode();
	//
	//		// 정상 동작 확인용 LED
	//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	//	}
	//
	//	RobotArm_t* pRobotData;
	//	osEvent evt;
	//
	//	evt = osMessageGet(robotDataMsgBox, 0);
	//
	//	if(evt.status == osEventMessage){
	//		pRobotData = (RobotArm_t*)evt.value.p;
	//
	////		char debugMsg[128];
	//
	////		sprintf(debugMsg, "[IK] Pos(%d, %d, %d) -> Ang(%d, %d, %d, %d)\r\n",
	////				pRobotData->targetX0,     pRobotData->targetY0,       pRobotData->currentZ_Height, // 입력값
	////				pRobotData->angleBase,   pRobotData->angleShoulder, pRobotData->angleElbow, pRobotData->angleGripper // 결과값
	////		);
	//
	////		HAL_UART_Transmit(&huart2, (uint8_t*)debugMsg, strlen(debugMsg), 10);
	//
	//		Set_Servo_Angle(&servoBase,     pRobotData->angleBase);
	//		Set_Servo_Angle(&servoShoulder, pRobotData->angleShoulder);
	//		Set_Servo_Angle(&servoElbow,    pRobotData->angleElbow);
	//		Set_Servo_Angle(&servoGripper,  pRobotData->angleGripper);
	//
	//		osPoolFree(poolRobotData, pRobotData);
}

static uint8_t DrainRobotArm(RobotArm_t *out_last) {
	uint8_t has = 0;
	for (;;) {
		osEvent evt = osMessageGet(robotDataMsgBox, 0);
		if (evt.status != osEventMessage)
			break;

		RobotArm_t *p = (RobotArm_t*) evt.value.p;
		if (p) {
			if (out_last)
				*out_last = *p;
			has = 1;
			osPoolFree(poolRobotData, p);
		}
	}
	return has;
}

//static void MotorService_10ms() {
//	static uint32_t div = 0;
//	div++;
//
//	if (div >= SERVO_UPDATE_PERIOD_MS) {
//		div = 0;
//		const float dt = SERVO_UPDATE_DT_S;
//
//		Servo_Update(&servoBase, dt);
//		Servo_Update(&servoShoulder, dt);
//		Servo_Update(&servoElbow, dt);
//		Servo_Update(&servoGripper, dt);
//	}
//
//}
static void MotorService_10ms() {
	static uint32_t lastServoTick = 0;

	// 정확히 10ms마다 실행되도록 보장
	if (HAL_GetTick() - lastServoTick >= SERVO_UPDATE_PERIOD_MS) {
		lastServoTick = HAL_GetTick();

		const float dt = SERVO_UPDATE_DT_S;

		Servo_Update(&servoBase, dt);
		Servo_Update(&servoShoulder, dt);
		Servo_Update(&servoElbow, dt);
		Servo_Update(&servoGripper, dt);
	}
}

static uint8_t Servo_IsAtTarget(const ServoMotor_t *s, float tol_deg) {
	float err = (float) s->target_angle - s->cmd_angle_f;
	if (err < 0.0f)
		err = -err;
	return (err <= tol_deg);
}

static uint8_t Arm_AllAtTarget(float tol_deg) {
	return Servo_IsAtTarget(&servoBase, tol_deg)
			&& Servo_IsAtTarget(&servoShoulder, tol_deg)
			&& Servo_IsAtTarget(&servoElbow, tol_deg)
			&& Servo_IsAtTarget(&servoGripper, tol_deg);
}

static void Manual_ApplyLatest(uint8_t has_new_cmd, const RobotArm_t *cmd) {
	if (!has_new_cmd || !cmd)
		return;

	servoBase.target_angle = cmd->angleBase;
	servoShoulder.target_angle = cmd->angleShoulder;
	servoElbow.target_angle = cmd->angleElbow;
	servoGripper.target_angle = cmd->angleGripper;
}

static void FSM_Goto(RobotArmState_t next) {
	Robot_State = next;
	s_entry = 1;
	s_t0 = HAL_GetTick();
	s_sub = 0;
}

static void FSM_Step_Auto(uint8_t has_new_cmd, const RobotArm_t *cmd) {
	if (has_new_cmd && cmd) {
		s_pickCmd = *cmd;
		s_has_pickCmd = 1;
	}
	if ((HAL_GetTick() - s_t0) > STATE_TIMEOUT_MS) {
		FSM_Goto(ROBOT_STATE_IDLE);
	}
	switch (Robot_State) {
	case ROBOT_STATE_IDLE:
		if (s_entry) {
			s_entry = 0;
			s_has_pickCmd = 0;
			s_idle_tick_sent = 0;
			s_idle_tick_t0 = HAL_GetTick();

			FpgaTick_Start();
			s_idle_tick_sent = 1;
		}
		if (s_has_pickCmd) {
			FSM_Goto(ROBOT_STATE_READY);
		}
		// if State Stuck? We must Send Tick per 0.5Sec -> Need To Test
		//		} else {
		//			if (s_idle_tick_sent
		//					&& (HAL_GetTick() - s_idle_tick_t0) > IDLE_WAIT_TIMEOUT_MS) {
		//				s_idle_tick_t0 = HAL_GetTick();
		//				FpgaTick_Start();
		//			}
		//		}
		break;
	case ROBOT_STATE_READY:
		if (s_entry) {
//			Servo_Init(&servoBase, &htim3, TIM_CHANNEL_1);
//			Servo_Init(&servoShoulder, &htim3, TIM_CHANNEL_3);
//			Servo_Init(&servoElbow, &htim3, TIM_CHANNEL_2);
//			Servo_Init(&servoGripper, &htim3, TIM_CHANNEL_4);

			s_entry = 0;
			servoBase.target_angle = READY_BASE;
			servoShoulder.target_angle = READY_SHOULDER;
			servoElbow.target_angle = READY_ELBOW;
			servoGripper.target_angle = GRIP_OPEN;
		}
		if (Arm_AllAtTarget(AT_TARGET_TOL_DEG)) {
			FSM_Goto(ROBOT_STATE_FINDnGRAP);
		}
		break;
	case ROBOT_STATE_FINDnGRAP:
		if (s_entry) {
			s_entry = 0;
			s_sub = 0;
		}
		if (s_sub == 0) {
			if (s_has_pickCmd) {
				servoBase.target_angle = s_pickCmd.angleBase;
				servoShoulder.target_angle = s_pickCmd.angleShoulder;
				servoElbow.target_angle = s_pickCmd.angleElbow;
				servoGripper.target_angle = GRIP_OPEN;
			}
			if (Servo_IsAtTarget(&servoBase, AT_TARGET_TOL_DEG)
					&& Servo_IsAtTarget(&servoShoulder, AT_TARGET_TOL_DEG)
					&& Servo_IsAtTarget(&servoElbow, AT_TARGET_TOL_DEG)) {
				servoGripper.target_angle = GRIP_CLOSE;
				s_sub = 1;
			}
		} else {
			if (Servo_IsAtTarget(&servoGripper, AT_TARGET_TOL_DEG)) {
				FSM_Goto(ROBOT_STATE_LIFT_UP);
			}
		}
		break;
	case ROBOT_STATE_LIFT_UP:
		if (s_entry) {
			s_entry = 0;
			s_sub = 0;
//			servoBase.target_angle = LIFT_BASE;
			servoShoulder.target_angle = LIFT_SHOULDER;
			servoElbow.target_angle = LIFT_ELBOW;
			servoGripper.target_angle = GRIP_CLOSE;
		}
		if (s_sub == 0) {
			if (Servo_IsAtTarget(&servoBase, AT_TARGET_TOL_DEG)
					&& Servo_IsAtTarget(&servoShoulder, AT_TARGET_TOL_DEG)
					&& Servo_IsAtTarget(&servoElbow, AT_TARGET_TOL_DEG)) {
				servoGripper.target_angle = GRIP_CLOSE;
				s_sub = 1;
			}
		} else {
			if (Servo_IsAtTarget(&servoGripper, AT_TARGET_TOL_DEG)) {
				FSM_Goto(ROBOT_STATE_PUT_DOWN_READY);
			}
		}
		break;
	case ROBOT_STATE_PUT_DOWN_READY:
		if (s_entry) {
					s_entry = 0;
					s_sub = 0;
					servoBase.target_angle = PUT_READY_BASE;
					servoGripper.target_angle = GRIP_CLOSE;
				}
				if (s_sub == 0) {
					if (Servo_IsAtTarget(&servoBase, AT_TARGET_TOL_DEG)) {
						servoGripper.target_angle = GRIP_CLOSE;
						s_sub = 1;
					}
				} else {
					if (Servo_IsAtTarget(&servoGripper, AT_TARGET_TOL_DEG)) {
						FSM_Goto(ROBOT_STATE_PUT_DOWN);
					}
				}
		break;
	case ROBOT_STATE_PUT_DOWN:
		if (s_entry) {
			s_entry = 0;
			s_sub = 0;

			servoBase.target_angle = PUT_BASE;
			servoShoulder.target_angle = PUT_SHOULDER;
			servoElbow.target_angle = PUT_ELBOW;
			servoGripper.target_angle = GRIP_CLOSE;
		}
		if (s_sub == 0) {
			if (Servo_IsAtTarget(&servoBase, AT_TARGET_TOL_DEG)
					&& Servo_IsAtTarget(&servoShoulder, AT_TARGET_TOL_DEG)
					&& Servo_IsAtTarget(&servoElbow, AT_TARGET_TOL_DEG)) {
				servoGripper.target_angle = GRIP_OPEN;
				s_sub = 1;
			}
		}
		else
		{
			if(Servo_IsAtTarget(&servoGripper, AT_TARGET_TOL_DEG))
			{
				FSM_Goto(ROBOT_STATE_IDLE);
			}
		}
		break;
	default:
		FSM_Goto(ROBOT_STATE_IDLE);
		break;
	}

}
static void FpgaTick_Start() {
	if (s_tick_active)
		return;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
	s_tick_end_ms = HAL_GetTick() + FPGA_TICK_PULSE;
	s_tick_active = 1;
}

static void FpgaTick_Service() {
	if (!s_tick_active)
		return;
	if ((int32_t) (HAL_GetTick() - s_tick_end_ms) >= 0) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
		s_tick_active = 0;
	}
}

void Presenter_RobotArm_DispMode() {
	static modeState_t prevModeState = 255; // 초기값은 없는 모드로 설정
	modeState_t modeState = Model_GetMode();

	// [중요] 모드가 바뀌었을 때만 메시지를 보냅니다. (도배 방지)
	if (prevModeState == modeState)
		return;
	prevModeState = modeState;

	char msg[64]; // 메시지 버퍼

	// UART 출력 내용 작성
	switch (modeState) {
	case MODE_STANBY:
		sprintf(msg, ">> [MODE CHANGED] : STANDBY\r\n");
		break;
	case MODE_MANUAL:
		sprintf(msg, ">> [MODE CHANGED] : MANUAL CONTROL\r\n");
		break;
	case MODE_AUTO:
		sprintf(msg, ">> [MODE CHANGED] : AUTO MODE (FPGA)\r\n");
		break;
	default:
		return;
	}

	// PC로 전송 (Nucleo 보드는 USB선으로 UART2가 연결됨)
	HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), 100);
}
