/********************************************************************************************************
* @file     ws2812.c
* 
* @brief    ws2812 c file
* 
* @author   404zen@gmail.com
* 
* @date     2023-03-07  
* 
* @version  Ver: 0.1
* 
* @attention 
* 
* WS2812颜色排列 GRB. 数据传输高位在前, 不想写了， 自生自灭吧。
* 
*******************************************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "ws2812.h"
#include "CH58x_common.h"
#include "CONFIG.h"


/* Defines ------------------------------------------------------------------*/

/* 根据手册， 单个码元周期不得小于1200ns
    T0H: 200~400 ns， T0L: >= 800ns
    T1H: 580~1000ns,  T1L: >= 200ns
    RESET: >= 800ns low level

    SPI时钟为Fsys 9 分频 6.67MHz
    T0H = 300ns, T0L = 900 ns
    T1H = 900ns, T1L = 300 ns
    RESET = 1200ns low level
 */
#define WS2812_0_CODE                   0xC0
#define WS2812_1_CODE                   0xFC
#define WS2812_RESET                    0x00

/* TASK EVENT */
#define WS2812_REFRESH_EVENT            0x0001

/* Variables ----------------------------------------------------------------*/

typedef struct
{
    uint8_t mode;
    uint8_t times;
    uint16_t steps;
}ws2812_mode_control_t;
ws2812_mode_control_t ws2812_mode[3];
ws2812_data_buf_t ws2812_data_buf[3];
__attribute__((aligned(4))) uint8_t spi_data_buf[1 + 8*3*3];          /* Reset + Data */

static uint8_t ws2812_task_id = 0;

/* Static functions prototypes ----------------------------------------------*/

static int8_t ws2812_write(ws2812_data_buf_t *buf, uint8_t led_nums);
static uint16_t ws2812_task_event_process(uint8_t task_id, uint16_t events);

/* Functions declears -------------------------------------------------------*/
/**
 * @brief  初始化
 * @note   None.
 * @param  None.
 * @retval None.
**/
void ws2812_init(void)
{
    GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);

    R8_SPI0_CLOCK_DIV = 9;                      /* 60MHz / 9 = 6.67MHz */
    R8_SPI0_CTRL_MOD = RB_SPI_ALL_CLEAR;
    R8_SPI0_CTRL_MOD = RB_SPI_MOSI_OE;          /* Only enable output pin */
    R8_SPI0_CTRL_CFG |= RB_SPI_AUTO_IF;         /* 访问BUFFER/FIFO自动清除IF_BYTE_END标志 */
    R8_SPI0_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;     /* 不启动DMA方式 */

    SPI0_DataMode(Mode3_HighBitINFront);

    SPI0_MasterSendByte(0x00);                  /* 复位灯珠 */

    // SPI0_ITCfg(ENABLE, SPI0_IT_DMA_END);     /* 启动了SPI DMA传输完成中断后，会一直卡在中断里 */
    // PFIC_EnableIRQ(SPI0_IRQn);

    // ws2812_task_id = TMOS_ProcessEventRegister(ws2812_task_event_process);
    // tmos_start_reload_task(ws2812_task_id, WS2812_REFRESH_EVENT, MS1_TO_SYSTEM_TIME(10));           /* 10 ms 刷新一次 100fps */
}

/**
 * @brief  2812 事件处理函数
 * @note   None.
 * @param  None.
 * @retval None.
**/
static uint16_t ws2812_task_event_process(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *p_msg;
        if( (p_msg = tmos_msg_receive(ws2812_task_id)) != NULL)
        {
            tmos_msg_deallocate(p_msg);
        }

        return (events ^ SYS_EVENT_MSG);
    }

    if(events & WS2812_REFRESH_EVENT)
    {

        return (events ^ WS2812_REFRESH_EVENT);
    }

    return 0;
}

/**
 * @brief  设置灯珠颜色
 * @note   发送一帧完整的数据约需要 88 us(3 LEDS).
 * @param  None.
 * @retval None.
**/
static int8_t ws2812_write(ws2812_data_buf_t *buf, uint8_t led_nums)
{
    if(led_nums == 0 || led_nums > 3)
        return -1;

    if(buf == NULL)
        return -2;

    memset(spi_data_buf, 0x00, sizeof(spi_data_buf));

    /* spi_data_buf首字节为复位帧 */
    uint8_t *p_data = (uint8_t *)buf;
    spi_data_buf[0] = WS2812_RESET;
    for (size_t i = 0; i < led_nums; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            uint8_t data = *p_data;
            if(data & 0x80)
            {
                spi_data_buf[i*8 + j + 1] = WS2812_1_CODE;
            }
            else
            {
                spi_data_buf[i*8 + j + 1] = WS2812_0_CODE;
            }
            data <<= 1;
        }
        p_data++;
    }
    return 0;
}

/**
 * @brief  设置单个灯珠颜色
 * @note   None.
 * @param  index 灯珠编号.
 * @param  g r b color.
 * @param  mode 模式
 * @param  times 持续时间
 * @retval None.
**/
void ws2812_set_pixel(uint8_t index, uint8_t g, uint8_t r, uint8_t b, uint8_t mode, uint8_t times)
{
    if(ws2812_mode[index].steps != 0)
        return ;
    
    ws2812_data_buf[index].g = g; ws2812_data_buf[index].r = r; ws2812_data_buf[index].b = b;
    ws2812_mode[index].mode = mode; ws2812_mode[index].times = times;

    ws2812_mode[index].steps = 1;
}

/**
 * @brief  SPI0 interrupt handler
 * @note   None.
 * @param  None.
 * @retval None.
**/
__INTERRUPT
__HIGH_CODE
void SPI0_IRQHandler(void)
{
    uint8_t intr_flag = R8_SPI0_INT_FLAG;

    if(intr_flag & SPI0_IT_DMA_END)
    {
        PRINT("SPI0 DMA END\r\n");
        SPI0_ClearITFlag(SPI0_IT_DMA_END);
    }
}




/******************************************* EOF *****************************************************/
