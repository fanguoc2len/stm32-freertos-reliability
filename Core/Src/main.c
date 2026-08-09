/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "task_supervisor.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "watchdog_policy.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint32_t produced_count;
  uint32_t consumed_count;
  uint32_t last_status;
  uint32_t total_access_count;
} SystemData_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CONTROL_TASK_PERIOD_TICKS      10U
#define DIAGNOSTIC_TASK_PERIOD_TICKS  100U
#define EVENT_CONTROL_CYCLE_DONE     (1UL << 0)
#define EVENT_DIAGNOSTIC_CYCLE_DONE  (1UL << 1)
#define MONITOR_TIMEOUT_TICKS  200U
#define EVENT_ALL_CYCLES_DONE \
  (EVENT_CONTROL_CYCLE_DONE | EVENT_DIAGNOSTIC_CYCLE_DONE)
#define BUTTON_DEBOUNCE_TICKS 50U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Definitions for ControlTask */
osThreadId_t ControlTaskHandle;
const osThreadAttr_t ControlTask_attributes = {
  .name = "ControlTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for DiagnosticTask */
osThreadId_t DiagnosticTaskHandle;
const osThreadAttr_t DiagnosticTask_attributes = {
  .name = "DiagnosticTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for EventTask */
osThreadId_t EventTaskHandle;
const osThreadAttr_t EventTask_attributes = {
  .name = "EventTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MonitorTask */
osThreadId_t MonitorTaskHandle;
const osThreadAttr_t MonitorTask_attributes = {
  .name = "MonitorTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for statusQueue */
osMessageQueueId_t statusQueueHandle;
const osMessageQueueAttr_t statusQueue_attributes = {
  .name = "statusQueue"
};
/* Definitions for buttonDebounceTimer */
osTimerId_t buttonDebounceTimerHandle;
const osTimerAttr_t buttonDebounceTimer_attributes = {
  .name = "buttonDebounceTimer"
};
/* Definitions for systemDataMutex */
osMutexId_t systemDataMutexHandle;
const osMutexAttr_t systemDataMutex_attributes = {
  .name = "systemDataMutex"
};
/* Definitions for buttonEventSemaphore */
osSemaphoreId_t buttonEventSemaphoreHandle;
const osSemaphoreAttr_t buttonEventSemaphore_attributes = {
  .name = "buttonEventSemaphore"
};
/* Definitions for systemEventFlags */
osEventFlagsId_t systemEventFlagsHandle;
const osEventFlagsAttr_t systemEventFlags_attributes = {
  .name = "systemEventFlags"
};
/* USER CODE BEGIN PV */
static volatile uint32_t control_task_run_count = 0U;
static volatile uint32_t diagnostic_task_run_count = 0U;
static volatile uint32_t queue_send_error_count = 0U;
static volatile uint32_t diagnostic_message_count = 0U;
static volatile uint32_t diagnostic_last_status = 0U;
static SystemData_t system_data = {0};
//static volatile uint32_t shared_counter = 0U;
static volatile uint32_t control_mutex_timeout_count = 0U;
static volatile uint32_t diagnostic_mutex_timeout_count = 0U;
static volatile uint32_t button_event_count = 0U;
static volatile uint32_t button_isr_count = 0U;
static volatile uint32_t button_semaphore_release_error_count = 0U;
//static volatile uint32_t monitor_sync_count = 0U;
static volatile uint32_t monitor_wait_error_count = 0U;
//static volatile uint32_t task_watchdog_fault_active = 0U;
//static volatile uint32_t task_watchdog_timeout_count = 0U;
static volatile uint32_t task_watchdog_last_error = 0U;
static volatile uint32_t watchdog_refresh_request_count = 0U;
static volatile uint32_t control_stack_min_free_bytes =0U;
static volatile uint32_t diagnostic_stack_min_free_bytes = 0U;
static volatile uint32_t event_stack_min_free_bytes =0U;
static volatile uint32_t monitor_stack_min_free_bytes =0U;
static volatile uint32_t button_timer_start_error_count =0U;
static volatile uint32_t button_debounced_count = 0U;
static volatile uint32_t button_bounce_rejected_count = 0U;
static TaskSupervisor_t task_supervisor;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartControlTask(void *argument);
void StartDiagnosticTask(void *argument);
void StartEventTask(void *argument);
void StartMonitorTask(void *argument);
void ButtonDebounceTimerCallback(void *argument);


/* USER CODE BEGIN PFP */
void SystemClock_Config(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  if (TaskSupervisor_Init(&task_supervisor) !=
      TASK_SUPERVISOR_OK)
  {
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of systemDataMutex */
  systemDataMutexHandle = osMutexNew(&systemDataMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of buttonEventSemaphore */
  buttonEventSemaphoreHandle = osSemaphoreNew(1, 0, &buttonEventSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of buttonDebounceTimer */
  buttonDebounceTimerHandle = osTimerNew(ButtonDebounceTimerCallback, osTimerOnce, NULL, &buttonDebounceTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of statusQueue */
  statusQueueHandle = osMessageQueueNew (16, sizeof(uint32_t), &statusQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ControlTask */
  ControlTaskHandle = osThreadNew(StartControlTask, NULL, &ControlTask_attributes);

  /* creation of DiagnosticTask */
  DiagnosticTaskHandle = osThreadNew(StartDiagnosticTask, NULL, &DiagnosticTask_attributes);

  /* creation of EventTask */
  EventTaskHandle = osThreadNew(StartEventTask, NULL, &EventTask_attributes);

  /* creation of MonitorTask */
  MonitorTaskHandle = osThreadNew(StartMonitorTask, NULL, &MonitorTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of systemEventFlags */
  systemEventFlagsHandle = osEventFlagsNew(&systemEventFlags_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  if ((ControlTaskHandle == NULL) ||
      (DiagnosticTaskHandle == NULL) ||
      (EventTaskHandle == NULL) ||
      (MonitorTaskHandle == NULL) ||
      (statusQueueHandle == NULL) ||
      (systemDataMutexHandle == NULL) ||
      (buttonEventSemaphoreHandle == NULL) ||
      (systemEventFlagsHandle == NULL))
  {
    Error_Handler();
  }
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : BUTTON_INT_Pin */
  GPIO_InitStruct.Pin = BUTTON_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(BUTTON_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if ((GPIO_Pin == BUTTON_INT_Pin) &&
      (buttonEventSemaphoreHandle != NULL))
  {
    button_isr_count++;

    if (osSemaphoreRelease(buttonEventSemaphoreHandle) != osOK)
    {
      button_semaphore_release_error_count++;
    }
  }
}


/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartControlTask */
/**
  * @brief  Function implementing the ControlTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartControlTask */
void StartControlTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  uint32_t next_wake_tick = osKernelGetTickCount();
  uint32_t status_message = 0U;

  (void)argument;

  for (;;)
  {
    control_task_run_count++;
    status_message++;
    if (osMessageQueuePut(statusQueueHandle,
                          &status_message,
                          0U,
                          0U) == osOK)
    {
      if (osMutexAcquire(systemDataMutexHandle, 1U) == osOK)
      {
        system_data.produced_count++;
        system_data.total_access_count++;

        (void)osMutexRelease(systemDataMutexHandle);
      }
      else
      {
        control_mutex_timeout_count++;
      }

    }
    else
    {
      queue_send_error_count++;
    }

    (void)osEventFlagsSet(systemEventFlagsHandle,
                          EVENT_CONTROL_CYCLE_DONE);
    next_wake_tick += CONTROL_TASK_PERIOD_TICKS;
    (void)osDelayUntil(next_wake_tick);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartDiagnosticTask */
/**
* @brief Function implementing the DiagnosticTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDiagnosticTask */
void StartDiagnosticTask(void *argument)
{
  /* USER CODE BEGIN StartDiagnosticTask */
	uint32_t next_wake_tick = osKernelGetTickCount();
	uint32_t received_status = 0U;

	(void)argument;

	for (;;)
	{
	  diagnostic_task_run_count++;

	while (osMessageQueueGet(statusQueueHandle,
	                           &received_status,
	                           NULL,
	                           0U) == osOK)
	  {
		if (osMutexAcquire(systemDataMutexHandle, 1U) == osOK)
		  {
		    system_data.consumed_count++;
		    system_data.last_status = received_status;
		    system_data.total_access_count++;

		    (void)osMutexRelease(systemDataMutexHandle);
		  }
		  else
		  {
		    diagnostic_mutex_timeout_count++;
		  }
	    diagnostic_last_status = received_status;
	    diagnostic_message_count++;
	  }
	  (void)osEventFlagsSet(systemEventFlagsHandle,
	                      EVENT_DIAGNOSTIC_CYCLE_DONE);
	  next_wake_tick += DIAGNOSTIC_TASK_PERIOD_TICKS;
	  (void)osDelayUntil(next_wake_tick);
	}
  /* USER CODE END StartDiagnosticTask */
}

/* USER CODE BEGIN Header_StartEventTask */
/**
* @brief Function implementing the EventTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartEventTask */
void StartEventTask(void *argument)
{
  /* USER CODE BEGIN StartEventTask */
  /* Infinite loop */
  (void)argument;
  for(;;)
  {
    if (osSemaphoreAcquire(buttonEventSemaphoreHandle,osWaitForever) == osOK){
    	if (osTimerStart(buttonDebounceTimerHandle,
    	                 BUTTON_DEBOUNCE_TICKS) != osOK)
    	{
    	  button_timer_start_error_count++;
    	}
    }
  }
  /* USER CODE END StartEventTask */
}

/* USER CODE BEGIN Header_StartMonitorTask */
/**
* @brief Function implementing the MonitorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMonitorTask */
void StartMonitorTask(void *argument)
{
  /* USER CODE BEGIN StartMonitorTask */
  uint32_t received_flags = 0U;
  (void)argument;
  /* Infinite loop */
  for(;;)
  {
	  received_flags =
	          osEventFlagsWait(systemEventFlagsHandle,
	                           EVENT_ALL_CYCLES_DONE,
	                           osFlagsWaitAll,
							   MONITOR_TIMEOUT_TICKS);
	  control_stack_min_free_bytes =
	      osThreadGetStackSpace(ControlTaskHandle);

	  diagnostic_stack_min_free_bytes =
	      osThreadGetStackSpace(DiagnosticTaskHandle);

	  event_stack_min_free_bytes =
	      osThreadGetStackSpace(EventTaskHandle);

	  monitor_stack_min_free_bytes =
	      osThreadGetStackSpace(MonitorTaskHandle);

	  if ((received_flags & osFlagsError) == 0U)
	  {
		 if (TaskSupervisor_ProcessWindow(
		          &task_supervisor,
		          TASK_SUPERVISOR_WINDOW_COMPLETE) !=
		      TASK_SUPERVISOR_OK)
		  {
		    Error_Handler();
		  }
	    //monitor_sync_count++;
	    //task_watchdog_fault_active = 0U;
		 if (WatchdogPolicy_IsRefreshAllowed(
		         task_supervisor.state) != 0U)
		 {
		   watchdog_refresh_request_count++;
		 }
	  }
	  else
	  {
	    monitor_wait_error_count++;
	    task_watchdog_last_error = received_flags;

	    if (received_flags == osFlagsErrorTimeout)
	    {
	      //task_watchdog_timeout_count++;
	      //task_watchdog_fault_active = 1U;

	      if (TaskSupervisor_ProcessWindow(
	              &task_supervisor,
	              TASK_SUPERVISOR_WINDOW_TIMEOUT) !=
	          TASK_SUPERVISOR_OK)
	      {
	        Error_Handler();
	      }

	      (void)osEventFlagsClear(systemEventFlagsHandle,
	                              EVENT_ALL_CYCLES_DONE);
	    }
	  }
  }
  /* USER CODE END StartMonitorTask */
}

/* ButtonDebounceTimerCallback function */
void ButtonDebounceTimerCallback(void *argument)
{
  /* USER CODE BEGIN ButtonDebounceTimerCallback */
	(void)argument;

	if (HAL_GPIO_ReadPin(BUTTON_INT_GPIO_Port,
	                     BUTTON_INT_Pin) == GPIO_PIN_SET)
	{
	  button_debounced_count++;
	}
	else
	{
	  button_bounce_rejected_count++;
	}
  /* USER CODE END ButtonDebounceTimerCallback */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  (void)file;
  (void)line;
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
