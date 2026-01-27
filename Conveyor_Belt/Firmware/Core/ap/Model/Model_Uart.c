#include "Model_Uart.h"

osMessageQId uartEvtMsgBox;
osMessageQDef(uartEvtQue, 16, void*);

osPoolDef(poolUartEvt, 16, uart_evt_t);
osPoolId poolUartEvt;

void Model_UartInit(void)
{
  poolUartEvt = osPoolCreate(osPool(poolUartEvt));
  uartEvtMsgBox = osMessageCreate(osMessageQ(uartEvtQue), NULL);
}

int Model_UartPushByte(uint8_t b)
{
  uart_evt_t *e = osPoolAlloc(poolUartEvt);
  if (!e) return 0;

  e->byte = b;

  if (osMessagePut(uartEvtMsgBox, (uint32_t)e, 0) != osOK) {
    osPoolFree(poolUartEvt, e);
    return 0;
  }
  return 1;
}

int Model_UartPopByte(uint8_t *out, uint32_t timeout_ms)
{
  osEvent evt = osMessageGet(uartEvtMsgBox, timeout_ms);
  if (evt.status != osEventMessage) return 0;

  uart_evt_t *e = (uart_evt_t*)evt.value.p;
  *out = e->byte;
  osPoolFree(poolUartEvt, e);
  return 1;
}

