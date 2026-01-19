/* Listener_UART.c */

#include "Listener_UART.h"

void Listener_Uart_Init()
{
    FpgaUartDrvInit();
}

void Listener_Uart_Execute()
{
    if (FpgaUartDrvGetAvailableBytes() >= 4)
    {
        uint8_t rxBuf[4];
        uint16_t len;

        len = FpgaUartDrvReadBytes(rxBuf, 4);

        if (len == 4) {
            RobotArm_t *pData = osPoolAlloc(poolRobotData);

            if(pData != NULL)
            {
                pData->targetX0 = rxBuf[0];
                pData->targetX1 = rxBuf[1];
                pData->targetY0 = rxBuf[2];
                pData->targetY1 = rxBuf[3];

                osStatus status = osMessagePut(fpgaDataMsgBox, (uint32_t)pData, 0);

                if (status != osOK) {
                    osPoolFree(poolRobotData, pData);
                    //printf("Queue Full! Memory Freed.\r\n");
                }
                else {
                    osMessagePut(robotEventMsgBox, EVENT_COORD_RECEIVED, 0);
                    printf("Recv: x0=%02x, x1=%02x, y0=%02x, y1=%02x\r\n",
                           rxBuf[0], rxBuf[1], rxBuf[2], rxBuf[3]);
                }
            }
        }
    }
}
