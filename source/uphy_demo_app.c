/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * http://www.rt-labs.com
 * Copyright 2022 rt-labs AB, Sweden.
 * See LICENSE file in the project root for full license information.
 ********************************************************************/

#include "cyhal.h"
#include "cybsp.h"

#include "options.h"
#include "up_api.h"
#include "up_util.h"
#include "model.h"

#include "uphy_demo_app.h"
#include "shell.h"
#include "rte_fs.h"
#include "network.h"
#include "math.h"
#include "osal.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

/* Enable to run synchronous operation mode */
#define APPLICATION_MODE_SYNCHRONOUS

/* U-Phy callbacks */
static void cb_avail (up_t * up, void * user_arg);
static void cb_sync (up_t * up, void * user_arg);
static void cb_param_write_ind (up_t * up, void * user_arg);
static void cb_status_ind (up_t * up, uint32_t status, void * user_arg);
static void cb_status_ind (up_t * up, uint32_t status, void * user_arg);
static void cb_error_ind (up_t * up, up_error_t error_code, void * user_arg);
static void cb_profinet_signal_led_ind (up_t * up, void * user_arg);
static void cb_loop_ind (up_t * up, void * user_arg);

static up_busconf_t up_busconf;

static up_cfg_t cfg = {
   .device = &up_device,
   .busconf = &up_busconf,
   .vars = up_vars,
   .sync = cb_sync,
   .avail = cb_avail,
   .param_write_ind = cb_param_write_ind,
   .poll_ind = cb_loop_ind,
   .status_ind = cb_status_ind,
   .error_ind = cb_error_ind,
   .profinet_signal_led_ind = cb_profinet_signal_led_ind,
};

/* Use EVK user LEDs and BTNs are only mapped to process data for the DIGIO
 * device. */
static bool is_digio_sample_device = true;

static TaskHandle_t uphy_task_hdl = NULL;

static const char * error_code_to_str (up_error_t error_code)
{
   switch (error_code)
   {
   case UP_ERROR_NONE:
      return "UP_ERROR_NONE";
   case UP_ERROR_CORE_COMMUNICATION:
      return "UP_ERROR_CORE_COMMUNICATION";
   case UP_ERROR_PARAMETER_WRITE:
      return "UP_ERROR_PARAMETER_WRITE";
   case UP_ERROR_PARAMETER_READ:
      return "UP_ERROR_PARAMETER_READ";
   case UP_ERROR_INVALID_PROFINET_MODULE_ID:
      return "UP_ERROR_INVALID_PROFINET_MODULE_ID";
   case UP_ERROR_INVALID_PROFINET_SUBMODULE_ID:
      return "UP_ERROR_INVALID_PROFINET_SUBMODULE_ID";
   case UP_ERROR_INVALID_PROFINET_PARAMETER_INDEX:
      return "UP_ERROR_INVALID_PROFINET_PARAMETER_INDEX";
   default:
      return "UNKNOWN";
   }
}

/*
 * Callback indicating that output data (from PLc) is available.
 * Called every U-Phy cycle.
 */
static void cb_avail (up_t * up, void * user_arg)
{
   up_read_outputs (up);

   /* Apply process data to actual device outputs  */
   if (is_digio_sample_device)
   {
      digio_set_output (up_data.O8.Output_8_bits.value);
   }
}
/*
 * Callback indicating that input data (to PLC) shall be updated.
 * Called every U-Phy cycle.
 */
static void cb_sync (up_t * up, void * user_arg)
{
   if (is_digio_sample_device)
   {
      up_data.I8.Input_8_bits.value = digio_get_input();
   }
   up_write_inputs (up);
}

static void cb_param_write_ind (up_t * up, void * user_arg)
{
   uint16_t slot_ix;
   uint16_t param_ix;
   binary_t data;
   up_param_t * p;

   while (up_param_get_write_req (up, &slot_ix, &param_ix, &data) == 0)
   {
      p = &up_device.slots[slot_ix].params[param_ix];
      memcpy (up_vars[p->ix].value, data.data, data.dataLength);
   }
}

