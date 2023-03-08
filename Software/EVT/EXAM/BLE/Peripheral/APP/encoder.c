/********************************************************************************************************
* @file     encoder.c
* 
* @brief    encoder c file
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
/* Includes ------------------------------------------------------------------*/
#include "encoder.h"

#include "config.h"
#include "CH58x_common.h"
#include <stdio.h>

#include "hidconsumer.h"
#include "hiddev.h"

#include "usb_hid.h"

#include "vibrator.h"

/* USER_TASK */
#include "user_task.h"
#include "user_hid_task.h"


/* Defines ------------------------------------------------------------------*/

/* TASK EVENT DEFINE */
#define ENCODER_TRIG_EVENT              0x0001
#define ENC_KEY_TRIG_EVENT              0x0002
#define KEY_PROCESS_EVENT               0x0004

/* KEY VALUE BUF SIZE */
#define KEY_VALUE_BUF_SIZE              64


#define KEY_FELTER_TIME                 2
#define KEY_SHORT_PRESS_TIME            50
#define KEY_LONG_PRESS_TIME             2000


typedef enum
{
    KEY_FREE = 0,
    KEY_TRIG,
    KEY_PRESS_FILTER,
    KEY_SHORT_PRESSED,
    KEY_SHORT_PRESS_RELEASE,
    KEY_LONG_PRESSED,
    KEY_LONG_PRESS_RELEASE,
    KEY_RELEASE_FILTER,
}key_state_t;

typedef struct
{
    uint32_t    key_port;
    uint32_t    key_io;
    uint8_t     active_level;
    key_state_t key_state;
    uint8_t     key_value;
    uint32_t    ts;
    uint32_t    rel_ts;
    int (*key_callback_func)(uint8_t key_val);
}general_key_t;

/* Variables ----------------------------------------------------------------*/

volatile int16_t encoder_value = 0;
static uint8_t encoder_task_id = 0;


/* Static functions prototypes ----------------------------------------------*/
static void encoder_interrupt_handler(void);
static uint16_t encoder_task_event_process(uint8_t task_id, uint16_t events);
static void key_irq_trig(uint8_t key_index);
static void key_scan_task(uint8_t period_tick);
static void encoder_scan_task(void);
static int key_callback(uint8_t key_value);
uint32_t get_io_level(general_key_t *key);


general_key_t key_table[] = {
    {(uint32_t)BA_PB, GPIO_Pin_13, 0, KEY_FREE, KEY_VALUE_ENC_KEY, 0, 0, key_callback},             /* ENC_KEY */
    {(uint32_t)BA_PA, GPIO_Pin_13, 0, KEY_FREE, KEY_VALUE_KEY_0, 0, 0, key_callback},               /* KEY_0 */
    {(uint32_t)BA_PA, GPIO_Pin_15, 0, KEY_FREE, KEY_VALUE_KEY_1, 0, 0, key_callback},               /* KEY_1 */
    {(uint32_t)BA_PB, GPIO_Pin_4, 0, KEY_FREE, KEY_VALUE_KEY_2, 0, 0, key_callback},                /* KEY_2 */
};

typedef enum{
    KEY_ENC_IDX = 0,
    KEY_0_IDX,
    KEY_1_IDX,
    KEY_2_IDX,
}key_index_t;




/* Functions declears -------------------------------------------------------*/
/**
 * @brief  编码器 初始化 IO
 * @note   None.
 * @param  None.
 * @retval None.
**/
void encoder_init(void)
{
    /* 编码器 */
    GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_PU);

    /* CH58x 只能实现上升沿或下降沿中断，无法实现边沿触发 */
    GPIOB_ITModeCfg(GPIO_Pin_7, GPIO_ITMode_FallEdge);


    for (size_t i = 0; i < ((sizeof(key_table))/(sizeof(key_table[0]))); i++)
    {
        if(key_table[i].active_level == 0)
        {
            /* 使能下降沿触发中断 */
            if(key_table[i].key_port == (uint32_t)BA_PA)
            {
                GPIOA_ModeCfg(key_table[i].key_io, GPIO_ModeIN_PU);
                GPIOA_ITModeCfg(key_table[i].key_io, GPIO_ITMode_FallEdge);
            }
            else if(key_table[i].key_port == (uint32_t)BA_PB)
            {
                GPIOB_ModeCfg(key_table[i].key_io, GPIO_ModeIN_PU);
                GPIOB_ITModeCfg(key_table[i].key_io, GPIO_ITMode_FallEdge);
            }
        }
        else
        {
            /* 使能上升降沿触发中断 */
            if(key_table[i].key_port == (uint32_t)BA_PA)
            {
                GPIOA_ModeCfg(key_table[i].key_io, GPIO_ModeIN_PD);
                GPIOA_ITModeCfg(key_table[i].key_io, GPIO_ITMode_RiseEdge);
            }
            else if(key_table[i].key_port == (uint32_t)BA_PB)
            {
                GPIOB_ModeCfg(key_table[i].key_io, GPIO_ModeIN_PD);
                GPIOB_ITModeCfg(key_table[i].key_io, GPIO_ITMode_RiseEdge);
            }
        }
    }
    
    encoder_task_id = TMOS_ProcessEventRegister(encoder_task_event_process);
    tmos_start_reload_task(encoder_task_id, KEY_PROCESS_EVENT, 1);              /* 1 tick = 625us */

    /* 开启中断 */
    PFIC_EnableIRQ(GPIO_A_IRQn);
    PFIC_EnableIRQ(GPIO_B_IRQn);
}

