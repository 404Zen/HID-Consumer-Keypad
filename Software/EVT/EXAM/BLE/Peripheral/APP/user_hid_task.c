/********************************************************************************************************
* @file     user_hid_task.c
* 
* @brief    user_hid_task c file
* 
* @author   404zen@gmail.com
* 
* @date     2023-01-02  
* 
* @version  Ver: 0.1
* 
* @attention 
* 
* 自动释放按键.
* 
*******************************************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "user_hid_task.h"
#include "usb_hid.h"

#include "CONFIG.h"

/* Defines ------------------------------------------------------------------*/
#define HID_TASK_DEBUG_ENABLE           1

#if HID_TASK_DEBUG_ENABLE
#define LOG_HID_TASK(X...)     printf(X)
#else
#define LOG_HID_TASK(X...)
#endif

/* Variables ----------------------------------------------------------------*/
static uint8_t user_hid_task_id = INVALID_TASK_ID;

typedef struct user_hid_data_queue
{
    hid_key_code_struct     key_data[USER_HID_DATA_QUEUE_DEEPTH];
    uint16_t                w_ptr;
    uint16_t                r_ptr;
}user_hid_data_queue_t;
user_hid_data_queue_t hid_kbd_queue;

typedef struct hid_consumer_queue
{
    uint8_t     data[USER_HID_CONSUMER_QUEUE_DEEPTH];
    uint16_t    w_ptr;
    uint16_t    r_ptr;
}hid_consumer_queue_t;
hid_consumer_queue_t hid_consumer_queue;


/* Static functions prototypes ----------------------------------------------*/
static uint16_t user_hid_task_events(uint8_t task_id, uint16_t events);
static void hid_period_events(void);

static void hid_consumer_data_process(void);

/* Functions declears -------------------------------------------------------*/
/**
 * @brief  用户 HID 任务初始化
 * @note   None.
 * @param  None.
 * @retval None.
**/
void user_hid_task_init(void)
{
    LOG_HID_TASK("%s \r\n", __func__);

    user_hid_task_id = TMOS_ProcessEventRegister(user_hid_task_events);

    tmos_start_reload_task(user_hid_task_id, USER_HID_TASK_1MS_PERIOD, 2);      /* 每两个tick循环一次 1.25ms */

    hid_kbd_queue.w_ptr = 0;
    hid_kbd_queue.r_ptr = 0;

    hid_consumer_queue.r_ptr = 0;
    hid_consumer_queue.w_ptr = 0;
}


/**
 * @brief  用户HID任务事件处理
 * @note   None.
 * @param  None.
 * @retval None.
**/
static uint16_t user_hid_task_events(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(user_hid_task_id)) != NULL)
        {
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }

        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if(events & USER_HID_TASK_1MS_PERIOD)
    {
        /* 如果FIFO中还有数据， 查询端点是否完成数据传输后上传下一包数据 */
        hid_period_events();
        hid_consumer_data_process();
        return (events ^ USER_HID_TASK_1MS_PERIOD);
    }

    return 0;
}


/**
 * @brief  获取HID队列剩余空间
 * @note   None.
 * @param  None.
 * @retval None.
**/
uint16_t get_hid_data_queue_free_space(void)
{
    if(hid_kbd_queue.w_ptr > hid_kbd_queue.r_ptr)
        return (uint16_t)(USER_HID_DATA_QUEUE_DEEPTH - (hid_kbd_queue.w_ptr - hid_kbd_queue.r_ptr) - 1);
    else if(hid_kbd_queue.w_ptr == hid_kbd_queue.r_ptr)
        return (uint16_t)USER_HID_DATA_QUEUE_DEEPTH;
    else
        return (uint16_t)(USER_HID_DATA_QUEUE_DEEPTH - ((USER_HID_DATA_QUEUE_DEEPTH - hid_kbd_queue.r_ptr) + hid_kbd_queue.w_ptr) );
}



/**
 * @brief  push data to hid queue
 * @note   None.
 * @param  None.
 * @retval None.
**/
int8_t hid_queue_push_data(hid_key_code_struct *data)
{
    if(get_hid_data_queue_free_space() <= 0)
    {
        LOG_HID_TASK("hid data queue is full, can not write in\r\n");
        return -1;
    }
    else
    {
        memcpy(&(hid_kbd_queue.key_data[hid_kbd_queue.w_ptr]), data, sizeof(hid_key_code_struct));
        hid_kbd_queue.w_ptr++;
        hid_kbd_queue.w_ptr %= USER_HID_DATA_QUEUE_DEEPTH;
    }

    return 0;
}

