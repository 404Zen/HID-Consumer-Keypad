/********************************************************************************************************
* @file     battery.h
* 
* @brief    battery header file
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
#ifndef __BATTERY_H__
#define __BATTERY_H__

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


/* Exported defines ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void battery_init(void);
uint8_t battery_calculate(uint16_t adc_val);


#endif
/******************************************* EOF *****************************************************/
