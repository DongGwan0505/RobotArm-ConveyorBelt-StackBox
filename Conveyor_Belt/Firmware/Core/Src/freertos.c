/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../ap/Listener/Listener.h"
#include "../ap/Controller/Controller.h"
#include "../ap/Presenter/Presenter.h"
#include "../ap/Model/Model_Motor.h"
#include"../ap/Model/Model_Uart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId ListenerHandle;
osThreadId ControllerHandle;
osThreadId PresenterHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void ListenerTask(void const * argument);
void ControllerTask(void const * argument);
void PresenterTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	Model_UartInit();
	ModelMotorInit();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Listener */
  osThreadDef(Listener, ListenerTask, osPriorityNormal, 0, 128);
  ListenerHandle = osThreadCreate(osThread(Listener), NULL);

  /* definition and creation of Controller */
  osThreadDef(Controller, ControllerTask, osPriorityNormal, 0, 128);
  ControllerHandle = osThreadCreate(osThread(Controller), NULL);

  /* definition and creation of Presenter */
  osThreadDef(Presenter, PresenterTask, osPriorityNormal, 0, 128);
  PresenterHandle = osThreadCreate(osThread(Presenter), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_ListenerTask */
/**
* @brief Function implementing the Listener thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ListenerTask */
void ListenerTask(void const * argument)
{
  /* USER CODE BEGIN ListenerTask */
	Listener_Init();
  /* Infinite loop */
  for(;;)
  {
	Listener_Excute();
    osDelay(1);
  }
  /* USER CODE END ListenerTask */
}

/* USER CODE BEGIN Header_ControllerTask */
/**
* @brief Function implementing the Controller thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ControllerTask */
void ControllerTask(void const * argument)
{
  /* USER CODE BEGIN ControllerTask */
	ControllerInit();
  /* Infinite loop */
  for(;;)
  {
	ControllerExcute();
    osDelay(1);
  }
  /* USER CODE END ControllerTask */
}

/* USER CODE BEGIN Header_PresenterTask */
/**
* @brief Function implementing the Presenter thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_PresenterTask */
void PresenterTask(void const * argument)
{
  /* USER CODE BEGIN PresenterTask */
	PresenterInit();
  /* Infinite loop */
  for(;;)
  {
	PresenterExcute();
    osDelay(1);
  }
  /* USER CODE END PresenterTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
