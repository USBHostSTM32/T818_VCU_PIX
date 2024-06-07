/* Includes ------------------------------------------------------------------*/
#include "usbh_hid_t818.h"
#include "usbh_hid_parser.h"

static USBH_StatusTypeDef USBH_HID_T818Decode(USBH_HandleTypeDef *phost);

HID_T818_Info_TypeDef    t818_info;

uint8_t                  t818_report_data[T818_REPORT_SIZE];
uint8_t                  t818_rx_report_buf[T818_REPORT_SIZE];

/* Structures defining how to access items in a HID T818 report */

/* Access x coordinate change. */
static const HID_Report_ItemTypedef x_axis_state =
{
  t818_report_data+1, /*data*/
  16,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFFFF,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xFFFF,  /*max value device can report*/
  1      /*resolution*/
};

/* Access y coordinate change. */
static const HID_Report_ItemTypedef y_axis_state =
{
  t818_report_data + 3, /*data*/
  16,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1023,  /*max value read can return*/
  0,     /*min vale device can report*/
  1023,  /*max value device can report*/
  1      /*resolution*/
};


/* Access rz coordinate change. */
static const HID_Report_ItemTypedef rz_axis_state =
{
  t818_report_data + 5, /*data*/
  16,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1023,  /*max value read can return*/
  0,     /*min vale device can report*/
  1023,  /*max value device can report*/
  1      /*resolution*/
};


/* Access slider coordinate change. */
static const HID_Report_ItemTypedef slider_axis_state =
{
  t818_report_data + 7, /*data*/
  16,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1023,  /*max value read can return*/
  0,     /*min vale device can report*/
  1023,  /*max value device can report*/
  1      /*resolution*/
};

/* Access vx coordinate change. */
static const HID_Report_ItemTypedef vx_axis_state =
{
  t818_report_data + 9, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min value device can report*/
  0xFF,     /*max value device can report*/
  1      /*resolution*/
};

/* Access vy coordinate change. */
static const HID_Report_ItemTypedef vy_axis_state =
{
  t818_report_data + 10, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min value device can report*/
  0xFF,     /*max value device can report*/
  1      /*resolution*/
};

/* Access rx coordinate change. */
static const HID_Report_ItemTypedef rx_axis_state =
{
  t818_report_data + 11, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min value device can report*/
  0xFF,     /*max value device can report*/
  1      /*resolution*/
};

/* Access ry coordinate change. */
static const HID_Report_ItemTypedef ry_axis_state =
{
  t818_report_data + 12, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min value device can report*/
  0xFF,     /*max value device can report*/
  1      /*resolution*/
};

