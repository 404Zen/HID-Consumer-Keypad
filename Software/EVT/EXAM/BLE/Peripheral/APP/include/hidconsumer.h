/********************************** (C) COPYRIGHT *******************************
 * File Name          : hidconsumer.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/13
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef HIDCONSUMER_H
#define HIDCONSUMER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdbool.h>

/*********************************************************************
 * CONSTANTS
 */

// Task Events
#define START_DEVICE_EVT          0x0001
#define START_REPORT_EVT          0x0002
#define START_PARAM_UPDATE_EVT    0x0004
#define START_PHY_UPDATE_EVT      0x0008
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*
 * Task Initialization for the BLE Application
 */
extern void HidEmu_Init(void);

/*
 * Task Event Processor for the BLE Application
 */
extern uint16_t HidEmu_ProcessEvent(uint8_t task_id, uint16_t events);


bool ble_get_connect_state(void);
void ble_hid_consumer_report(uint8_t key_code);
void ble_hid_kbd_report(uint8_t *key_data);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
