/********************************************************************************************************
* @file     user_hid_task.h
* 
* @brief    user_hid_task header file
* 
* @author   404zen@gmail.com
* 
* @date     2023-01-02  
* 
* @version  Ver: 0.1
* 
* @attention 
* 
* None.
* 
*******************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_HID_TASK_H__
#define __USER_HID_TASK_H__

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


/* Exported defines ----------------------------------------------------------*/

#define USER_HID_DATA_QUEUE_DEEPTH      32                  /* 按键键值缓存深度 */
#define USER_HID_CONSUMER_QUEUE_DEEPTH  32                  /* 多媒体数据缓存深度 */

#define USER_HID_TASK_1S_PERIOD         0x0001
#define USER_HID_TASK_1MS_PERIOD        0x0002              /* 实际上是1.25ms */
// #define 


#define UHID_CONSUMER_PLAY               0x01
#define UHID_CONSUMER_NEXT               0x02
#define UHID_CONSUMER_LAST               0x04
#define UHID_CONSUMER_MUTE               0x08
#define UHID_CONSUMER_VOL_ADD            0x10
#define UHID_CONSUMER_VOL_DEC            0x20
#define UHID_CONSUMER_FORWARD            0x40
#define UHID_CONSUMER_BACKWARD           0x80


/* Exported types ------------------------------------------------------------*/
typedef struct hid_key_code
{
    uint8_t spec_key;                   /* HID特殊按键 */
    uint8_t resv_byte;
    uint8_t key_val[6];                 /* 普通按键键值 */
}hid_key_code_struct;





/* Exported functions prototypes ---------------------------------------------*/
void user_hid_task_init(void);

/* keyboard */
uint16_t get_hid_data_queue_free_space(void);               /* 获取数据队列剩余空间 */
int8_t hid_queue_push_data(hid_key_code_struct *data);      /* push data to hid queue */
hid_key_code_struct *hid_queue_pop_data(void);              /* pop data from hid queue */

/* consumer */
uint16_t get_hid_consumer_queue_free_space(void);             /* 获取多媒体数据队列剩余空间 */
int8_t hid_consumer_queue_push_data(uint8_t data);          
uint8_t hid_consumer_queue_pop_data(void);

#endif
/******************************************* EOF *****************************************************/
