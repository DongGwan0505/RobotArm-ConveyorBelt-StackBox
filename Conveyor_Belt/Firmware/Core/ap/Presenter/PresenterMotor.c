#include "PresenterMotor.h"

static ServoMotor_t s_servo[MOTOR_COUNT];
//static ServoMotor_t* MotorMap(uint8_t motor_id);
//
//static ServoMotor_t* MotorMap(uint8_t motor_id) {
//	if (motor_id >= MOTOR_COUNT)
//		return NULL;
//	return &s_servo[motor_id];
//}

void PresenterMotorInit() {
	Servo_Init(&s_servo[0], &htim3, TIM_CHANNEL_3);
	Servo_Init(&s_servo[1], &htim3, TIM_CHANNEL_4);
	Set_Servo_Angle(&s_servo[0],90);
	Set_Servo_Angle(&s_servo[0],90);
}

void PresenterMotorExcute() {

// This Code is For Act in Same Angle
//	MotorData_t cmd;
//	MotorData_t last;
//	int has = 0;
//
//	if (!ModelGetResult(&cmd, osWaitForever))
//		return;
//	last = cmd;
//	has = 1;
//	while (ModelGetResult(&cmd, 0)) {
//		last = cmd;
//	}
//	if (!has)
//		return;
//
//	if (last.motor_id == MOTOR_ID_ALL) {
//		ServoMotor_t *s0 = MotorMap(0);
//		ServoMotor_t *s1 = MotorMap(1);
//		if (s0)
//			Set_Servo_Angle(s0, cmd.angle);
//
//		if (s1)
//			Set_Servo_Angle(s1, cmd.angle);
//		return;
//	}
//	ServoMotor_t *servo = MotorMap(cmd.motor_id);
//	if (!servo)
//		return;
//	Set_Servo_Angle(servo, cmd.angle);

// // This Code is For Act in Same Differnet Angle
	MotorBatch_t act;
	if(!ModelGetBatchMotor(&act, osWaitForever)) return;

	for(uint8_t i = 0; i<act.count; i++){
		uint8_t id = act.cmd[i].motor_id;
		if(id< MOTOR_COUNT){
//			Set_Servo_Angle(&s_servo[id], 90);
			Set_Servo_Angle(&s_servo[id], act.cmd[i].angle);
		}
	}


}
