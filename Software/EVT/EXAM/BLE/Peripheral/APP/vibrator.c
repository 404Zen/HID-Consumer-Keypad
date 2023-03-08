/********************************************************************************************************
* @file     vibrator.c
* 
* @brief    vibrator c file
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
#include "vibrator.h"
#include "CH58x_common.h"
#include "CH58xBLE_LIB.h"


/* Defines ------------------------------------------------------------------*/

#define VIBRATOR_STOP_EVENT             0x0001

/* Variables ----------------------------------------------------------------*/

static uint8_t vibrator_task_id = 0;

/* Static functions prototypes ----------------------------------------------*/

static uint16_t vibrator_event_process(uint8_t task_id, uint16_t events);

/* Functions declears -------------------------------------------------------*/
/**
 * @brief  初始化
 * @note   None.
 * @param  None.
 * @retval None.
**/
void vibrator_init(void)
{
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeOut_PP_5mA);    // PA12 - PWM4

    // PWMX_CLKCfg(6);                                   // cycle = 4/Fsys
    // PWMX_CycleCfg(PWMX_Cycle_64);                     // 周期 = 64*cycle
    // PWMX_ACTOUT(CH_PWM4, 64 / 2, High_Level, ENABLE);  // 25% 占空比

    // PWMX_ACTOUT(CH_PWM4, 64 / 4, Low_Level, DISABLE);  // 25% 占空比

    vibrator_task_id = TMOS_ProcessEventRegister(vibrator_event_process);
}

/**
 * @brief  启动振子, 误差很大，可以考虑改用定时器(需要改硬件)
 * @note   None.
 * @param  freq 频率. 使用 CH58x 的 PWM 模块限制较多, 只能逼近所要求的频率与占空比
 * @param  duty 占空比 0-100 
 * @param  times 持续事件
 * @retval None.
**/
void vibrator_start(uint32_t freq, uint8_t duty, uint16_t times)
{
    uint8_t da = 0;
    uint8_t clk_div = 0;
    uint32_t cycle = 0;
    PWMX_CycleTypeDef pwm_cycle = 0;
    uint16_t cycle_nums = 0;
    uint32_t remain_min = 256;
    uint32_t remain_buf[4];


    cycle = (uint32_t)(60000000 / freq);    

    remain_buf[0] = cycle % 32;     if(remain_buf[0] < remain_min) remain_min = remain_buf[0];
    remain_buf[1] = cycle % 64;     if(remain_buf[1] < remain_min) remain_min = remain_buf[1];
    remain_buf[2] = cycle % 128;    if(remain_buf[2] < remain_min) remain_min = remain_buf[2];
    remain_buf[3] = cycle % 256;    if(remain_buf[3] < remain_min) remain_min = remain_buf[3];

    for (uint8_t i = 0; i < sizeof(remain_buf); i++)
    {
        if(remain_buf[i] == remain_min)
        {
            if(i == 0)  {pwm_cycle = PWMX_Cycle_32; cycle_nums = 32;  }
            if(i == 1)  {pwm_cycle = PWMX_Cycle_64; cycle_nums = 64;  }
            if(i == 2)  {pwm_cycle = PWMX_Cycle_128;cycle_nums = 128; }
            if(i == 3)  {pwm_cycle = PWMX_Cycle_256;cycle_nums = 256; }
            break;
        }
    }

    clk_div = (uint8_t)(cycle / cycle_nums);
    da = cycle_nums * duty / 100;

    PWMX_CLKCfg(clk_div);
    PWMX_CycleCfg(pwm_cycle);
    PWMX_ACTOUT(CH_PWM4, da, High_Level, ENABLE);

    tmos_start_task(vibrator_task_id, VIBRATOR_STOP_EVENT, MS1_TO_SYSTEM_TIME(times));
}

/**
 * @brief  事件处理函数
 * @note   None.
 * @param  None.
 * @retval None.
**/
static uint16_t vibrator_event_process(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *p_msg;
        if( (p_msg = tmos_msg_receive(vibrator_task_id)) != NULL)
        {
            tmos_msg_deallocate(p_msg);
        }

        return (events ^ SYS_EVENT_MSG);
    }

    if(events & VIBRATOR_STOP_EVENT)
    {
        PWMX_ACTOUT(CH_PWM4, 0, High_Level, DISABLE);
        return (events ^ VIBRATOR_STOP_EVENT);
    }


    return 0;
}


/******************************************* EOF *****************************************************/
