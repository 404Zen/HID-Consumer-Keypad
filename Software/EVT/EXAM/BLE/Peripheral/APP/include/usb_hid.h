/********************************************************************************************************
* @file     usb_hid.h
* 
* @brief    usb_hid header file
* 
* @author   404zen@gmail.com
* 
* @date     2023-03-07  
* 
* @version  Ver: 0.1
* 
* @attention 
* 
* None.
* 
*******************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_HID_H__
#define __USB_HID_H__

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void user_usb_hid_init(void);                               /* USB初始化 */
bool get_usb_connected(void);                               /* 获取USB连接状态 */
void usb_hid_kbd_report(uint8_t *data);                     /* 键盘 */
void usb_hid_consumer_report(uint8_t data);                 /* 多媒体 */

#endif
/******************************************* EOF *****************************************************/
