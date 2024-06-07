/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_HID_JOYSTICK_H
#define __USBH_HID_JOYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */

/** @defgroup USBH_HID_T818
  * @brief This file is the Header file for usbh_hid_t818.c
  * @{
  */


/** @defgroup USBH_HID_T818_Exported_Types
  * @{
  */

//COSTANTI DA AGGIUNGERE
#define T818_ID_VENDOR					(1103U)
#define T818_ID_PRODUCT					(46742U)
#define T818_REPORT_SIZE				(64)


typedef struct _HID_T818_Info{

	uint16_t wheel_rotation; //X axis
	uint16_t brake:10; // Y axis
	uint16_t throttle:10; // Rz axis
	uint16_t clutch:10; // Slider axis
	uint8_t vx_axis; // not mapped
	uint8_t vy_axis; // not mapped
	uint8_t rx_axis;
	uint8_t ry_axis;
	uint8_t z_axis; // not mapped

	uint8_t paddle_shifter_left:1;
	uint8_t paddle_shifter_right:1;
	uint8_t drink:1;
	uint8_t radio:1;
	uint8_t one_plus:1;
	uint8_t ten_minus:1;
	uint8_t sha:1;
	uint8_t oil:1;

	uint8_t parking:1;
	uint8_t neutral:1;
	uint8_t k1:1;
	uint8_t k2:1;
	uint8_t s1:1;
	uint8_t left_side_wheel_up:1;
	uint8_t left_side_wheel_down:1;
	uint8_t right_side_wheel_up:1;

	uint8_t right_side_wheel_down:1;
	uint8_t grip_anticlockwise:1;
	uint8_t grip_clockwise:1;
	uint8_t eng_anticlockwise:1;
	uint8_t eng_clockwise:1;
	uint8_t button_22:1;
	uint8_t button_23:1;
	uint8_t grip:1;

	uint8_t eng:1;

	uint8_t pad_arrow:4;
}HID_T818_Info_TypeDef;

/**
  * @}
  */

/** @defgroup USBH_HID_T818_Exported_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_HID_T818_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_HID_T818_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_HID_T818_Exported_FunctionsPrototype
  * @{
  */
extern USBH_StatusTypeDef USBH_HID_T818Init(USBH_HandleTypeDef *phost);
HID_T818_Info_TypeDef *USBH_HID_GetT818Info(USBH_HandleTypeDef *phost);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBH_HID_T818_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
