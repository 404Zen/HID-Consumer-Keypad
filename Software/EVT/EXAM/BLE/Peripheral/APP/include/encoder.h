/********************************************************************************************************
* @file     encoder.h
* 
* @brief    encoder header file
* 
* @author   404zen@gmail.com
* 
* @date     2023-01-12  
* 
* @version  Ver: 0.1
* 
* @attention 
* 
* None.
* 
*******************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ENCODER_H__
#define __ENCODER_H__

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


/* Exported defines ----------------------------------------------------------*/

#define KEY_LONG_PRESS_VALUE            0x80
#define KEY_VALUE_ENC_KEY               0x01
#define KEY_VALUE_KEY_0                 0x02
#define KEY_VALUE_KEY_1                 0x03
#define KEY_VALUE_KEY_2                 0x04
#define KEY_VALUE_ENC_ADD               0x05
#define KEY_VALUE_ENC_DEC               0x06

/* Exported types ------------------------------------------------------------*/




/* Exported functions prototypes ---------------------------------------------*/
void encoder_init(void);
int16_t get_encoder_value(void);

#endif
/******************************************* EOF *****************************************************/
