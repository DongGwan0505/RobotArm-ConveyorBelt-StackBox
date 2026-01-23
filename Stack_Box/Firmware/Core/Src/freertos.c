/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
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
osThreadId ListenerTaskHandle;
osThreadId PresenterTaskHandle;
osThreadId ControllerTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Listener(void const * argument);
void Presenter(void const * argument);
void Controller(void const * argument);

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
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

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

  /* definition and creation of ListenerTask */
  osThreadDef(ListenerTask, Listener, osPriorityNormal, 0, 128);
  ListenerTaskHandle = osThreadCreate(osThread(ListenerTask), NULL);

  /* definition and creation of PresenterTask */
  osThreadDef(PresenterTask, Presenter, osPriorityNormal, 0, 128);
  PresenterTaskHandle = osThreadCreate(osThread(PresenterTask), NULL);

  /* definition and creation of ControllerTask */
  osThreadDef(ControllerTask, Controller, osPriorityNormal, 0, 512);
  ControllerTaskHandle = osThreadCreate(osThread(ControllerTask), NULL);

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

/* USER CODE BEGIN Header_Listener */
/**
* @brief Function implementing the ListenerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Listener */
void Listener(void const * argument)
{
  /* USER CODE BEGIN Listener */
	Listener_Init();
  /* Infinite loop */
  for(;;)
  {
	  Listener_Excute();
    osDelay(1);
  }
  /* USER CODE END Listener */
}

/* USER CODE BEGIN Header_Presenter */
/**
* @brief Function implementing the PresenterTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Presenter */
void Presenter(void const * argument)
{
  /* USER CODE BEGIN Presenter */
	Presenter_Init();
  /* Infinite loop */
  for(;;)
  {
	  Presenter_Excute();
    osDelay(1);
  }
  /* USER CODE END Presenter */
}

/* USER CODE BEGIN Header_Controller */
/**
* @brief Function implementing the ControllerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Controller */
void Controller(void const * argument)
{
  /* USER CODE BEGIN Controller */
	Controller_Init();
  /* Infinite loop */
  for(;;)
  {
	  Controller_Excute();
    osDelay(1);
  }
  /* USER CODE END Controller */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
