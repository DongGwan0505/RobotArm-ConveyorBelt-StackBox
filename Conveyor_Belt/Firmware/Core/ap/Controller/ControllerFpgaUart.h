#ifndef AP_CONTROLLER_CONTROLLERFPGAUART_H_
#define AP_CONTROLLER_CONTROLLERFPGAUART_H_

#include <stdint.h>
#include "cmsis_os.h"
#include "../Model/Model_Uart.h"
#include "../Model/Model_Motor.h"

#define SQUARE 0x01
#define CIRECLE 0x02
#define TRIANGEL 0x04
#define MOTOR_ID_ALL 0xff

#define MOTOR_ID_LEFT 0;
#define MOTOR_ID_RIGHT 1;

void ControllerFpgaUartInit();
void ControllerFpgaExcute();


#endif /* AP_CONTROLLER_CONTROLLERFPGAUART_H_ */