/**
 * @brief  pop data from hid queue
 * @note   None.
 * @param  None.
 * @retval if no data, return NULL.
**/
hid_key_code_struct *hid_queue_pop_data(void)
{
    hid_key_code_struct *ptr = NULL;
    if(hid_kbd_queue.r_ptr == hid_kbd_queue.w_ptr)
    {
        return ptr;
    }
    else
    {
        ptr = &(hid_kbd_queue.key_data[hid_kbd_queue.r_ptr]);
        hid_kbd_queue.r_ptr++;
        hid_kbd_queue.r_ptr %= USER_HID_DATA_QUEUE_DEEPTH;

        return ptr;
    }
}



/**
 * @brief  HID循环调用时间
 * @note   None.
 * @param  None.
 * @retval None.
**/
static void hid_period_events(void)
{
    static uint8_t data_or_release = 0;         /* data = 1, release = 0 */
    hid_key_code_struct *hid_data = NULL;
    static uint8_t release_packets[8] = {0};


    switch (data_or_release)
    {
    case 0:
        /* 发送全0键值，释放按键 */
        if(EP1_GetINSta())              /* 查询上一包数据是否发送完成 */
        {
            usb_hid_kbd_report(release_packets);
            data_or_release = 1;
        }
        break;

    case 1:
        /* 发生带键值的数据包 */
        if(EP1_GetINSta())              /* 查询上一包数据是否发送完成 */
        {
            hid_data = hid_queue_pop_data();
            if(hid_data != NULL)
            {
                usb_hid_kbd_report((uint8_t *)hid_data);
                data_or_release = 0;
            }
        }
        break;
    
    default:
        data_or_release = 0;
        break;
    }
}

/* consumer */
/**
 * @brief  处理多媒体数据
 * @note   None.
 * @param  None.
 * @retval None.
**/
static void hid_consumer_data_process(void)
{
    static uint8_t data_flag = 1;                        /* 0-send release, 1-send data */
    uint8_t consumer_data = 0x00;

    switch (data_flag)
    {
    case 0:
        if(EP3_GetINSta())              /* 查询上一包数据是否发送完成 */
        {
            /* 发生按键释放包 */
            usb_hid_consumer_report(consumer_data);
            data_flag = 1;
        }
        break;

    case 1:
        if(EP3_GetINSta())
        {
            consumer_data = hid_consumer_queue_pop_data();
            if(consumer_data)
            {
                usb_hid_consumer_report(consumer_data);
                data_flag = 0;
            }
        }
        break;
    
    default:
        data_flag = 0;
        break;
    }
}
/**
 * @brief  获取多媒体数据队列剩余空间
 * @note   None.
 * @param  None.
 * @retval None.
**/
uint16 get_hid_consumer_queue_free_space(void)
{
    if(hid_consumer_queue.w_ptr > hid_consumer_queue.r_ptr)
    {
        return (uint16_t)(USER_HID_CONSUMER_QUEUE_DEEPTH - (hid_consumer_queue.w_ptr - hid_consumer_queue.r_ptr) - 1);
    }
    else if(hid_consumer_queue.w_ptr == hid_consumer_queue.r_ptr)
    {
        return (uint16_t)USER_HID_CONSUMER_QUEUE_DEEPTH;
    }
    else
    {
        return (uint16_t)(USER_HID_CONSUMER_QUEUE_DEEPTH - ((USER_HID_CONSUMER_QUEUE_DEEPTH - hid_consumer_queue.r_ptr) + hid_consumer_queue.w_ptr));
    }
}

/**
 * @brief  push数据到多媒体数据队列
 * @note   None.
 * @param  None.
 * @retval None.
**/
int8_t hid_consumer_queue_push_data(uint8_t data)   
{
    if(get_hid_consumer_queue_free_space())
    {
        hid_consumer_queue.data[hid_consumer_queue.w_ptr] = data;
        hid_consumer_queue.w_ptr++;
        hid_consumer_queue.w_ptr %= USER_HID_CONSUMER_QUEUE_DEEPTH;

        return 0;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief  从多媒体数据队列中pop数据
 * @note   None.
 * @param  None.
 * @retval 没有数据返回0x00.
**/
uint8_t hid_consumer_queue_pop_data(void)
{
    uint8_t ret = 0;
    if(hid_consumer_queue.r_ptr != hid_consumer_queue.w_ptr)
    {
        ret = hid_consumer_queue.data[hid_consumer_queue.r_ptr];
        hid_consumer_queue.r_ptr++;
        hid_consumer_queue.r_ptr %= USER_HID_CONSUMER_QUEUE_DEEPTH;
    }

    return ret;
}


/******************************************* EOF *****************************************************/
