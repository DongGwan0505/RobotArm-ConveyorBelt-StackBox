#include "ControllerFpgaUart.h"

static int ControllerFpgaUartCompare(uint8_t rx, MotorData_t *out1, MotorData_t *out2);

void ControllerFpgaUartInit() {

}
void ControllerFpgaExcute() {
	static uint8_t last_rx = 0xFF;
	uint8_t rx;
	if (!Model_UartPopByte(&rx, osWaitForever))
		return;
	MotorData_t cmd1, cmd2;
	if (rx == last_rx)
			return;
		last_rx = rx;
	if (!ControllerFpgaUartCompare(rx, &cmd1, &cmd2)) return;
	MotorData_t arr[2] = {cmd1, cmd2};
	(void) ModelSetBachMotor(arr, 2);

}

static int ControllerFpgaUartCompare(uint8_t rx, MotorData_t *out1, MotorData_t *out2) {
	if (!out1 && !out2)
		return 0;
//	out->motor_id = MOTOR_ID_ALL;
	switch (rx) {
	case SQUARE:
		out1->motor_id = MOTOR_ID_LEFT;
		out2->motor_id = MOTOR_ID_RIGHT;
//		out1->angle = 35;
//		out2->angle = 90;
		out1->angle = 40;
		out2->angle = 40;
		return 1;
	case CIRECLE:
		out1->motor_id = MOTOR_ID_LEFT;
		out2->motor_id = MOTOR_ID_RIGHT;
//		out1->angle = 90;
//		out2->angle = 125;
		out1->angle = 90;
		out2->angle = 90;
		return 1;
	case TRIANGEL:
		out1->motor_id = MOTOR_ID_LEFT;
		out2->motor_id = MOTOR_ID_RIGHT;
//		out1->angle = 90;
//		out2->angle = 90;
		out1->angle = 130;
		out2->angle = 130;
		return 1;

	default:
		return 0;
	}
}

