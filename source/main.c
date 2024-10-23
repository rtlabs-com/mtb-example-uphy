/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * http://www.rt-labs.com
 * Copyright 2024 rt-labs AB, Sweden.
 * See LICENSE file in the project root for full license information.
 ********************************************************************/

/*
 * Entry point for the U-Phy Demo Application.
 */

#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"
#include "cy_log.h"

/* FreeRTOS header file */
#include <FreeRTOS.h>
#include <event_groups.h>
#include <task.h>

#include "cy_log.h"
#include "osal_log.h"

#include "uphy_demo_app.h"
#include "shell.h"
#include "filesys.h"
#include <inttypes.h>

#define LED_PROFINET_FLASH_DELAY (500 / portTICK_PERIOD_MS)
#define LED_ACTIVITY_FLASH_DELAY (20 / portTICK_PERIOD_MS)

#define LED_EVT_PROFINET_SIGNAL    (1 << 0)
#define LED_EVT_ENTER_RUNNING_MODE (1 << 1)
#define LED_EVT_EXIT_RUNNING_MODE  (1 << 2)
#define LED_EVT_MASK                                                           \
   (LED_EVT_PROFINET_SIGNAL | LED_EVT_ENTER_RUNNING_MODE |                     \
    LED_EVT_EXIT_RUNNING_MODE)

#define BUTTION_RELEASED (true)
#define BUTTION_PRESSED  (false)
#define LED_ON           (false)
#define LED_OFF          (true)

#define LED_TASK_PRIORITY (tskIDLE_PRIORITY + 4)

static TaskHandle_t led_task_hdl = NULL;
static EventGroupHandle_t led_events;

/**
 * Mode LED task
 * In a U-Phy context RUNNING means active PLC connection.
 * When device is not in RUNNING mode the mode LED flashes
 * at 0,5Hz .
 * When in RUNNING mode the mode LED is stable ON.
 * When PROFINET signal LED is also implemented using the mode LED.
 */
void led_task (void * arg)
{
   EventBits_t events;
   bool running = false;

   for (;;)
   {
      events =
         xEventGroupWaitBits (led_events, LED_EVT_MASK, pdTRUE, pdFALSE, 2000);

      if (events & LED_EVT_PROFINET_SIGNAL)
      {
         /* 1Hz flash for 3s */
         for (uint16_t i = 0; i < 6; i++)
         {
            cyhal_gpio_toggle (CYBSP_USER_LED3);
            vTaskDelay (LED_PROFINET_FLASH_DELAY);
         }
      }

      if (events & LED_EVT_ENTER_RUNNING_MODE)
      {
         running = true;
         cyhal_gpio_write (CYBSP_USER_LED3, LED_ON);
      }

      if (events & LED_EVT_EXIT_RUNNING_MODE)
      {
         running = false;
         cyhal_gpio_write (CYBSP_USER_LED3, LED_OFF);
      }

      /* No PLC connected - Flash activity LED every second */
      if (events == 0 && !running)
      {
         cyhal_gpio_write (CYBSP_USER_LED3, LED_ON);
         vTaskDelay (LED_ACTIVITY_FLASH_DELAY);
         cyhal_gpio_write (CYBSP_USER_LED3, LED_OFF);
      }
   }
}

void led_profinet_signal (void)
{
   xEventGroupSetBits (led_events, LED_EVT_PROFINET_SIGNAL);
}

void led_set_running_mode (bool on)
{
   if (on)
   {
      xEventGroupSetBits (led_events, LED_EVT_ENTER_RUNNING_MODE);
   }
   else
   {
      xEventGroupSetBits (led_events, LED_EVT_EXIT_RUNNING_MODE);
   }
}

void led_error (void)
{
   cyhal_gpio_write (CYBSP_USER_LED3, LED_OFF);
}

/* Bit 0 and 1 of output data is mapped to EVK USER LEDs
 * bit value 1 => LED ON
 * bit value 0 => LED OFF
 */
void digio_set_output (uint8_t data)
{
   cyhal_gpio_write (CYBSP_USER_LED1, (data & 0x01) ? LED_ON : LED_OFF);
   cyhal_gpio_write (CYBSP_USER_LED2, (data & 0x02) ? LED_ON : LED_OFF);
}

/* Bit 0 and 1 of input data is mapped to EVK USER BTNs
 * Button pressed => bit value 1
 * Button released => bit value 0
 */
uint8 digio_get_input (void)
{
   uint8_t data = 0;

   if (cyhal_gpio_read (CYBSP_USER_BTN1) == BUTTION_PRESSED)
   {
      data |= 0x01;
   }

   if (cyhal_gpio_read (CYBSP_USER_BTN2) == BUTTION_PRESSED)
   {
      data |= 0x02;
   }

   return data;
}

