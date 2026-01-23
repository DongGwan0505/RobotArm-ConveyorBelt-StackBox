/*
 * Presenter_RackPinion.c
 *
 * Created on: Jan 14, 2026
 * Author: kccistc
 */

#include "Presenter_RackPinion.h"
#include <stdio.h> // sprintf 사용을 위해 추가

// 0: 파이썬 전송용(이진 데이터), 1: 시리얼 모니터 디버깅용(텍스트)
#define DEBUG_MODE 0

void Presenter_RP_Init() {
    char msg[] = "\r\n=== RACK SYSTEM START ===\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
}

void Presenter_RP_Execute() {
    osEvent evt = osMessageGet(rpDataMsgBox, 10);

    if (evt.status == osEventMessage) {
        rp_t *pData = (rp_t *)evt.value.p;

        if (pData != NULL) {
#if DEBUG_MODE
            // --- 1. 시리얼 모니터 디버깅용 (텍스트 모드) ---
            char buf[64];
            // [Rack ID] IR상태, 박스개수 형식으로 출력
            sprintf(buf, "[Rack %d] IR: %s, Count: %d/5\r\n",
                    pData->id,
                    (pData->irStatus ? "DETECT" : "CLEAR"),
                    pData->boxCount);

            HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 100);

#else
            // --- 2. 파이썬/GUI 모드: 이진 데이터만 전송 ---
            HAL_UART_Transmit(&huart2, (uint8_t*)pData, sizeof(rp_t), 10);
#endif


            // 사용 후 메모리 해제
            osPoolFree(poolrpData, pData);
        }
    }
}
