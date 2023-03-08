/********************************************************************************************************
* @file     ws2812.h
* 
* @brief    ws2812 header file
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
#ifndef __WS2812_H__
#define __WS2812_H__

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


/* Exported defines ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
typedef struct 
{
    uint8_t g;
    uint8_t r;
    uint8_t b;
}ws2812_data_buf_t;

#define WS2812_MODE_NORMAL              0x00
#define WS2812_MODE_GRADUAL             0x01



/* Exported functions prototypes ---------------------------------------------*/
void ws2812_init(void);
void ws2812_set_pixel(uint8_t index, uint8_t g, uint8_t r, uint8_t b, uint8_t mode, uint8_t times);


#endif
/******************************************* EOF *****************************************************/
