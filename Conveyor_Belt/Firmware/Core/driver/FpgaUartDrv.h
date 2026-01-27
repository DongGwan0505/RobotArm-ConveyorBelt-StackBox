#ifndef DRIVER_FPGAUARTDRV_H_
#define DRIVER_FPGAUARTDRV_H_

#include <stdint.h>
#include <string.h>
#include "usart.h"
#include "stm32f4xx_hal.h"


void FpgaUartDrvInit();
void FpgaUartDrvRxArm();
void FgpaUartDrvRxIsr();
uint8_t FpgaUartRxGetByte();



#endif /* DRIVER_FPGAUARTDRV_H_ */
