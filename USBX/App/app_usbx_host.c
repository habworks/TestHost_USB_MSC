/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_usbx_host.c
  * @author  MCD Application Team
  * @brief   USBX host applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "app_usbx_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "TestApp_USB_MSC.h"
#include "IO_Support.h"
#include "usb_otg.h"
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

static TX_THREAD ux_host_app_thread;

/* USER CODE BEGIN PV */
TX_THREAD                   msc_app_thread;
UX_HOST_CLASS_STORAGE       *storage;
UX_HOST_CLASS_STORAGE_MEDIA *storage_media;
FX_MEDIA                    *USB_Media;
TX_EVENT_FLAGS_GROUP        USB_EventFlag;

UCHAR *USBX_AllocatedStackMemoryPtr;
UCHAR *USBX_AllocatedHostAppTaskPtr;
bool USB_EventFlagCreated = false;
bool USB_OTG_FS_HCD_Init = false;

extern HCD_HandleTypeDef hhcd_USB_OTG_FS;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID app_ux_host_thread_entry(ULONG thread_input);
static UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *current_class, VOID *current_instance);
static VOID ux_host_error_callback(UINT system_level, UINT system_context, UINT error_code);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application USBX Host Initialization.
  * @param  memory_ptr: memory pointer
  * @retval status
  */
UINT MX_USBX_Host_Init(VOID *memory_ptr)
{
  UINT ret = UX_SUCCESS;
  UCHAR *pointer;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN MX_USBX_Host_Init0 */

  /* USER CODE END MX_USBX_Host_Init0 */

  /* Allocate the stack for USBX Memory */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       USBX_HOST_MEMORY_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_ALLOCATE_STACK_ERORR */
    return TX_POOL_ERROR;
    /* USER CODE END USBX_ALLOCATE_STACK_ERORR */
  }
  USBX_AllocatedStackMemoryPtr = pointer;

  /* Initialize USBX Memory */
  if (ux_system_initialize(pointer, USBX_HOST_MEMORY_STACK_SIZE, UX_NULL, 0) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_SYSTEM_INITIALIZE_ERORR */
    return UX_ERROR;
    /* USER CODE END USBX_SYSTEM_INITIALIZE_ERORR */
  }

  /* Install the host portion of USBX */
  if (ux_host_stack_initialize(ux_host_event_callback) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_HOST_INITIALIZE_ERORR */
    return UX_ERROR;
    /* USER CODE END USBX_HOST_INITIALIZE_ERORR */
  }

  /* Register a callback error function */
  ux_utility_error_callback_register(&ux_host_error_callback);

  /* Initialize the host storage class */
  if (ux_host_stack_class_register(_ux_system_host_class_storage_name,
                                   ux_host_class_storage_entry) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_HOST_STORAGE_REGISTER_ERORR */
    return UX_ERROR;
    /* USER CODE END USBX_HOST_STORAGE_REGISTER_ERORR */
  }

  /* Allocate the stack for host application main thread */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, UX_HOST_APP_THREAD_STACK_SIZE,
                       TX_NO_WAIT) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_ALLOCATE_STACK_ERORR */
    return TX_POOL_ERROR;
    /* USER CODE END MAIN_THREAD_ALLOCATE_STACK_ERORR */
  }
  USBX_AllocatedHostAppTaskPtr = pointer;

  /* Create the host application main thread */
  if (tx_thread_create(&ux_host_app_thread, UX_HOST_APP_THREAD_NAME, app_ux_host_thread_entry,
                       0, pointer, UX_HOST_APP_THREAD_STACK_SIZE, UX_HOST_APP_THREAD_PRIO,
                       UX_HOST_APP_THREAD_PREEMPTION_THRESHOLD, UX_HOST_APP_THREAD_TIME_SLICE,
                       UX_HOST_APP_THREAD_START_OPTION) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_CREATE_ERORR */
    return TX_THREAD_ERROR;
    /* USER CODE END MAIN_THREAD_CREATE_ERORR */
  }

  /* USER CODE BEGIN MX_USBX_Host_Init1 */
    /* Create the event flags group */
    if (!USB_EventFlagCreated)
    {
        USB_EventFlagCreated = true;
        if (tx_event_flags_create(&USB_EventFlag, "USB Event Flag") != TX_SUCCESS)
        {
          return TX_GROUP_ERROR;
        }
    }
  /* USER CODE END MX_USBX_Host_Init1 */

  return ret;
}

