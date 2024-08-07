/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_usbx_host.h
  * @author  MCD Application Team
  * @brief   USBX Host applicative header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_USBX_HOST_H__
#define __APP_USBX_HOST_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ux_api.h"
#include "main.h"
#include "ux_host_msc.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ux_hcd_stm32.h"
#include "app_azure_rtos_config.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define USBX_HOST_MEMORY_STACK_SIZE     22 * 1024

#define UX_HOST_APP_THREAD_STACK_SIZE   1024
#define UX_HOST_APP_THREAD_PRIO         10

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
UINT MX_USBX_Host_Init(VOID *memory_ptr);

/* USER CODE BEGIN EFP */
VOID USBX_APP_Host_Init(VOID);
void USBH_DriverVBUS(uint8_t state);
uint16_t MX_USBX_Host_UnInit(void);
void USBX_APP_Host_UnInit(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

#ifndef UX_HOST_APP_THREAD_NAME
#define UX_HOST_APP_THREAD_NAME  "USBX App Host Main Thread"
#endif

#ifndef UX_HOST_APP_THREAD_PREEMPTION_THRESHOLD
#define UX_HOST_APP_THREAD_PREEMPTION_THRESHOLD  UX_HOST_APP_THREAD_PRIO
#endif

#ifndef UX_HOST_APP_THREAD_TIME_SLICE
#define UX_HOST_APP_THREAD_TIME_SLICE  TX_NO_TIME_SLICE
#endif

#ifndef UX_HOST_APP_THREAD_START_OPTION
#define UX_HOST_APP_THREAD_START_OPTION  TX_AUTO_START
#endif

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __APP_USBX_HOST_H__ */
