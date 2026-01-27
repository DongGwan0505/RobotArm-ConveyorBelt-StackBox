#ifndef AP_PRESENTER_PRESENTERMOTOR_H_
#define AP_PRESENTER_PRESENTERMOTOR_H_

#include <stdint.h>
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "../Model/Model_Motor.h"
#include "../driver/servo_motor.h"

#define MOTOR_COUNT 2
#define MOTOR_ID_ALL 0xff

void PresenterMotorInit();
void PresenterMotorExcute();

#endif /* AP_PRESENTER_PRESENTERMOTOR_H_ */