/**
  * @brief  Function implementing app_ux_host_thread_entry.
  * @param  thread_input: User thread input parameter.
  * @retval none
  */
static VOID app_ux_host_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN app_ux_host_thread_entry */
  TX_PARAMETER_NOT_USED(thread_input);
  USBX_APP_Host_Init();
  /* USER CODE END app_ux_host_thread_entry */
}

/**
  * @brief  ux_host_event_callback
  *         This callback is invoked to notify application of instance changes.
  * @param  event: event code.
  * @param  current_class: Pointer to class.
  * @param  current_instance: Pointer to class instance.
  * @retval status
  */
UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *current_class, VOID *current_instance)
{
  UINT status = UX_SUCCESS;

  /* USER CODE BEGIN ux_host_event_callback0 */
//  UX_PARAMETER_NOT_USED(current_class);
//  UX_PARAMETER_NOT_USED(current_instance);
  /* USER CODE END ux_host_event_callback0 */

  switch (event)
  {
    case UX_DEVICE_INSERTION:

      /* USER CODE BEGIN UX_DEVICE_INSERTION */
      /* Get current Storage Class */
      if (current_class -> ux_host_class_entry_function == ux_host_class_storage_entry)
      {
        if (storage == UX_NULL)
        {
          /* Get current Storage Instance */
          storage = (UX_HOST_CLASS_STORAGE *)current_instance;

          /* Get the storage media */
          storage_media = (UX_HOST_CLASS_STORAGE_MEDIA *)current_class -> ux_host_class_media;

          if (storage_media -> ux_host_class_storage_media_lun != 0)
          {
            storage_media = UX_NULL;
          }
          else
          {
            /* Get the media file */
            USB_Media = &storage_media->ux_host_class_storage_media;
          }

          /* Check the storage class state */
          if (storage -> ux_host_class_storage_state ==  UX_HOST_CLASS_INSTANCE_LIVE)
          {
            /* Set STORAGE_MEDIA flag */
            if (tx_event_flags_set(&USB_EventFlag, USB_EVENT_MSC_INSERTED, TX_OR) != TX_SUCCESS)
            {
              Error_Handler();
            }
          }
        }
      }
      /* USER CODE END UX_DEVICE_INSERTION */

      break;

    case UX_DEVICE_REMOVAL:

      /* USER CODE BEGIN UX_DEVICE_REMOVAL */
      if ((VOID*)storage == current_instance)
      {
        /* Clear storage media instance & media file */
        storage = UX_NULL;
        storage_media = UX_NULL;
        USB_Media = UX_NULL;

        if (tx_event_flags_set(&USB_EventFlag, USB_EVENT_MSC_REMOVED, TX_OR) != TX_SUCCESS)
        {
          Error_Handler();
        }
      }
      /* USER CODE END UX_DEVICE_REMOVAL */

      break;

#if defined (UX_HOST_CLASS_STORAGE_NO_FILEX)
    case UX_STORAGE_MEDIA_INSERTION:

      /* USER CODE BEGIN UX_STORAGE_MEDIA_INSERTION */

      /* USER CODE END UX_STORAGE_MEDIA_INSERTION */

      break;

    case UX_STORAGE_MEDIA_REMOVAL:

      /* USER CODE BEGIN UX_STORAGE_MEDIA_REMOVAL */

      /* USER CODE END UX_STORAGE_MEDIA_REMOVAL */

      break;
#endif

    case UX_DEVICE_CONNECTION:

      /* USER CODE BEGIN UX_DEVICE_CONNECTION */

      /* USER CODE END UX_DEVICE_CONNECTION */

      break;

    case UX_DEVICE_DISCONNECTION:

      /* USER CODE BEGIN UX_DEVICE_DISCONNECTION */

      /* USER CODE END UX_DEVICE_DISCONNECTION */

      break;

    default:

      /* USER CODE BEGIN EVENT_DEFAULT */

      /* USER CODE END EVENT_DEFAULT */

      break;
  }

  /* USER CODE BEGIN ux_host_event_callback1 */

  /* USER CODE END ux_host_event_callback1 */

  return status;
}

