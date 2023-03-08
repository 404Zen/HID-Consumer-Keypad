/********************************************************************************************************
* @file     user_task.c
* 
* @brief    user_task c file
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
/* Includes ------------------------------------------------------------------*/
#include "user_task.h"
#include "CONFIG.h"

#include "hidconsumer.h"
#include "hiddev.h"
#include "usb_hid.h"

/* 外设驱动 */
#include "encoder.h"



/* Defines ------------------------------------------------------------------*/

/* Variables ----------------------------------------------------------------*/

static uint8_t user_task_id = 0;

/* Global variables */
bool g_usb_addr_state;
bool g_ble_conn_state;


/* Static functions prototypes ----------------------------------------------*/

static uint16_t user_task_event_process(uint8_t task_id, uint16_t events);



/* Functions declears -------------------------------------------------------*/
/**
 * @brief  用户任务初始化
 * @note   None.
 * @param  None.
 * @retval None.
**/
void user_task_init(void)
{
    user_task_id = TMOS_ProcessEventRegister(user_task_event_process);

    tmos_set_event(user_task_id, USER_TASK_START_EVENT);
    tmos_start_reload_task(user_task_id, DEVICE_STATE_DETECT_EVENT, MS1_TO_SYSTEM_TIME(500));     /* 500MS */
}


/**
 * @brief  用户事件处理函数
 * @note   None.
 * @param  None.
 * @retval None.
**/
static uint16_t user_task_event_process(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *p_msg;
        if( (p_msg = tmos_msg_receive(user_task_id)) != NULL)
        {
            tmos_msg_deallocate(p_msg);
        }

        return (events ^ SYS_EVENT_MSG);
    }

    if(events & USER_TASK_START_EVENT)
    {
        g_usb_addr_state = get_usb_connected();
        g_ble_conn_state = ble_get_connect_state();

        PRINT("USER_TASK_START_EVENT\r\n");
        return (events ^ USER_TASK_START_EVENT);
    }

    if(events & DEVICE_STATE_DETECT_EVENT)
    {
        /* 定时扫描USB连接状态以及蓝牙状态 */
        bool usb_addr_state = get_usb_connected();
        bool ble_conn_state = ble_get_connect_state();

        if(g_usb_addr_state != usb_addr_state)
        {
            g_usb_addr_state = usb_addr_state;
            PRINT("USB state change, New state = %d\r\n", usb_addr_state);

            if(g_usb_addr_state == true)
            {
                tmos_clear_event(user_task_id, SET_LOWER_MODE_EVENT);
            }
            else
            {
                PRINT("Go to low power mode after 10s\r\n");
                tmos_start_task(user_task_id, SET_LOWER_MODE_EVENT, MS1_TO_SYSTEM_TIME(1000*10));
            }
        }

        if(g_ble_conn_state != ble_conn_state)
        {
            g_ble_conn_state = ble_conn_state;
            PRINT("BLE state change, New state = %d\r\n", ble_conn_state);
        }

        return (events ^ DEVICE_STATE_DETECT_EVENT);
    }

    if(events & SET_LOWER_MODE_EVENT)
    {
        /* 开发板上不带外部测试3V3 电流低于100uA, DinoKeypad V1.1 测试电流约4mA,需要先修正硬件 */
        PRINT("SET_LOWER_MODE_EVENT\r\n");
        PWR_PeriphWakeUpCfg(ENABLE, RB_SLP_GPIO_WAKE, Long_Delay);
        LowPower_Sleep(RB_PWR_RAM30K | RB_PWR_RAM2K | RB_PWR_EXTEND);   //只保留30+2K SRAM USB 以及 WIFI 供电
        HSECFG_Current(HSE_RCur_100);                                   // 降为额定电流
        DelayMs(10);
        PRINT("User wake up\r\n");

        return (events ^ SET_LOWER_MODE_EVENT);
    }

    return 0;
}





/******************************************* EOF *****************************************************/
