#include "Model_Motor.h"

osPoolDef(MotorPool,MOTOR_MSG_DEPTH, MotorData_t);
static osPoolId s_motorPool;

osMessageQDef(MotorQ, MOTOR_MSG_DEPTH, uint32_t);
static osMessageQId s_motorQ;

osPoolDef(MotorBatchPool, 1, MotorBatch_t);
static osPoolId s_batchPool;

osMessageQDef(MotorBatchQ, 1, uint32_t);
static osMessageQId s_batchQ;


void ModelMotorInit()
{
	 s_motorPool = osPoolCreate(osPool(MotorPool));
	 s_motorQ = osMessageCreate(osMessageQ(MotorQ), NULL);
	 s_batchPool = osPoolCreate(osPool(MotorBatchPool));
	 s_batchQ    = osMessageCreate(osMessageQ(MotorBatchQ), NULL);
}

osStatus ModelSetBachMotor(MotorData_t *cmds, uint8_t count)
{
	if(!cmds || count == 0 || count > MOTOR_MAX) return osErrorParameter;
	if(!s_batchPool || !s_batchQ) return osErrorResource;
	for(;;){
		osEvent e = osMessageGet(s_batchQ, 0);
		if(e.status != osEventMessage) break;
		MotorBatch_t *old = (MotorBatch_t*)(uintptr_t)e.value.v;
		if(old) osPoolFree(s_batchPool, old);
	}

	MotorBatch_t *node = (MotorBatch_t*)osPoolAlloc(s_batchPool);
	if(!node) return osErrorNoMemory;

	node->count = count;
	for(uint8_t i= 0; i<count; i++){
		node->cmd[i] = cmds[i];
	}
	osStatus st = osMessagePut(s_batchQ, (uint32_t)(uintptr_t)node, 0);
	if(st != osOK) osPoolFree(s_batchPool, node);
	return st;
}
int ModelGetBatchMotor(MotorBatch_t *out, uint32_t timeout)
{
	if(!out) return 0;
	if(!s_batchPool || !s_batchQ) return 0;

	osEvent e = osMessageGet(s_batchQ, timeout);
	if(e.status != osEventMessage) return 0;

	MotorBatch_t *node = (MotorBatch_t*)(uintptr_t)e.value.v;
	if(!node) return 0;
	*out = *node;
	osPoolFree(s_batchPool, node);
	return 1;
}

osStatus ModelSetResult(MotorData_t *cmd)
{
	if(!cmd) return osErrorParameter;
	if(!s_motorPool || !s_motorQ) return osErrorResource;

	for(;;){
	        osEvent e = osMessageGet(s_motorQ, 0);
	        if(e.status != osEventMessage) break;

	        MotorData_t *old = (MotorData_t*)(uintptr_t)e.value.v;
	        if(old) osPoolFree(s_motorPool, old);
	    }

	MotorData_t *node = (MotorData_t*)osPoolAlloc(s_motorPool);
	if(!node) return osErrorNoMemory;


	*node = *cmd;
	osStatus st = osMessagePut(s_motorQ, (uint32_t)(uintptr_t)node, 0);
	if(st != osOK){
		osPoolFree(s_motorPool, node);
	}
	return st;
}

int ModelGetResult(MotorData_t *out, uint32_t timeout)
{
	if(!out) return 0;
	if(!s_motorPool || !s_motorQ) return 0;
	osEvent e = osMessageGet(s_motorQ, timeout);
	if(e.status != osEventMessage) return 0;


	MotorData_t *node = (MotorData_t*)(uintptr_t)e.value.v;
	if(!node) return 0;

	*out = *node;
	osPoolFree(s_motorPool, node);
	return 1;
}