/**
 * @brief  编码器 按键 事件处理
 * @note   None.
 * @param  None.
 * @retval None.
**/
static uint16_t encoder_task_event_process(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *p_msg;
        if( (p_msg = tmos_msg_receive(encoder_task_id)) != NULL)
        {
            tmos_msg_deallocate(p_msg);
        }

        return (events ^ SYS_EVENT_MSG);
    }

    if (events & ENCODER_TRIG_EVENT)
    {
        encoder_interrupt_handler();
        return (events ^ ENCODER_TRIG_EVENT);
    }

    if (events & ENC_KEY_TRIG_EVENT)
    {
        if(GPIOB_ReadPortPin(GPIO_Pin_7) == 0)
        {
            PRINT("ENC KEY TRIG\r\n");
        }

        return (events ^ ENC_KEY_TRIG_EVENT);
    }

    if (events & KEY_PROCESS_EVENT)
    {
        key_scan_task(1);
        encoder_scan_task();
        return (events ^ KEY_PROCESS_EVENT);
    }
    
    return 0;
}

/**
 * @brief  GPIOA 中断服务函数
 * @note   None.
 * @param  None.
 * @retval None.
**/
__INTERRUPT
__HIGH_CODE
void GPIOA_IRQHandler(void)
{
    if(GPIOA_ReadITFlagBit(GPIO_Pin_13) != 0)
    {
        /* KEY_1 */
        key_irq_trig(KEY_0_IDX);
        GPIOA_ClearITFlagBit(GPIO_Pin_13);
    }

    if(GPIOA_ReadITFlagBit(GPIO_Pin_15)!= 0)
    {
        /* KEY_2 */
        key_irq_trig(KEY_1_IDX);
        GPIOA_ClearITFlagBit(GPIO_Pin_15);
    }
}

/**
 * @brief  GPIOB 中断服务函数
 * @note   None.
 * @param  None.
 * @retval None.
**/
__INTERRUPT
__HIGH_CODE
void GPIOB_IRQHandler(void)
{
    if(GPIOB_ReadITFlagBit(GPIO_Pin_7) != 0)
    {
        tmos_start_task(encoder_task_id, ENCODER_TRIG_EVENT, MS1_TO_SYSTEM_TIME(1));
        GPIOB_ClearITFlagBit(GPIO_Pin_7);
    }

    if(GPIOB_ReadITFlagBit(GPIO_Pin_4)!= 0)
    {
        /* KEY_3 */
        key_irq_trig(KEY_2_IDX);
        GPIOB_ClearITFlagBit(GPIO_Pin_4);
    }

    if(GPIOB_ReadITFlagBit(GPIO_Pin_13) != 0)
    {   
        /* ENC_KEY */
        // tmos_start_task(encoder_task_id, ENC_KEY_TRIG_EVENT, MS1_TO_SYSTEM_TIME(1));
        key_irq_trig(KEY_ENC_IDX);
        GPIOB_ClearITFlagBit(GPIO_Pin_13);
    }
}

/**
 * @brief  编码器中断处理
 * @note   None.
 * @param  None.
 * @retval None.
**/
static void encoder_interrupt_handler(void)
{
    // DelayUs(1);
    /* 保证PB7是低电平再检测PB12, 硬件上需要加电容滤波，根据板子确定电容值 */
    if(GPIOB_ReadPortPin(GPIO_Pin_7) == 0)
    {
        if(GPIOB_ReadPortPin(GPIO_Pin_12))                  /* 高电平 正转 */
            encoder_value++;
        else                                                /* 低电平 反转 */
            encoder_value--;
    }
    else
    {
        /* PB7 高电平， 上升沿触发 */
        if(GPIOB_ReadPortPin(GPIO_Pin_12))                  /* PB22 高电平， 反转 */
            encoder_value--;
        else                                                /* PB22 低电平， 正转 */
            encoder_value++;
    }
}

/**
 * @brief  获取编码器的值
 * @note   None.
 * @param  None.
 * @retval None.
**/
int16_t get_encoder_value(void)
{
    int16_t ret = encoder_value;
    encoder_value = 0;
    return ret;
}

