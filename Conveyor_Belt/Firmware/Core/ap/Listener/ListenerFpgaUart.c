#include "ListenerFpgaUart.h"

uint8_t rx_data;

void ListenerFpgaUartInit() {
	FpgaUartDrvInit();
	FpgaUartDrvRxArm();
}


void ListenerFpgaUartExcute() {
	while (FpgaUartRxGetByte(&rx_data)) {
		printf("%c", rx_data);
		(void) Model_UartPushByte(rx_data);
	}
}

