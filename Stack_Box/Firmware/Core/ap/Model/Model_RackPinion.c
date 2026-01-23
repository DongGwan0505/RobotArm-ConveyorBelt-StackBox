/*
 * Model_RackPinion.c
 *
 * Created on: Jan 9, 2026
 * Author: kccistc
 */

#include "Model_RackPinion.h"

// 단일 변수에서 배열로 변경 (NUM_RP만큼 생성)
static rpState_t currentrpState[NUM_RP];
static int boxCount[NUM_RP];
static int currentIrStatus[NUM_RP]; // IR 상태 저장용 변수

// RTOS에서 제공하는 Queue
osMessageQId rpEventMsgBox; // 시스템 이벤트를 전달하기 위한 메시지 큐 ID
osMessageQDef(rpEventQue, 16, uint32_t); // 3개 시스템 대응을 위해 큐 크기 확장 (8 -> 16)

osMessageQId rpDataMsgBox; // Presenter로 보낼 데이터를 담는 메시지 큐 ID
osMessageQDef(rpDataQue, 16, uint32_t);

// RTOS에서 제공하는 동적메모리 할당
osPoolDef(poolrpData, 12, rp_t); // 시스템이 늘어났으므로 메모리 풀 블록 확장 (4 -> 12)
osPoolId poolrpData; // 메모리 풀의 ID

// 이벤트 동적할당 안 한 이유 : 작은 숫자는 메모리 풀에서 공간을 할당(Alloc)받고
// 그 주소(포인터)를 보내고 다시 해제(Free)하는 과정을 거치는 것보다,
// 숫자 값 자체를 큐에 직접 넣어서 보내는 것이 연산 속도가 훨씬 빠르고 메모리 관리 부담이 적어서

void Model_rpInit() {
    poolrpData = osPoolCreate(osPool(poolrpData)); // 정의된 풀 정보로 메모리 풀을 생성
    rpEventMsgBox = osMessageCreate(osMessageQ(rpEventQue), NULL); // 정의된 큐 정보로 이벤트 큐를 생성
    rpDataMsgBox = osMessageCreate(osMessageQ(rpDataQue), NULL);

    // 모든 시스템(0, 1, 2) 초기화
    for (int i = 0; i < NUM_RP; i++) {
        currentrpState[i] = RP_WAIT_BOX;
        boxCount[i] = 0;
        currentIrStatus[i] = 0;
    }
}

void Model_SetrpState(int id, rpState_t state) {
    if (id < NUM_RP) currentrpState[id] = state;
}

rpState_t Model_GetrpState(int id) {
    if (id < NUM_RP) return currentrpState[id];
    return RP_WAIT_BOX;
}

void Model_IncrementrpBoxCount(int id) {
    if (id < NUM_RP) boxCount[id]++;
}

int Model_GetrpBoxCount(int id) {
    if (id < NUM_RP) return boxCount[id];
    return 0;
}

void Model_ResetrpBoxCount(int id) {
    if (id < NUM_RP) boxCount[id] = 0;
}

void Model_SetIrStatus(int id, int status) {
    if (id < NUM_RP) currentIrStatus[id] = status;
}

int Model_GetIrStatus(int id) {
    if (id < NUM_RP) return currentIrStatus[id];
    return 0;
}
