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
#include "t818_drive_control.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UPDATE_STATE_PERIOD_MS                   	(20U)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern USBH_HandleTypeDef hUsbHostFS;

// Declaration and configurations
static t818_drive_control_t drive_control;

static const t818_drive_control_config_t t818_config = { .t818_host_handle =
		&hUsbHostFS };
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId updateStateTaskHandle;
uint32_t updateStateTaskBuffer[1024];
osStaticThreadDef_t updateStateTaskControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
	if (USBH_HID_GetT818Info(drive_control.config->t818_host_handle)
			!= USBH_OK) {
		Error_Handler();
	}
}
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const *argument);
void StartUpdateStateTask(void const *argument);

extern void MX_USB_HOST_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

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
	if (t818_drive_control_init(&drive_control, &t818_config,
			USBH_HID_T818GetInstance()) != T818_DC_OK) {
		Error_Handler();
	}

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
	osThreadStaticDef(updateStateTask, StartUpdateStateTask, osPriorityRealtime,
			0, 1024, updateStateTaskBuffer, &updateStateTaskControlBlock);
	updateStateTaskHandle = osThreadCreate(osThread(updateStateTask), NULL);

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
void StartDefaultTask(void const *argument) {
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
void StartUpdateStateTask(void const *argument) {
	/* USER CODE BEGIN StartUpdateStateTask */
	const TickType_t xFrequency = pdMS_TO_TICKS(UPDATE_STATE_PERIOD_MS); //TASK PERIOD
	TickType_t xLastWakeTime;

	// Initialize xLastWakeTime with the current time
	xLastWakeTime = xTaskGetTickCount();
	/* Infinite loop */
	for (;;) {
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		if (t818_drive_control_step(&drive_control) != T818_DC_OK) {
			Error_Handler();
		}
	}
	/* USER CODE END StartUpdateStateTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