/**
  * @brief ux_host_error_callback
  *         This callback is invoked to notify application of error changes.
  * @param  system_level: system level parameter.
  * @param  system_context: system context code.
  * @param  error_code: error event code.
  * @retval Status
  */
VOID ux_host_error_callback(UINT system_level, UINT system_context, UINT error_code)
{
  /* USER CODE BEGIN ux_host_error_callback0 */
  UX_PARAMETER_NOT_USED(system_level);
  UX_PARAMETER_NOT_USED(system_context);
  /* USER CODE END ux_host_error_callback0 */

  switch (error_code)
  {
    case UX_DEVICE_ENUMERATION_FAILURE:

      /* USER CODE BEGIN UX_DEVICE_ENUMERATION_FAILURE */

      /* USER CODE END UX_DEVICE_ENUMERATION_FAILURE */

      break;

    case  UX_NO_DEVICE_CONNECTED:

      /* USER CODE BEGIN UX_NO_DEVICE_CONNECTED */

      /* USER CODE END UX_NO_DEVICE_CONNECTED */

      break;

    default:

      /* USER CODE BEGIN ERROR_DEFAULT */

      /* USER CODE END ERROR_DEFAULT */

      break;
  }

  /* USER CODE BEGIN ux_host_error_callback1 */

  /* USER CODE END ux_host_error_callback1 */
}

/* USER CODE BEGIN 1 */
/**
  * @brief  USBX_APP_Host_Init
  *         Initialization of USB Host.
  * @param  None
  * @retval None
  */
VOID USBX_APP_Host_Init(VOID)
{
  /* USER CODE BEGIN USB_Host_Init_PreTreatment_0 */

  /* USER CODE END USB_Host_Init_PreTreatment_0 */

  /* Initialize the LL driver */
  if (!USB_OTG_FS_HCD_Init)
  {
      USB_OTG_FS_HCD_Init = true;
      MX_USB_OTG_FS_HCD_Init();
  }

  /* Register all the USB host controllers available in this system. */
  ux_host_stack_hcd_register(_ux_system_host_hcd_stm32_name,
                             _ux_hcd_stm32_initialize, USB_OTG_FS_PERIPH_BASE,
                             (ULONG)&hhcd_USB_OTG_FS);

  /* Drive vbus */
  USBH_DriverVBUS(1);

  /* Enable USB Global Interrupt */
  HAL_HCD_Start(&hhcd_USB_OTG_FS);

  /* USER CODE BEGIN USB_Host_Init_PreTreatment1 */

  /* USER CODE END USB_Host_Init_PreTreatment1 */

}

/**
* @brief  Drive VBUS.
* @param  state : VBUS state
*          This parameter can be one of the these values:
*           1 : VBUS Active
*           0 : VBUS Inactive
* @retval Status
*/
void USBH_DriverVBUS(uint8_t state)
{
  /* USER CODE BEGIN 0 */

  /* USER CODE END 0*/

  if (state == 1)
  {
    /* Drive high Charge pump */
    /* Add IOE driver control */
    /* USER CODE BEGIN DRIVE_HIGH_CHARGE_FOR_FS */
      VBUS_ON();
    /* USER CODE END DRIVE_HIGH_CHARGE_FOR_FS */
  }
  else
  {
    /* Drive low Charge pump */
    /* Add IOE driver control */
    /* USER CODE BEGIN DRIVE_LOW_CHARGE_FOR_FS */
      VBUS_OFF();
    /* USER CODE END DRIVE_LOW_CHARGE_FOR_FS */
  }

  HAL_Delay(500);
}