/**
 * @brief  按键触发
 * @note   None.
 * @param  None.
 * @retval None.
**/
static void key_irq_trig(uint8_t key_index)
{
    if(key_table[key_index].key_state == KEY_FREE)
        key_table[key_index].key_state++;
}

/**
 * @brief  按键扫描任务
 * @note   None.
 * @param  None.
 * @retval None.
**/
static void key_scan_task(uint8_t period_tick)
{
    static uint32_t tick = 0;
    bool io_state = false;
    tick += period_tick;

    for (uint8_t i = 0; i < (sizeof(key_table)/sizeof(key_table[0])); i++)
    {
        switch (key_table[i].key_state)
        {
        case KEY_FREE:
            /* code */
            break;

        case KEY_TRIG:
            io_state = get_io_level(&key_table[i]);
            if(io_state == key_table[i].active_level)
            {
                key_table[i].ts = tick;
                key_table[i].key_state = KEY_PRESS_FILTER;
            }
            break;

        case KEY_PRESS_FILTER:
            io_state = get_io_level(&key_table[i]);
            if(io_state == key_table[i].active_level)
            {
                if(tick - key_table[i].ts >= KEY_FELTER_TIME)
                {
                    key_table[i].ts = tick;
                    key_table[i].key_state = KEY_SHORT_PRESSED;
                }
            }
            else
            {
                key_table[i].key_state = KEY_FREE;
            }
            break;

        case KEY_SHORT_PRESSED:
            io_state = get_io_level(&key_table[i]);
            if(io_state == key_table[i].active_level)
            {
                if(tick - key_table[i].ts >= KEY_SHORT_PRESS_TIME)
                {
                    key_table[i].key_state = KEY_LONG_PRESSED;
                }
            }
            else
            {
                key_table[i].key_state = KEY_SHORT_PRESS_RELEASE;
                key_table[i].rel_ts = tick;
            }
            break;

        case KEY_SHORT_PRESS_RELEASE:
            io_state = get_io_level(&key_table[i]);
            if(io_state == key_table[i].active_level)
            {
                key_table[i].key_state = KEY_SHORT_PRESSED;
            }
            else
            {
                if (tick - key_table[i].rel_ts >= KEY_FELTER_TIME)
                {
                    key_table[i].key_state = KEY_FREE;
                    
                    if(tick - key_table[i].ts >= KEY_SHORT_PRESS_TIME)
                    {
                        if(key_table[i].key_callback_func != NULL)
                        {
                            key_table[i].key_callback_func(key_table[i].key_value);
                        }
                    }
                }
                
            }
            break;
        
        case KEY_LONG_PRESSED:
            io_state = get_io_level(&key_table[i]);
            if(io_state == key_table[i].active_level)
            {
                if(tick - key_table[i].ts >= KEY_LONG_PRESS_TIME)
                {
                    key_table[i].key_state = KEY_LONG_PRESS_RELEASE;            /* 需要连发长按可以注释本行 */
                    if(key_table[i].key_callback_func!= NULL)
                    {
                        key_table[i].key_callback_func(key_table[i].key_value | KEY_LONG_PRESS_VALUE);
                    }
                }
            }
            else
            {
                if(tick - key_table[i].ts >= KEY_LONG_PRESS_TIME)
                {
                    key_table[i].key_state = KEY_LONG_PRESS_RELEASE;
                }
                else
                {
                    key_table[i].key_state = KEY_SHORT_PRESS_RELEASE;
                }
                key_table[i].rel_ts = tick;
            }
            break;

        case KEY_LONG_PRESS_RELEASE:
            io_state = get_io_level(&key_table[i]);
            if(io_state == key_table[i].active_level)
            {
                /* Do nothing, wait for user release */
                key_table[i].rel_ts = tick + 1;
                // key_table[i].key_state = KEY_LONG_PRESSED;
            }
            else
            {
                if(tick - key_table[i].rel_ts >= KEY_FELTER_TIME)
                {
                    key_table[i].key_state = KEY_FREE;
                }
            }
            break;

        default:
            break;
        }
    }
}

/**
 * @brief  编码器扫描
 * @note   None.
 * @param  None.
 * @retval None.
**/
static void encoder_scan_task(void)
{
    int16_t encoder_value = get_encoder_value();
    if(encoder_value != 0)
    {
        if(encoder_value > 0)
        {
            for (int16_t i = 0; i < encoder_value; i++)
            {
                key_callback(KEY_VALUE_ENC_ADD);
            }
        }
        else
        {
            for (int16_t i = 0; i > encoder_value; i--)
            {
                key_callback(KEY_VALUE_ENC_DEC);
            }
        }
    }
}