static void cb_status_ind (up_t * up, uint32_t status, void * user_arg)
{
   led_set_running_mode ((bool)(status & UP_CORE_RUNNING));
}

static void cb_error_ind (up_t * up, up_error_t error_code, void * user_arg)
{
   printf (
      "U-Phy Core Error: error_code=%" PRIi16 " %s\n",
      error_code,
      error_code_to_str (error_code));
}

static void cb_profinet_signal_led_ind (up_t * up, void * user_arg)
{
   led_profinet_signal();
}

static void cb_loop_ind (up_t * up, void * user_arg)
{
#if !defined(APPLICATION_MODE_SYNCHRONOUS)
   up_write_inputs (up);
   up_read_outputs (up);
#endif
}

void up_app_main (up_t * up)
{
   if (up_init_device (up) != 0)
   {
      printf ("Failed to configure device\n");
      exit (EXIT_FAILURE);
   }

   if (up_util_init (&up_device, up, up_vars) != 0)
   {
      printf ("Failed to init up utils\n");
      exit (EXIT_FAILURE);
   }

   if (up_start_device (up) != 0)
   {
      printf ("Failed to start device\n");
      exit (EXIT_FAILURE);
   }

#if defined(APPLICATION_MODE_SYNCHRONOUS)
   if (up_write_event_mask(up, UP_EVENT_MASK_SYNCHRONOUS_MODE) != 0)
   {
      printf ("Failed to write eventmask mode\n");
      exit (EXIT_FAILURE);
   }
#endif

   /* Write input signals to set initial values and status */
   up_write_inputs (up);

   printf ("Run event loop\n");

   while (up_worker (up) == true)
      ;

   printf ("Unexpected error in U-Phy library.\n");
   printf ("Restart device\n");
}

up_t * up_app_init (up_bustype_t bustype)
{
   up_t * up;

   /* User LEDs and buttons are only mapped to process io data for the default
    * DIGIO sample
    */
   if (strcmp (cfg.device->name, "U-Phy DIGIO Sample") != 0)
   {
      is_digio_sample_device = false;
   }

   cfg.device->bustype = bustype;

   switch (bustype)
   {
   case UP_BUSTYPE_MOCK:
      up_busconf.mock = up_mock_config;
      break;
   case UP_BUSTYPE_PROFINET:
      up_busconf.profinet = up_profinet_config;
      break;
   case UP_BUSTYPE_ETHERNETIP:
      up_busconf.ethernetip = up_ethernetip_config;
      break;
   case UP_BUSTYPE_MODBUS:
   case UP_BUSTYPE_ECAT:
   default:
      printf ("Bustype %d unsupported\n", bustype);
      break;
   }

   extern void up_core_init (void); // TBD - why not in header-file
   up_core_init();

   up = up_init (&cfg);

   return up;
}

void uphy_task (void * type)
{
   up_t * up;
   up_bustype_t bustype = (up_bustype_t)type;
   cy_rslt_t result;
   ip_config_t ip_config;

   if (bustype == UP_BUSTYPE_PROFINET)
   {
      /* Profinet stack will handle IP addresses */
      ip_config = IP_CONFIG_STATIC;
   }
   else
   {
      /* Use DHCP */
      ip_config = IP_CONFIG_DYNAMIC;
   }

   printf ("Init network\n");
   printf ("Application will hang until ethernet cable is inserted\n");

   result = connect_to_ethernet (ip_config);
   if (result != CY_RSLT_SUCCESS)
   {
      printf (
         "\n Failed to connect to the ethernet network! Error code: "
         "0x%08" PRIx32 "\n",
         (uint32_t)result);
      CY_ASSERT (0);
   }

   printf ("Ethernet connected.\n");
   printf ("Starting U-Phy Demo\n");
   printf ("Active device model: \"%s\"\n", cfg.device->name);

   printf ("Init U-Phy Device \n");
   up = up_app_init (bustype);

   printf ("Run U-Phy Device \n");
   up_app_main (up);

   printf ("Error - unexpected return from up_app_main()\n");
   CY_ASSERT (0);
}

