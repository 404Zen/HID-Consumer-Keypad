/********************************************************************************************************
* @file     vibrator.h
* 
* @brief    vibrator header file
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
#ifndef __VIBRATOR_H__
#define __VIBRATOR_H__

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void vibrator_init(void);                                               /* 初始化 */
void vibrator_start(uint32_t freq, uint8_t duty, uint16_t times);       /*  */


#define VIBR_ENCODER_TURN               vibrator_start(10000, 80, 300)
#define VIBR_ENCODER_PRESS              vibrator_start(200000, 80, 300)
#define VIBR_KEY_SHORT_PRESS            vibrator_start(500000, 80, 250)
#define VIBR_KEY_LONG_PRESS             vibrator_start(200000, 80, 500)

#endif
/******************************************* EOF *****************************************************/
