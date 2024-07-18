/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include <dbw_kernel.h>
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
// Declaration and configurations

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId updateStateTaskHandle;
uint32_t updateStateTaskBuffer[ 1024 ];
osStaticThreadDef_t updateStateTaskControlBlock;
osThreadId urbTxTaskHandle;
uint32_t urbTxTaskBuffer[ 256 ];
osStaticThreadDef_t urbTxTaskControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
	dbw_kernel_t *dwb_kernel = dbw_kernel_get_instance();
	if (USBH_HID_GetT818Info(dwb_kernel->drive_control.config->t818_host_handle)!= USBH_OK) {
		Error_Handler();
	}

}
#ifdef USE_CAN
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	dbw_kernel_t *dwb_kernel = dbw_kernel_get_instance();
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &dwb_kernel->can_manager.RxHeader, dwb_kernel->can_manager.rx_data);
}
#endif
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartUpdateStateTask(void const * argument);
void StartUrbTxTask(void const * argument);

extern void MX_USB_HOST_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
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
	if(dbw_kernel_init() != DBW_OK){
		Error_Handler();
	}

	 HAL_TIM_Base_Start(&htim13);
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

  /* definition and creation of updateStateTask */
  osThreadStaticDef(updateStateTask, StartUpdateStateTask, osPriorityHigh, 0, 1024, updateStateTaskBuffer, &updateStateTaskControlBlock);
  updateStateTaskHandle = osThreadCreate(osThread(updateStateTask), NULL);

  /* definition and creation of urbTxTask */
  osThreadStaticDef(urbTxTask, StartUrbTxTask, osPriorityRealtime, 0, 256, urbTxTaskBuffer, &urbTxTaskControlBlock);
  urbTxTaskHandle = osThreadCreate(osThread(urbTxTask), NULL);

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
  /* init code for USB_HOST */
  MX_USB_HOST_Init();
  /* USER CODE BEGIN StartDefaultTask */
	/* Infinite loop */
	for (;;) {
		osDelay(1);
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartUpdateStateTask */
/**
 * @brief Function implementing the updateStateTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartUpdateStateTask */
void StartUpdateStateTask(void const * argument)
{
  /* USER CODE BEGIN StartUpdateStateTask */
	dbw_kernel_t *dbw_kernel = dbw_kernel_get_instance();
	const TickType_t xFrequency = pdMS_TO_TICKS(UPDATE_STATE_PERIOD_MS); //TASK PERIOD
	TickType_t xLastWakeTime;

	// Initialize xLastWakeTime with the current time
	xLastWakeTime = xTaskGetTickCount();
	/* Infinite loop */
	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
#ifdef USE_CAN
		if ((can_parser_from_array_to_auto_control_feedback(dbw_kernel->can_manager.rx_data,
				&dbw_kernel->auto_control.auto_data_feedback) != CAN_PARSER_OK)) {
			Error_Handler();
		}
#endif
		if ((t818_drive_control_step(&dbw_kernel->drive_control,&dbw_kernel->urb_sender,&dbw_kernel->rotation_manager,dbw_kernel->auto_data_feedback.steer) != T818_DC_OK)) {
			Error_Handler();
		}

		if ((auto_control_step(&dbw_kernel->auto_control) != AUTO_CONTROL_OK)) {
			Error_Handler();
		}

#ifdef USE_CAN
		if ((can_parser_from_auto_control_to_array(
				dbw_kernel->auto_control.auto_control_data, dbw_kernel->can_manager.tx_data)
				!= CAN_PARSER_OK)) {
			Error_Handler();
		}

		if (can_manager_auto_control_tx(&dbw_kernel->can_manager,
				dbw_kernel->can_manager.tx_data) != CAN_MANAGER_OK) {
			Error_Handler();
		}
#endif
		timer_val = __HAL_TIM_GET_COUNTER(&htim13);
	}
  /* USER CODE END StartUpdateStateTask */
}

/* USER CODE BEGIN Header_StartUrbTxTask */
/**
 * @brief Function implementing the urbTxTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartUrbTxTask */
void StartUrbTxTask(void const * argument)
{
  /* USER CODE BEGIN StartUrbTxTask */
	dbw_kernel_t *dbw_kernel = dbw_kernel_get_instance();
	const TickType_t xFrequency = pdMS_TO_TICKS(URB_TX_PERIOD_MS); //TASK PERIOD
	TickType_t xLastWakeTime;

	// Initialize xLastWakeTime with the current time
	xLastWakeTime = xTaskGetTickCount();
	/* Infinite loop */
	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		if (urb_sender_dequeue_msg(&dbw_kernel->urb_sender) != URB_SENDER_OK) {
			Error_Handler();
		}
	}
  /* USER CODE END StartUrbTxTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