static int str_to_bus_config (const char * str, up_bustype_t * bustype)
{
   if (str == NULL || bustype == NULL)
   {
      return -1;
   }

#if UP_DEVICE_PROFINET_SUPPORTED
   if (strcmp (str, "profinet") == 0)
   {
      *bustype = UP_BUSTYPE_PROFINET;
      return 0;
   }
#endif

#if UP_DEVICE_ETHERCAT_SUPPORTED
   if (strcmp (str, "ethercat") == 0)
   {
      *bustype = UP_BUSTYPE_ECAT;
      return 0;
   }
#endif

#if UP_DEVICE_ETHERNETIP_SUPPORTED
   if (strcmp (str, "ethernetip") == 0)
   {
      *bustype = UP_BUSTYPE_ETHERNETIP;
      return 0;
   }
#endif

#if UP_DEVICE_MODBUS_SUPPORTED
   if (strcmp (str, "modbus") == 0)
   {
      *bustype = UP_BUSTYPE_MODBUS;
      return 0;
   }
#endif

   if (strcmp (str, "mock") == 0)
   {
      *bustype = UP_BUSTYPE_MOCK;
      return 0;
   }

   printf ("Unsupported fieldbus \"%s\". Is it supported by device model?\n", str);
   return -1;
}

/**
 * Read auto start configuration file.
 *
 * @param bustype pointer to bustype to be filled in
 *
 * @return 0 if autostart is enabled, -1 if not
 */
int auto_start (up_bustype_t * bustype)
{
   char buf[32];
   RTE_FILE * f = rte_fs_fopen (STORAGE_ROOT "autostart", "r");
   if (f > 0)
   {
      rte_fs_fread (buf, 1, sizeof (buf), f);
      rte_fs_fclose (f);

      if (str_to_bus_config (buf, bustype) == 0)
      {
         printf ("Autostart enabled - %s\n", buf);
         return 0;
      }
   }
   else
   {
      printf (
         "Autostart disabled, start U-Phy using console command 'up_start'\n");
   }
   return -1;
}
static void start_uphy (up_bustype_t bustype)
{
   /* Only allow starting uphy once */
   if (uphy_task_hdl == NULL)
   {
      xTaskCreate (
         uphy_task,
         "uphy_task",
         5000,
         (void *)bustype,
         OS_PRIORITY_HIGH,
         &uphy_task_hdl);
   }
}

void start_demo (void)
{
   up_bustype_t bustype;

   /* if autostart is configured, automatically start u-phy
    * if not wait for shell console command */
   if (auto_start (&bustype) == 0)
   {
      start_uphy (bustype);
   }
}

void shell_print_start_banner (void)
{
   const char DELIMITER_STR[] =
      "----------------------------------------------------------------------";
   /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
   printf ("\x1b[2J\x1b[;H");
   printf ("\n%s\n", DELIMITER_STR);
   printf (" Industrial Ethernet Demo\n");
   printf (" Configure communication protocol using this shell. Usage:\n");
   printf ("  'help'       - list of available commands\n");
   printf ("  'help <cmd>' - detailed help\n");
   printf ("  'about'      - for information on this application\n\n");
   printf (" Built : %s\n", __DATE__ " at " __TIME__);
   printf (" U-Phy : %s\n", up_version());
   printf ("%s\n\n", DELIMITER_STR);
}