/* Access z coordinate change. */
static const HID_Report_ItemTypedef z_axis_state =
{
  t818_report_data + 13, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xFF,  /*max value read can return*/
  0,     /*min value device can report*/
  0xFF,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button paddle shifter left state. */
static const HID_Report_ItemTypedef paddle_shifter_left_state =
{
  t818_report_data + 15, /*data*/
  1,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,  /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};


/* Access button paddle shifter left state. */
static const HID_Report_ItemTypedef paddle_shifter_right_state =
{
  t818_report_data + 15, /*data*/
  1,     /*size*/
  1,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button drink state. */
static const HID_Report_ItemTypedef drink_state =
{
  t818_report_data + 15, /*data*/
  1,     /*size*/
  2,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button radio state. */
static const HID_Report_ItemTypedef radio_state =
{
  t818_report_data + 15, /*data*/
  1,     /*size*/
  3,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button 1 plus state. */
static const HID_Report_ItemTypedef one_plus_state =
{
  t818_report_data + 15, /*data*/
  1,     /*size*/
  4,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button 10 minus state. */
static const HID_Report_ItemTypedef ten_minus_state =
{
  t818_report_data + 15, /*data*/
  1,     /*size*/
  5,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button sha state. */
static const HID_Report_ItemTypedef sha_state =
{
  t818_report_data + 15, /*data*/
  1,     /*size*/
  6,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button oil state. */
static const HID_Report_ItemTypedef oil_state =
{
  t818_report_data + 15, /*data*/
  1,     /*size*/
  7,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button parking state. */
static const HID_Report_ItemTypedef parking_state =
{
  t818_report_data + 16, /*data*/
  1,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button neutral state. */
static const HID_Report_ItemTypedef neutral_state =
{
  t818_report_data + 16, /*data*/
  1,     /*size*/
  1,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button K1 state. */
static const HID_Report_ItemTypedef k1_state =
{
  t818_report_data + 16, /*data*/
  1,     /*size*/
  2,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button K2 state. */
static const HID_Report_ItemTypedef k2_state =
{
  t818_report_data + 16, /*data*/
  1,     /*size*/
  3,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button S1 state. */
static const HID_Report_ItemTypedef s1_state =
{
  t818_report_data + 16, /*data*/
  1,     /*size*/
  4,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button left side wheel up state. */
static const HID_Report_ItemTypedef left_side_wheel_up_state =
{
  t818_report_data + 16, /*data*/
  1,     /*size*/
  5,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button left side wheel down state. */
static const HID_Report_ItemTypedef left_side_wheel_down_state =
{
  t818_report_data + 16, /*data*/
  1,     /*size*/
  6,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button right side wheel down state. */
static const HID_Report_ItemTypedef right_side_wheel_down_state =
{
  t818_report_data + 16, /*data*/
  1,     /*size*/
  7,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button right side wheel up state. */
static const HID_Report_ItemTypedef right_side_wheel_up_state =
{
  t818_report_data + 17, /*data*/
  1,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button grip anticlockwise state. */
static const HID_Report_ItemTypedef grip_anticlockwise_state =
{
  t818_report_data + 17, /*data*/
  1,     /*size*/
  1,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button grip clockwise state. */
static const HID_Report_ItemTypedef grip_clockwise_state =
{
  t818_report_data + 17, /*data*/
  1,     /*size*/
  2,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button eng anticlockwise state. */
static const HID_Report_ItemTypedef eng_anticlockwise_state =
{
  t818_report_data + 17, /*data*/
  1,     /*size*/
  3,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button eng clockwise state. */
static const HID_Report_ItemTypedef eng_clockwise_state =
{
  t818_report_data + 17, /*data*/
  1,     /*size*/
  4,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button button 22 state. */
static const HID_Report_ItemTypedef button_22_state =
{
  t818_report_data + 17, /*data*/
  1,     /*size*/
  5,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button button 23 state. */
static const HID_Report_ItemTypedef button_23_state =
{
  t818_report_data + 17, /*data*/
  1,     /*size*/
  6,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button grip state. */
static const HID_Report_ItemTypedef grip_state =
{
  t818_report_data + 17, /*data*/
  1,     /*size*/
  7,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access button eng state. */
static const HID_Report_ItemTypedef eng_state =
{
  t818_report_data + 18, /*data*/
  1,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/* Access arrow pad state. */
static const HID_Report_ItemTypedef pad_arrow_state =
{
  t818_report_data + 19, /*data*/
  4,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0x0F,  /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};

/**
  * @}
  */


/** @defgroup USBH_HID_T818_Private_Functions
  * @{
  */

/**
  * @brief  USBH_HID_T818Init
  *         The function init the HID T818.
  * @param  phost: Host handle
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_HID_T818Init(USBH_HandleTypeDef *phost)
{
  uint32_t i;
  HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;

  memset(&t818_info, 0, sizeof(HID_T818_Info_TypeDef));

  for (i = 0U; i < (sizeof(t818_report_data)); i++)
  {
  	t818_report_data[i] = 0U;
  	t818_rx_report_buf[i]=0U;
  }

  if (HID_Handle->length > sizeof(t818_report_data))
  {
    HID_Handle->length = (uint16_t)sizeof(t818_report_data);
  }

  HID_Handle->pData = t818_rx_report_buf;
  if ((HID_QUEUE_SIZE * sizeof(t818_report_data)) > sizeof(phost->device.Data))
  {
	  return USBH_FAIL;
  }
  else
  {
	  USBH_HID_FifoInit(&HID_Handle->fifo, phost->device.Data, HID_QUEUE_SIZE * sizeof(t818_report_data));
  }
  return USBH_OK;
}

/**
  * @brief  USBH_HID_GetT818Info
  *         The function return T818 information.
  * @param  phost: Host handle
  * @retval T818 information
  */
HID_T818_Info_TypeDef *USBH_HID_GetT818Info(USBH_HandleTypeDef *phost)
{
  if (USBH_HID_T818Decode(phost) == USBH_OK)
  {
    return &t818_info;
  }
  else
  {
    return NULL;
  }
}



/**
  * @brief  USBH_HID_T818Decode
  *         The function decode T818 data.
  * @param  phost: Host handle
  * @retval USBH Status
  */
static USBH_StatusTypeDef USBH_HID_T818Decode(USBH_HandleTypeDef *phost)
{
  HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef *) phost->pActiveClass->pData;

  if (HID_Handle->length == 0U || (HID_Handle->fifo.buf == NULL))
  {
    return USBH_FAIL;
  }

  /*Fill report */
  if (USBH_HID_FifoRead(&HID_Handle->fifo, &t818_report_data, HID_Handle->length) ==  HID_Handle->length)
  {

    /*Decode report */
    t818_info.wheel_rotation = (uint16_t)HID_ReadItem((HID_Report_ItemTypedef *) &x_axis_state, 0U);
    t818_info.brake = (uint16_t)HID_ReadItem((HID_Report_ItemTypedef *) &y_axis_state, 0U);
    t818_info.throttle = (uint16_t)HID_ReadItem((HID_Report_ItemTypedef *) &rz_axis_state, 0U);
    t818_info.clutch = (uint16_t)HID_ReadItem((HID_Report_ItemTypedef *) &slider_axis_state, 0U);
    t818_info.vx_axis = (uint16_t)HID_ReadItem((HID_Report_ItemTypedef *) &vx_axis_state, 0U);
    t818_info.vy_axis = (uint16_t)HID_ReadItem((HID_Report_ItemTypedef *) &vy_axis_state, 0U);
    t818_info.rx_axis = (uint16_t)HID_ReadItem((HID_Report_ItemTypedef *) &rx_axis_state, 0U);
    t818_info.ry_axis = (uint16_t)HID_ReadItem((HID_Report_ItemTypedef *) &ry_axis_state, 0U);
    t818_info.z_axis = (uint16_t)HID_ReadItem((HID_Report_ItemTypedef *) &z_axis_state, 0U);

    t818_info.paddle_shifter_left = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &paddle_shifter_left_state, 0U);
    t818_info.paddle_shifter_right = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &paddle_shifter_right_state, 0U);
    t818_info.drink = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &drink_state, 0U);
    t818_info.radio = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &radio_state, 0U);
    t818_info.one_plus = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &one_plus_state, 0U);
    t818_info.ten_minus = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &ten_minus_state, 0U);
    t818_info.sha = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &sha_state, 0U);
    t818_info.oil = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &oil_state, 0U);

    t818_info.parking = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &parking_state, 0U);
    t818_info.neutral = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &neutral_state, 0U);
    t818_info.k1 = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &k1_state, 0U);
    t818_info.k2 = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &k2_state, 0U);
    t818_info.s1 = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &s1_state, 0U);
    t818_info.left_side_wheel_up = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &left_side_wheel_up_state, 0U);
    t818_info.left_side_wheel_down = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &left_side_wheel_down_state, 0U);
    t818_info.right_side_wheel_up = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &right_side_wheel_up_state, 0U);

    t818_info.right_side_wheel_down = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &right_side_wheel_down_state, 0U);
    t818_info.grip_anticlockwise = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &grip_anticlockwise_state, 0U);
    t818_info.grip_clockwise = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &grip_clockwise_state, 0U);
    t818_info.eng_anticlockwise = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &eng_anticlockwise_state, 0U);
    t818_info.eng_clockwise = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &eng_clockwise_state, 0U);
    t818_info.button_22 = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &button_22_state, 0U);
    t818_info.button_23 = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &button_23_state, 0U);
    t818_info.grip = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &grip_state, 0U);

    t818_info.eng = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &eng_state, 0U);

    t818_info.pad_arrow = (uint8_t)HID_ReadItem((HID_Report_ItemTypedef *) &pad_arrow_state, 0U);

    return USBH_OK;
  }

  return USBH_FAIL;
}




/************************ END OF FILE****/
