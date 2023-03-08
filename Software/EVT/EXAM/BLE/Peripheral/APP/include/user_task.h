/********************************************************************************************************
* @file     user_task.h
* 
* @brief    user_task header file
* 
* @author   404zen@gmail.com
* 
* @date     2023-03-05  
* 
* @version  Ver: 0.1
* 
* @attention 
* 
* None.
* 
*******************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_TASK_H__
#define __USER_TASK_H__

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


/* Exported defines ----------------------------------------------------------*/

#define USER_TASK_START_EVENT           0x0001
#define DEVICE_STATE_DETECT_EVENT       0x0002
#define SET_LOWER_MODE_EVENT            0x0004






/* Exported types ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void user_task_init(void);
void user_encoder_trig(void);

#endif
/******************************************* EOF *****************************************************/