int _cmd_about (int argc, char * argv[])
{
   printf ("\r\nIndustrial Ethernet Demo\r\n\r\n");
   printf ("This user example shows how to implement Industrial Ethernet\r\n"
           "connectivity using the U-Phy Middleware. Currently Profinet and\r\n"
           "Ethernet/IP is supported.\r\n\r\n"
           "The application implements a basic I/O device connecting inputs\r\n"
           "and outputs to the EVK buttons and LEDs.\r\n\r\n"
           "Profinet GSDML and EtherNet/IP EDS files for integration in an\r\n"
           "engineering tool are found in the `generated/` folder.\r\n\r\n"
           "Start communication using 'up_start' command.\r\n");
   return 0;
}

const shell_cmd_t cmd_about = {
   .cmd = _cmd_about,
   .name = "about",
   .help_short = "about this application",
   .help_long = "Print information about this application"};

SHELL_CMD (cmd_about);

int _cmd_show_device (int argc, char * argv[])
{
   printf ("\"%s\"\r\n", up_device.name);

   for (int i = 0; i < up_device.n_slots; i++)
   {
      up_slot_t slot = up_device.slots[i];
      printf ("Slot[%d]: %s\r\n", i + 1, slot.name);

      if (slot.n_inputs > 0)
      {
         for (int j = 0; j < slot.n_inputs; j++)
         {
            printf ("   [in] %s\r\n", slot.inputs[j].name);
         }
      }

      if (slot.n_outputs > 0)
      {
         for (int j = 0; j < slot.n_outputs; j++)
         {
            printf ("  [out] %s\r\n", slot.outputs[j].name);
         }
      }

      if (slot.n_params > 0)
      {
         for (int j = 0; j < slot.n_params; j++)
         {
            printf ("  [par] %s\r\n", slot.params[j].name);
         }
      }
   }
   return 0;
}

const shell_cmd_t cmd_show_device = {
   .cmd = _cmd_show_device,
   .name = "up_device",
   .help_short = "show static device configuration",
   .help_long = "Show static information from the device model."};

SHELL_CMD (cmd_show_device);

int _cmd_start (int argc, char * argv[])
{
   char * fieldbus;
   up_bustype_t bustype;

   /* Check command line arguments */
   if (argc != 2)
   {
      printf ("error - try \"help %s\n", argv[0]);
      return -1;
   }

   fieldbus = argv[1];

   if (str_to_bus_config (fieldbus, &bustype) == 0)
   {
      start_uphy (bustype);
   }

   return 0;
}

const shell_cmd_t cmd_start = {
   .cmd = _cmd_start,
   .name = "up_start",
   .help_short = "start u-phy protocol",
   .help_long = "Start u-phy host device.\n"
                "Usage: up_start <protocol>\n"
                "where protocol can be profinet, ethernetip or mock\n"};

SHELL_CMD (cmd_start);

int _cmd_autostart (int argc, char * argv[])
{
   up_bustype_t bustype;
   char * fieldbus;

   if (argc != 2)
   {
      printf ("error - try \"help %s\n", argv[0]);
      return -1;
   }

   fieldbus = argv[1];
   if ((argc == 2) && (str_to_bus_config (fieldbus, &bustype) == 0))
   {
      RTE_FILE * f = rte_fs_fopen (STORAGE_ROOT "autostart", "w");
      rte_fs_fwrite (fieldbus, 1, strlen (fieldbus) + 1, f);
      rte_fs_fclose (f);

      printf ("%s autostart added\n", fieldbus);
   }
   else
   {
      printf ("autostart disabled\n");

      rte_fs_remove (STORAGE_ROOT "autostart");
      return -1;
   }

   return 0;
}

const shell_cmd_t cmd_autostart = {
   .cmd = _cmd_autostart,
   .name = "up_autostart",
   .help_short = "configure u-phy device autostart",
   .help_long = "Set u-phy autostart configuration \n"
                "Usage: up_autostart <protocol>\n"
                "where protocol can be profinet | ethernetip  | mock.\n"
                "If no valid protocol is given, autostart is disabled."};

SHELL_CMD (cmd_autostart);