void init_leds (void)
{
   cyhal_gpio_init (
      CYBSP_USER_LED1,
      CYHAL_GPIO_DIR_OUTPUT,
      CYHAL_GPIO_DRIVE_STRONG,
      CYBSP_LED_STATE_OFF);

   cyhal_gpio_init (
      CYBSP_USER_LED2,
      CYHAL_GPIO_DIR_OUTPUT,
      CYHAL_GPIO_DRIVE_STRONG,
      CYBSP_LED_STATE_OFF);

   cyhal_gpio_init (
      CYBSP_USER_LED3,
      CYHAL_GPIO_DIR_OUTPUT,
      CYHAL_GPIO_DRIVE_STRONG,
      CYBSP_LED_STATE_OFF);

   led_events = xEventGroupCreate();

   xTaskCreate (led_task, "LED", 1000, NULL, LED_TASK_PRIORITY, &led_task_hdl);
}

/* In the default DIGIO sample USER BTN 1 and 2 are mapped to input slot bit
 * 0 and 1.
 */
void init_buttons (void)
{
   cyhal_gpio_init (
      CYBSP_USER_BTN1,
      CYHAL_GPIO_DIR_INPUT,
      CYHAL_GPIO_DRIVE_PULLUP,
      CYBSP_BTN_OFF);

   cyhal_gpio_init (
      CYBSP_USER_BTN2,
      CYHAL_GPIO_DIR_INPUT,
      CYHAL_GPIO_DRIVE_PULLUP,
      CYBSP_BTN_OFF);
}

int app_log_output_callback (
   CY_LOG_FACILITY_T facility,
   CY_LOG_LEVEL_T level,
   char * logmsg)
{
   const char * level_str = "";

   (void)facility;

   switch (level)
   {
   case CY_LOG_OFF:
      /* Don't print */
      return 0;
   case CY_LOG_ERR:
      level_str = "[ERROR]";
      break;
   case CY_LOG_WARNING:
      level_str = "[WARN]";
      break;
   case CY_LOG_NOTICE:
      level_str = "[NOTE]";
      break;
   case CY_LOG_INFO:
      level_str = "[INFO]";
      break;
   case CY_LOG_DEBUG:
   default:
      level_str = "[DEBUG]";
      break;
   }

   return printf( "%7s %s", level_str, logmsg);
}

/*
 * uphy lib log function
 */

void os_log_cy (uint8_t type, const char * fmt, ...)
{
   int cy_log_level;
   char log_buf[256];
   va_list list;

   switch (LOG_LEVEL_GET (type))
   {
   case LOG_LEVEL_VERBOSE:
      cy_log_level = CY_LOG_DEBUG1;
      break;
   case LOG_LEVEL_DEBUG:
      cy_log_level = CY_LOG_DEBUG;
      break;
   case LOG_LEVEL_INFO:
      cy_log_level = CY_LOG_INFO;
      break;
   case LOG_LEVEL_WARNING:
      cy_log_level = CY_LOG_WARNING;
      break;
   case LOG_LEVEL_ERROR:
   case LOG_LEVEL_FATAL:
   default:
      cy_log_level = CY_LOG_ERR;
      break;
   }

   va_start (list, fmt);
   vsnprintf (log_buf, sizeof (log_buf), fmt, list);
   va_end (list);

   cy_log_msg (CYLF_MIDDLEWARE, cy_log_level, "%s", log_buf);
}

int main (void)
{
   cy_rslt_t result;

   /* Initialize the device and board peripherals */
   result = cybsp_init();
   if (result != CY_RSLT_SUCCESS)
   {
      CY_ASSERT (0);
   }

   /* Initialize logging  
    * U-Phy messages are identified as CYLF_MIDDLEWARE */
   cy_log_init(CY_LOG_INFO, app_log_output_callback, NULL);

   /* use custom os_log implementation to route it to CY logs */
   os_log = os_log_cy;

   /* Start uart shell console */
   shell_console_init();

   /* Mount filesystem on serial flash */
   fs_init();

   init_leds();

   init_buttons();

   start_demo();

   /* Enable global interrupts */
   __enable_irq();

   /* Start the FreeRTOS scheduler. */
   vTaskStartScheduler();

   /* Should never get here. */
   CY_ASSERT (0);
}

__attribute__ ((noreturn)) void exit (int __status)
{
   printf ("exit called\n");
   led_error();
   CY_HALT();
   while (1)
      vTaskDelay (1000);
};

/* [] END OF FILE */
