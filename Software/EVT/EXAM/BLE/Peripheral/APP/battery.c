/********************************************************************************************************
* @file     battery.c
* 
* @brief    battery c file
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
/* Includes ------------------------------------------------------------------*/
#include "battery.h"
#include "CH58x_common.h"


/* Defines ------------------------------------------------------------------*/

/* Variables ----------------------------------------------------------------*/
uint16_t adcBuff[40];
volatile uint8_t adclen = 0;
volatile uint8_t DMA_end = 0;
signed short RoughCalib_Value = 0; // ADC粗调偏差值

/* Static functions prototypes ----------------------------------------------*/

/* Functions declears -------------------------------------------------------*/
/**
 * @brief  电池采样初始化
 * @note   None.
 * @param  None.
 * @retval None.
**/
void battery_init(void)
{
    /* PA5 AIN1 */
    GPIOA_ModeCfg(GPIO_Pin_5, GPIO_ModeIN_Floating);
    ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_0);

    RoughCalib_Value = ADC_DataCalib_Rough(); // 用于计算ADC内部偏差，记录到全局变量 RoughCalib_Value中
    PRINT("RoughCalib_Value =%d \n", RoughCalib_Value);

    ADC_ChannelCfg(1);
}


/**
 * @brief  电池电压百分比计算
 * @note   None.
 * @param  None.
 * @retval None.
**/
uint8_t battery_calculate(uint16_t adc_val)
{
    uint32_t adcv_sum = 0;

    if(adc_val != 0)
    {
        adcv_sum = adc_val;
    }
    else
    {  
        for(uint8_t i = 0; i < 20; i++)
        {
            adcBuff[i] = ADC_ExcutSingleConver() + RoughCalib_Value; // 连续采样20次
        }

        /* 电压计算公式， 1050mv * ADC / 2048 */
        for (uint8_t i = 0; i < 20; i++)
        {
            adcv_sum += adcBuff[i];
        }
        adcv_sum /= 20;     /* 算术平均 */
    }

    adcv_sum *= 1050;
    adcv_sum >>= 11;
    // PRINT("Detect vlotage = %d mv\r\n", adcv_sum*2);

    /* adc 测量出来的值为电池电压的一半 */
    if(adcv_sum > 2000)
    {
        return 100;
    }
    else if(adcv_sum < 1744)
    {
        return 0;
    }
    else
    {
        adcv_sum -= 1744;
        adcv_sum *= 100;
        adcv_sum >>= 8;

        return (uint8_t)(adcv_sum);
    }
}


/******************************************* EOF *****************************************************/
