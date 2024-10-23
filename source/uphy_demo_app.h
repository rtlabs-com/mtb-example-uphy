
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

#ifndef UPHY_DEMO_APP_H_
#define UPHY_DEMO_APP_H_

extern void led_profinet_signal (void);

extern void led_set_running_mode (bool on);

extern void digio_set_output (uint8_t data);

extern uint8_t digio_get_input (void);

extern void start_demo (void);

#endif /* UPHY_DEMO_APP_H_ */
