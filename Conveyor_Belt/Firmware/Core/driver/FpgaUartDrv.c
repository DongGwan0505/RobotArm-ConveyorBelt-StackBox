#include "FpgaUartDrv.h"

static uint8_t rx_arm_data;
static volatile uint8_t last_byte;
static volatile uint8_t has_new;


void FpgaUartDrvInit(void)
{
    rx_arm_data = 0;
    last_byte   = 0;
    has_new     = 0;
}

void FpgaUartDrvRxArm()
{
	HAL_UART_Receive_IT(&huart2, &rx_arm_data, 1);
}

void FpgaUartDrvRxIsr()
{
	last_byte = rx_arm_data;
	has_new = 1;
	FpgaUartDrvRxArm();
}

uint8_t FpgaUartRxGetByte(uint8_t *rx_data)
{
	if(has_new){
		if (last_byte == 0x0D || last_byte == 0x0A){
			has_new = 0;
			return 0;
		}

		*rx_data = last_byte;
		has_new = 0;
		return 1;
	}else {
		return 0;
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
    	FpgaUartDrvRxIsr();
    }
}