/*******************************************************************************************************
* @brief Uninitialize the USBX as a host.  This is the opposite function to MX_USBX_Host_Init.  They are
* seven steps that are performed in the MX_USBX_Host_Init process.  This function reverses / removes the
* affect of those actions.  The steps performed are:
* 1. Allocated memory of the USBX stack
* 2. Initialized USBX Memory
* 3. Install the host portion of USBX
* 4. Register callback error function
* 5. Initialized the host storage class
* 6. Allocated memory of the Host application main thread
* 7. Create the host application main thread
*
* @author original: Hab Collector \n
*
* @return True if file found
*
* STEP 1: Reverse the operation of the function MX_USBX_Host_Init
* STEP 2: Return the result: Ux result as MSB, Tx as LSB
********************************************************************************************************/
uint16_t MX_USBX_Host_UnInit(void)
{
    UINT Tx_Status = TX_SUCCESS;
    UINT Ux_Status = UX_SUCCESS;

    // STEP 1: Reverse the operation of the function MX_USBX_Host_Init
    // 7. Delete the host application main thread
    Tx_Status = tx_thread_delete(&ux_host_app_thread);

    // 6. Release the allocated memory of the Host application main thread
    if ((Tx_Status == TX_SUCCESS) && (Ux_Status == UX_SUCCESS))
        Tx_Status = tx_byte_release((VOID *)USBX_AllocatedHostAppTaskPtr);

    // 5. Uninitialized the host storage class
    if ((Tx_Status == TX_SUCCESS) && (Ux_Status == UX_SUCCESS))
        Ux_Status = ux_host_stack_class_unregister(ux_host_class_storage_entry);

    // 4. Unregister callback error function
    ux_utility_error_callback_register(NULL);

    // 3. Un-install the host portion of USBX
    if ((Tx_Status == TX_SUCCESS) && (Ux_Status == UX_SUCCESS))
        Ux_Status = ux_host_stack_uninitialize();

    // 2. Uninitialized USBX Memory
    if ((Tx_Status == TX_SUCCESS) && (Ux_Status == UX_SUCCESS))
        Ux_Status = ux_system_uninitialize();

    // 1. Release the allocated memory of the USBX stack
    if ((Tx_Status == TX_SUCCESS) && (Ux_Status == UX_SUCCESS))
        Tx_Status = tx_byte_release((VOID *)USBX_AllocatedStackMemoryPtr);

    // STEP 2: Return the result: Ux result as MSB, Tx as LSB
    uint16_t Status = Ux_Status & 0xFF;
    Status = Status << 8;
    Status = Status | (Tx_Status & 0xFF);
    return(Status);

} // END OF MX_USBX_Host_UnInit



/*******************************************************************************************************
* @brief Un-initialize the USBX host application.  This is the opposite function to USBX_APP_Host_Init.  They are
* four steps that are performed in the USBX_APP_Host_Init process.  This function reverses / removes the
* affect of those actions.  The steps performed are:
* 1. Initialize the LL driver
* 2. Register all the USB host controllers available in this system
* 3. Drive VBus
* 4. Enable USB Global Interrupt
*
* @note This function will not undo the LL driver init (step 1).  The function USBX_APP_Host_Init includes
* a flag where by the LL driver is only init once.
*
* @author original: Hab Collector \n
*
* @return True if file found
*
* STEP 1: Reverse the operation of the function USBX_APP_Host_Init
********************************************************************************************************/
void USBX_APP_Host_UnInit(void)
{
    // STEP 1: Reverse the operation of the function USBX_APP_Host_Init
    // 4. Disable USB Global Interrupt
    HAL_HCD_Stop(&hhcd_USB_OTG_FS);

    // 3. Disable VBus
    USBH_DriverVBUS(0);

    // 2. Un-regisister USB host controllers in the system
    ux_host_stack_hcd_unregister(_ux_system_host_hcd_stm32_name, USB_OTG_FS_PERIPH_BASE, (ULONG)&hhcd_USB_OTG_FS);

}  // END OF USBX_APP_Host_UnInit

/* USER CODE END 1 */