/**
 * @brief  读取按键电平
 * @note   None.
 * @param  None.
 * @retval 0-low level, Others-High level.
**/
uint32_t get_io_level(general_key_t *key)
{
    uint32_t io_state = 0;

    #if 1   /* CH57x IO level read */
    if(key->key_port == (uint32_t)BA_PA)   /* Port A */
    {
        io_state = GPIOA_ReadPortPin(key->key_io);
    }
    else if(key->key_port == (uint32_t)BA_PB)  /* Port B */
    {
        io_state = GPIOB_ReadPortPin(key->key_io);
    }
    #endif

    return io_state;
}

/**
 * @brief  按键回调函数
 * @note   None.
 * @param  None.
 * @retval None.
**/
static int key_callback(uint8_t key_value)
{
    // PRINT("key_callback, key value = 0x%02X\r\n", key_value);
    static uint8_t kbd_data_buf[8];
    hid_key_code_struct hid_key_code;

    memset(&hid_key_code, 0, sizeof(hid_key_code_struct));
    
    switch (key_value)
    {
    case KEY_VALUE_ENC_KEY:
        VIBR_ENCODER_PRESS;
        if(get_usb_connected())
        {
            hid_consumer_queue_push_data(HID_CONSUMER_MUTE);
            break;
        }
        if(ble_get_connect_state())
        {
            ble_hid_consumer_report(HID_CONSUMER_MUTE);
            ble_hid_consumer_report(0x00);
            break;
        }
        break;

    case KEY_VALUE_ENC_KEY | KEY_LONG_PRESS_VALUE:
        break;

    case KEY_VALUE_KEY_0:
        VIBR_KEY_SHORT_PRESS;
        if(get_usb_connected())
        {
            hid_consumer_queue_push_data(HID_CONSUMER_SCAN_PREV_TRK);
            break;
        }
        if(ble_get_connect_state())
        {
            ble_hid_consumer_report(HID_CONSUMER_SCAN_PREV_TRK);
            ble_hid_consumer_report(0x00);
            break;
        }
        break;

    case KEY_VALUE_KEY_0 | KEY_LONG_PRESS_VALUE:
        break;

    case KEY_VALUE_KEY_1:
        VIBR_KEY_SHORT_PRESS;
        if(get_usb_connected())
        {
            hid_consumer_queue_push_data(HID_CONSUMER_PLAY_PAUSE);
            break;
        }
        if(ble_get_connect_state())
        {
            ble_hid_consumer_report(HID_CONSUMER_PLAY_PAUSE);
            ble_hid_consumer_report(0x00);
            break;
        }
        break;

    case KEY_VALUE_KEY_1 | KEY_LONG_PRESS_VALUE:
        break;

    case KEY_VALUE_KEY_2:
        VIBR_KEY_SHORT_PRESS;
        if(get_usb_connected())
        {
            hid_consumer_queue_push_data(HID_CONSUMER_SCAN_NEXT_TRK);
            break;
        }
        if(ble_get_connect_state())
        {
            ble_hid_consumer_report(HID_CONSUMER_SCAN_NEXT_TRK);
            ble_hid_consumer_report(0x00);
            break;
        }
        break;

    case KEY_VALUE_KEY_2 | KEY_LONG_PRESS_VALUE:
        VIBR_KEY_LONG_PRESS;
        kbd_data_buf[0] = 0x01 | 0x04;
        kbd_data_buf[2] = HID_KEYBOARD_L;
        if(get_usb_connected())
        {
            hid_queue_push_data((hid_key_code_struct *)&kbd_data_buf);
            break;
        }
        if(ble_get_connect_state())
        {
            ble_hid_kbd_report(kbd_data_buf);
            kbd_data_buf[0] = 0x00;
            kbd_data_buf[2] = 0x00;
            ble_hid_kbd_report(kbd_data_buf);
            break;
        }
        break;

    case KEY_VALUE_ENC_ADD:
        VIBR_ENCODER_TURN;
        if(get_usb_connected())
        {
            hid_consumer_queue_push_data(HID_CONSUMER_VOLUME_UP);
            break;
        }
        if(ble_get_connect_state())
        {
            ble_hid_consumer_report(HID_CONSUMER_VOLUME_UP);
            ble_hid_consumer_report(0x00);
            break;
        }
        break;

    case KEY_VALUE_ENC_DEC:
        VIBR_ENCODER_TURN;
        if(get_usb_connected())
        {
            hid_consumer_queue_push_data(HID_CONSUMER_VOLUME_DOWN);
            break;
        }
        if(ble_get_connect_state())
        {
            ble_hid_consumer_report(HID_CONSUMER_VOLUME_DOWN);
            ble_hid_consumer_report(0x00);
            break;
        }
        break;
    
    default:
        break;
    }

    return 0;
}

/******************************************* EOF *****************************************************/
