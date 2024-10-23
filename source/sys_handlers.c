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
 * Exception and error handlers.
 */

#include "cybsp.h"
#include "cyhal.h"

/* FreeRTOS header file */
#include <FreeRTOS.h>
#include <event_groups.h>
#include <task.h>

#include <stdio.h>

extern void led_error();

void HardFault_Handler (void)
{
   led_error();
   printf ("HardFault_Handler\n");

#if defined(PRINT_HEAP_USAGE)
   print_heap_usage();
#endif

   CY_HALT();
};

void NMIException_Handler (void)
{
   led_error();
   printf ("NMIException_Handler\n");
   CY_HALT();
};

void MemManage_Handler (void)
{
   led_error();
   printf ("MemManage_Handler\n");
   CY_HALT();
};

void BusFault_Handler (void)
{
   led_error();
   printf ("BusFault_Handler\n");
   CY_HALT();
};

void UsageFault_Handler (void)
{
   led_error();
   printf ("UsageFault_Handler\n");
   CY_HALT();
};

void DebugMon_Handler (void)
{
   led_error();
   printf ("DebugMon_Handler\n");
   CY_HALT();
};

void cy_halt (void)
{
   led_error();
   printf ("*** HALT ! ***\n");
   CY_HALT();
}

void vApplicationMallocFailedHook (void)
{
   led_error();
   taskDISABLE_INTERRUPTS();
   CY_ASSERT (0U != 0U);
   CY_HALT();
   for (;;)
   {
   }
}

void vApplicationStackOverflowHook (TaskHandle_t xTask, char * pcTaskName)
{
   (void)xTask;
   (void)pcTaskName;
   led_error();
   taskDISABLE_INTERRUPTS();
   CY_ASSERT (0U != 0U);
   CY_HALT();
   for (;;)
   {
   }
}
