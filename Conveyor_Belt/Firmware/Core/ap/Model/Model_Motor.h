#ifndef AP_MODEL_MODEL_MOTOR_H_
#define AP_MODEL_MODEL_MOTOR_H_

#include <stdint.h>
#include "cmsis_os.h"

#define MOTOR_MAX 2
#define MOTOR_MSG_DEPTH 1

typedef struct{
	uint8_t motor_id;
	uint8_t angle;
}MotorData_t;


typedef struct{
	uint8_t count;
	MotorData_t cmd[MOTOR_MAX];
}MotorBatch_t;

void ModelMotorInit();
osStatus ModelSetBachMotor(MotorData_t *cmds, uint8_t count);
int ModelGetBatchMotor(MotorBatch_t *out, uint32_t timeout);
osStatus ModelSetResult(MotorData_t *cmd);
int ModelGetResult(MotorData_t *out, uint32_t timeout);
int ModelGetResult(MotorData_t *out, uint32_t timeout);

#endif /* AP_MODEL_MODEL_MOTOR_H_ */
