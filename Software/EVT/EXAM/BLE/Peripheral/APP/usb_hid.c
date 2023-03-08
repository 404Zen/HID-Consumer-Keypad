/********************************************************************************************************
* @file     usb_hid.c
* 
* @brief    usb_hid c file
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
#include "usb_hid.h"
#include "CH58x_common.h"

#define DevEP0SIZE    0x40
// 设备描述符
const uint8_t MyDevDescr[] = {0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, DevEP0SIZE, 0x3e, 0x41, 0x07, 0x21, 0x00, 0x00, 0x01, 0x02, 0x00, 0x01};

/* 多媒体报告描述符 */
const uint8_t ConsumerRepDesc[] =
{
    #if 0   /* 可以自定义每个bit的键值 */
    0x05,0x0C,  // Usage Page (Consumer)
    0x09,0x01,  // Usage(Consumer Control)
    0xA1,0x01,  // Collection (Application),                Main Items —— Collection —— Application
        0x15,0x00,  // Logical Minimum (0),                     Global Items —— Logical Minimum —— 0
        0x25,0x01,  // Logical Maximum (1),                     Global Items —— Logical Maximum —— 1
        0x75,0x01,  // Report Size (1),                         Global Items —— Report Size —— 1
        0x95,0x01,  // Report Count (1),                        Global Items —— Report Count —— 1

        0x09,0xCD,  // Usage(Play/Pause),开始暂停
        0x81,0x06,  // Input (Data, Value, Relative),

        0x09,0xB5,  // Usage(Scan Next Track),下一曲
        0x81,0x06,  // Input (Data, Value, Relative),

        0x09,0xB6,  // Usage(Scan Previous Track),上一曲
        0x81,0x06,  // Input (Data, Value, Relative),

        0x09,0xE2,  // Usage(Mute),静音
        0x81,0x06,  // Input (Data, Value, Relative),

        0x09,0xE9,  // Usage(Volume Increment),音量+
        0x81,0x06,  // Input (Data, Value, Relative),

        0x09,0xEA,  // Usage(Volume Decrement),音量-
        0x81,0x06,  // Input (Data, Value, Relative),

        0x09,0xB3,  // Usage(Fast Forward),快进
        0x81,0x02,  // Input (Data, Value, Absolute),

        0x09,0xB4,  // Usage(Rewind),倒带
        0x81,0x02,  // Input (Data, Value, Absolute),
    0xC0  // End Collection,                                Main Items —— End Collection
    #else               /* 直接发送键值 */
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x15, 0x00,        //   Logical Minimum (0)
    0x26, 0xFF, 0x1F,  //   Logical Maximum (8191)
    0x19, 0x00,        //   Usage Minimum (Unassigned)
    0x2A, 0xFF, 0x1F,  //   Usage Maximum (0x1FFF)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
    #endif
};


// 配置描述符
const uint8_t MyCfgDescr[] = {
    0x09,        //   bLength
    0x02,        //   bDescriptorType (Configuration)
    0x54, 0x00,  //   wTotalLength 59
    0x03,        //   bNumInterfaces 3
    0x01,        //   bConfigurationValue
    0x00,        //   iConfiguration (String Index)
    0xA0,        //   bmAttributes Remote Wakeup
    0x32,        //   bMaxPower 100mA

// 09  HID keyboard
    0x09,        //   bLength
    0x04,        //   bDescriptorType (Interface)
    0x00,        //   bInterfaceNumber 0
    0x00,        //   bAlternateSetting
    0x01,        //   bNumEndpoints 1
    0x03,        //   bInterfaceClass
    0x01,        //   bInterfaceSubClass
    0x01,        //   bInterfaceProtocol
    0x00,        //   iInterface (String Index)

// 18
    0x09,        //   bLength
    0x21,        //   bDescriptorType (HID)
    0x11, 0x01,  //   bcdHID 1.11
    0x00,        //   bCountryCode
    0x01,        //   bNumDescriptors
    0x22,        //   bDescriptorType[0] (HID)
    0x3E, 0x00,  //   wDescriptorLength[0] 62

//27
    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x81,        //   bEndpointAddress (IN/D2H)
    0x03,        //   bmAttributes (Interrupt)
    0x08, 0x00,  //   wMaxPacketSize 8
    0x0A,        //   bInterval 10 (unit depends on device speed)

//34 HID Mouse
    0x09,        //   bLength
    0x04,        //   bDescriptorType (Interface)
    0x01,        //   bInterfaceNumber 1
    0x00,        //   bAlternateSetting
    0x01,        //   bNumEndpoints 1
    0x03,        //   bInterfaceClass
    0x01,        //   bInterfaceSubClass
    0x02,        //   bInterfaceProtocol
    0x00,        //   iInterface (String Index)

//43
    0x09,        //   bLength
    0x21,        //   bDescriptorType (HID)
    0x10, 0x01,  //   bcdHID 1.10
    0x00,        //   bCountryCode
    0x01,        //   bNumDescriptors
    0x22,        //   bDescriptorType[0] (HID)
    0x34, 0x00,  //   wDescriptorLength[0] 52

//52
    0x07,        //   bLength
    0x05,        //   bDescriptorType (Endpoint)
    0x82,        //   bEndpointAddress (IN/D2H)
    0x03,        //   bmAttributes (Interrupt)
    0x04, 0x00,  //   wMaxPacketSize 4
    0x0A,        //   bInterval 10 (unit depends on device speed)

// 59 HID Consumer
    0x09,        //   bLength
    0x04,        //   bDescriptorType (Interface)
    0x02,        //   bInterfaceNumber 2
    0x00,        //   bAlternateSetting
    0x01,        //   bNumEndpoints 1
    0x03,        //   bInterfaceClass
    0x01,        //   bInterfaceSubClass
    0x01,        //   bInterfaceProtocol
    0x00,        //   iInterface (String Index)

// 68
    0x09,  // bLength。配置本描述符的长度。
    0x21,  // bDescriptorType。描述符类型。HID描述符为0x21。
    0x11,0x01,  // bcdHID。HID设备所遵循的HID版本号，为4位16进制的BCD码。1.0即0x0100，1.1即0x0101，2.0即0x0200。
    0x00,  // bCountryCode。HID设备国家/地区代码。
    0x01,  // bNumDescriptor。HID设备支持的下级描述符的数量。由于HID设备至少需要包括一个报告描述符，故其值至小为0x01，一般的HID设备也为1，也就是有一个报告描述符，物理描述符很少用到。
    0x22,  // bDescriptorTyep。下级描述符的类型。下级描述符第1个必须是报告描述符，所以这里存放报告描述符类型，报告描述符的类型为0x22。
    0x17,0x00,  // wDescriptorLength。下级描述符的长度

// 76   
    // 端点描述符(Consumer)
    0x07,  // bLength。配置本描述符的长度。
    0x05,  // bDescriptorType。描述符类型。端点描述符为0x05。
    0x83,  // bEndpointAddress。
                // Bit 3…0: 端点编号；
                // Bit 6…4: 保留，默认为0；
                // Bit 7:如果是控制端点可以忽略，因为控制端点有两个方向，否则一般表示数据传输方向，0 = OUT endpoint   1 = IN endpoint。
    0x03,  // bmAttributes。Bits 1..0: 表示传输类型
                // 00 = Control-控制传输
                // 01 = Isochronous-同步传输
                // 10 = Bulk-批量传输
                // 11 = Interrupt-中断传输
    0x02, 0x00,  // wMaxPackeSize（双字节）。表示当前配置下此端点能够接收或发送的最大数据包的大小。
    0x0a  // bInterval。查询时间。就是主机多久和设备通讯一次。低速和全速称为帧，一个值代表1ms。高速称为微帧，一个值代表125us。

};
/* USB速度匹配描述符 */
const uint8_t My_QueDescr[] = {0x0A, 0x06, 0x00, 0x02, 0xFF, 0x00, 0xFF, 0x40, 0x01, 0x00};

/* USB全速模式,其他速度配置描述符 */
uint8_t USB_FS_OSC_DESC[sizeof(MyCfgDescr)] = {
    0x09, 0x07, /* 其他部分通过程序复制 */
};



// 语言描述符
const uint8_t MyLangDescr[] = {0x04, 0x03, 0x09, 0x04};
// 厂家信息
const uint8_t MyManuInfo[] = {0x0A, 0x03, 'D', 0, 'i', 0, 'n', 0, 'o', 0};
// 产品信息
const uint8_t MyProdInfo[] = {0x16, 0x03, 'D', 0, 'i', 0, 'n', 0, 'o', 0, 'K', 0, 'e', 0, 'y', 0, 'p', 0, 'a', 0, 'd', 0};
/*HID类报表描述符*/
const uint8_t KeyRepDesc[] = {0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7, 0x15, 0x00, 0x25,
                              0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x01, 0x95, 0x03,
                              0x75, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02, 0x95, 0x05, 0x75, 0x01, 0x91,
                              0x01, 0x95, 0x06, 0x75, 0x08, 0x26, 0xff, 0x00, 0x05, 0x07, 0x19, 0x00, 0x29, 0x91, 0x81,
                              0x00, 0xC0};
const uint8_t MouseRepDesc[] = {0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x09, 0x01, 0xA1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29,
                                0x03, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x03, 0x81, 0x02, 0x75, 0x05, 0x95, 0x01,
                                0x81, 0x01, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x09, 0x38, 0x15, 0x81, 0x25, 0x7f, 0x75,
                                0x08, 0x95, 0x03, 0x81, 0x06, 0xC0, 0xC0};

/**********************************************************/
uint8_t        DevConfig, Ready;
uint8_t        SetupReqCode;
uint16_t       SetupReqLen;
const uint8_t *pDescr;
uint8_t        Report_Value = 0x00;
uint8_t        Idle_Value = 0x00;
uint8_t        USB_SleepStatus = 0x00; /* USB睡眠状态 */

/*鼠标键盘数据*/
uint8_t HIDMouse[4] = {0x0, 0x0, 0x0, 0x0};
uint8_t HIDKey[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
/******** 用户自定义分配端点RAM ****************************************/
__attribute__((aligned(4))) uint8_t EP0_Databuf[64 + 64 + 64]; //ep0(64)+ep4_out(64)+ep4_in(64)
__attribute__((aligned(4))) uint8_t EP1_Databuf[64 + 64];      //ep1_out(64)+ep1_in(64)
__attribute__((aligned(4))) uint8_t EP2_Databuf[64 + 64];      //ep2_out(64)+ep2_in(64)
__attribute__((aligned(4))) uint8_t EP3_Databuf[64 + 64];      //ep3_out(64)+ep3_in(64)


static bool usb_connected = false;


/*********************************************************************
 * @fn      USB_DevTransProcess
 *
 * @brief   USB 传输处理函数
 *
 * @return  none
 */
void USB_DevTransProcess(void)
{
    uint8_t len, chtype;
    uint8_t intflag, errflag = 0;

    intflag = R8_USB_INT_FG;
    if(intflag & RB_UIF_TRANSFER)
    {
        if((R8_USB_INT_ST & MASK_UIS_TOKEN) != MASK_UIS_TOKEN) // 非空闲
        {
            switch(R8_USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
            // 分析操作令牌和端点号
            {
                case UIS_TOKEN_IN:
                {
                    switch(SetupReqCode)
                    {
                        case USB_GET_DESCRIPTOR:
                            len = SetupReqLen >= DevEP0SIZE ? DevEP0SIZE : SetupReqLen; // 本次传输长度
                            memcpy(pEP0_DataBuf, pDescr, len);                          /* 加载上传数据 */
                            SetupReqLen -= len;
                            pDescr += len;
                            R8_UEP0_T_LEN = len;
                            R8_UEP0_CTRL ^= RB_UEP_T_TOG; // 翻转
                            break;
                        case USB_SET_ADDRESS:
                            R8_USB_DEV_AD = (R8_USB_DEV_AD & RB_UDA_GP_BIT) | SetupReqLen;
                            R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                            break;

                        case USB_SET_FEATURE:
                            break;

                        default:
                            R8_UEP0_T_LEN = 0; // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                            R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                            break;
                    }
                }
                break;

                case UIS_TOKEN_OUT:
                {
                    len = R8_USB_RX_LEN;
                    if(SetupReqCode == 0x09)
                    {
                        PRINT("[%s] Num Lock\t", (pEP0_DataBuf[0] & (1<<0)) ? "*" : " ");
                        PRINT("[%s] Caps Lock\t", (pEP0_DataBuf[0] & (1<<1)) ? "*" : " ");
                        PRINT("[%s] Scroll Lock\n", (pEP0_DataBuf[0] & (1<<2)) ? "*" : " ");
                    }
                }
                break;

                case UIS_TOKEN_OUT | 1:
                {
                    if(R8_USB_INT_ST & RB_UIS_TOG_OK)
                    { // 不同步的数据包将丢弃
                        R8_UEP1_CTRL ^= RB_UEP_R_TOG;
                        len = R8_USB_RX_LEN;
                        DevEP1_OUT_Deal(len);
                    }
                }
                break;

                case UIS_TOKEN_IN | 1:
                    R8_UEP1_CTRL ^= RB_UEP_T_TOG;
                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                    break;

                case UIS_TOKEN_OUT | 2:
                {
                    if(R8_USB_INT_ST & RB_UIS_TOG_OK)
                    { // 不同步的数据包将丢弃
                        R8_UEP2_CTRL ^= RB_UEP_R_TOG;
                        len = R8_USB_RX_LEN;
                        DevEP2_OUT_Deal(len);
                    }
                }
                break;

                case UIS_TOKEN_IN | 2:
                    R8_UEP2_CTRL ^= RB_UEP_T_TOG;
                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                    break;

                case UIS_TOKEN_OUT | 3:
                {
                    if(R8_USB_INT_ST & RB_UIS_TOG_OK)
                    { // 不同步的数据包将丢弃
                        R8_UEP3_CTRL ^= RB_UEP_R_TOG;
                        len = R8_USB_RX_LEN;
                        DevEP3_OUT_Deal(len);
                    }
                }
                break;

                case UIS_TOKEN_IN | 3:
                    R8_UEP3_CTRL ^= RB_UEP_T_TOG;
                    R8_UEP3_CTRL = (R8_UEP3_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                    break;

                case UIS_TOKEN_OUT | 4:
                {
                    if(R8_USB_INT_ST & RB_UIS_TOG_OK)
                    {
                        R8_UEP4_CTRL ^= RB_UEP_R_TOG;
                        len = R8_USB_RX_LEN;
                        DevEP4_OUT_Deal(len);
                    }
                }
                break;

                case UIS_TOKEN_IN | 4:
                    R8_UEP4_CTRL ^= RB_UEP_T_TOG;
                    R8_UEP4_CTRL = (R8_UEP4_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_NAK;
                    break;

                default:
                    break;
            }
            R8_USB_INT_FG = RB_UIF_TRANSFER;
        }
        if(R8_USB_INT_ST & RB_UIS_SETUP_ACT) // Setup包处理
        {
            R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
            SetupReqLen = pSetupReqPak->wLength;
            SetupReqCode = pSetupReqPak->bRequest;
            chtype = pSetupReqPak->bRequestType;

            len = 0;
            errflag = 0;
            if((pSetupReqPak->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD)
            {
                /* 非标准请求 */
                /* 其它请求,如类请求，产商请求等 */
                if(pSetupReqPak->bRequestType & 0x40)
                {
                    /* 厂商请求 */
                }
                else if(pSetupReqPak->bRequestType & 0x20)
                {
                    switch(SetupReqCode)
                    {
                        case DEF_USB_SET_IDLE: /* 0x0A: SET_IDLE */
                            Idle_Value = EP0_Databuf[3];
                            break; //这个一定要有

                        case DEF_USB_SET_REPORT: /* 0x09: SET_REPORT */
                            break;

                        case DEF_USB_SET_PROTOCOL: /* 0x0B: SET_PROTOCOL */
                            Report_Value = EP0_Databuf[2];
                            break;

                        case DEF_USB_GET_IDLE: /* 0x02: GET_IDLE */
                            EP0_Databuf[0] = Idle_Value;
                            len = 1;
                            break;

                        case DEF_USB_GET_PROTOCOL: /* 0x03: GET_PROTOCOL */
                            EP0_Databuf[0] = Report_Value;
                            len = 1;
                            break;

                        default:
                            errflag = 0xFF;
                    }
                }
            }
            else /* 标准请求 */
            {
                switch(SetupReqCode)
                {
                    case USB_GET_DESCRIPTOR:
                    {
                        switch(((pSetupReqPak->wValue) >> 8))
                        {
                            /* 请求设备描述符 */
                            case USB_DESCR_TYP_DEVICE:
                            {
                                pDescr = MyDevDescr;
                                len = MyDevDescr[0];
                            }
                            break;

                            /* 请求配置描述符 */
                            case USB_DESCR_TYP_CONFIG:
                            {
                                pDescr = MyCfgDescr;
                                len = MyCfgDescr[2];
                            }
                            break;

                            /* HID描述符 */
                            case USB_DESCR_TYP_HID:
                                switch((pSetupReqPak->wIndex) & 0xff)
                                {
                                    /* 选择接口 */
                                    case 0:
                                    /* HID 描述符 Mouse */
                                        pDescr = (uint8_t *)(&MyCfgDescr[18]);
                                        len = 9;
                                        break;

                                    case 1:
                                        pDescr = (uint8_t *)(&MyCfgDescr[43]);
                                        len = 9;
                                        break;

                                    case 2:
                                        pDescr = (uint8_t *)(&MyCfgDescr[68]);
                                        len = 9;
                                        break;

                                    default:
                                        /* 不支持的字符串描述符 */
                                        errflag = 0xff;
                                        break;
                                }
                                break;

                            case USB_DESCR_TYP_REPORT:
                            {
                                if(((pSetupReqPak->wIndex) & 0xff) == 0) //接口0报表描述符
                                {
                                    pDescr = KeyRepDesc; //数据准备上传
                                    len = sizeof(KeyRepDesc);
                                }
                                else if(((pSetupReqPak->wIndex) & 0xff) == 1) //接口1报表描述符
                                {
                                    pDescr = MouseRepDesc; //数据准备上传
                                    len = sizeof(MouseRepDesc);
                                    // Ready = 1; //如果有更多接口，该标准位应该在最后一个接口配置完成后有效
                                }
                                else if(((pSetupReqPak->wIndex) & 0xff) == 2) //接口2报表描述符
                                {
                                    pDescr = ConsumerRepDesc; //数据准备上传
                                    len = sizeof(ConsumerRepDesc);
                                    Ready = 1;
                                }
                                else
                                    len = 0xff; //本程序只有3个接口，这句话正常不可能执行
                            }
                            break;

                            case USB_DESCR_TYP_STRING:
                            {
                                switch((pSetupReqPak->wValue) & 0xff)
                                {
                                    case 1:
                                        pDescr = MyManuInfo;
                                        len = MyManuInfo[0];
                                        break;
                                    case 2:
                                        pDescr = MyProdInfo;
                                        len = MyProdInfo[0];
                                        break;
                                    case 0:
                                        pDescr = MyLangDescr;
                                        len = MyLangDescr[0];
                                        break;
                                    default:
                                        errflag = 0xFF; // 不支持的字符串描述符
                                        break;
                                }
                            }
                            break;

                            case 0x06:
                                pDescr = (uint8_t *)(&My_QueDescr[0]);
                                len = sizeof(My_QueDescr);
                                break;

                            case 0x07:
                                memcpy(&USB_FS_OSC_DESC[2], &MyCfgDescr[2], sizeof(MyCfgDescr) - 2);
                                pDescr = (uint8_t *)(&USB_FS_OSC_DESC[0]);
                                len = sizeof(USB_FS_OSC_DESC);
                                break;

                            default:
                                errflag = 0xff;
                                break;
                        }
                        if(SetupReqLen > len)
                            SetupReqLen = len; //实际需上传总长度
                        len = (SetupReqLen >= DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                        memcpy(pEP0_DataBuf, pDescr, len);
                        pDescr += len;
                    }
                    break;

                    case USB_SET_ADDRESS:
                        PRINT("USB_SET_ADDRESS\r\n");                           /* 主机设置地址 */
                        usb_connected = true;
                        SetupReqLen = (pSetupReqPak->wValue) & 0xff;
                        break;

                    case USB_GET_CONFIGURATION:
                        pEP0_DataBuf[0] = DevConfig;
                        if(SetupReqLen > 1)
                            SetupReqLen = 1;
                        break;

                    case USB_SET_CONFIGURATION:
                        DevConfig = (pSetupReqPak->wValue) & 0xff;
                        break;

                    case USB_CLEAR_FEATURE:
                    {
                        if((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) // 端点
                        {
                            switch((pSetupReqPak->wIndex) & 0xff)
                            {
                                case 0x83:
                                    R8_UEP3_CTRL = (R8_UEP3_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                                    break;
                                case 0x03:
                                    R8_UEP3_CTRL = (R8_UEP3_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                                    break;
                                case 0x82:
                                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                                    break;
                                case 0x02:
                                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                                    break;
                                case 0x81:
                                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_NAK;
                                    break;
                                case 0x01:
                                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_ACK;
                                    break;
                                default:
                                    errflag = 0xFF; // 不支持的端点
                                    break;
                            }
                        }
                        else if((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
                        {
                            if(pSetupReqPak->wValue == 1)
                            {
                                USB_SleepStatus &= ~0x01;
                            }
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                    }
                    break;

                    case USB_SET_FEATURE:
                        if((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
                        {
                            /* 端点 */
                            switch(pSetupReqPak->wIndex)
                            {
                                case 0x83:
                                    R8_UEP3_CTRL = (R8_UEP3_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_STALL;
                                    break;
                                case 0x03:
                                    R8_UEP3_CTRL = (R8_UEP3_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_STALL;
                                    break;
                                case 0x82:
                                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_STALL;
                                    break;
                                case 0x02:
                                    R8_UEP2_CTRL = (R8_UEP2_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_STALL;
                                    break;
                                case 0x81:
                                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_T_TOG | MASK_UEP_T_RES)) | UEP_T_RES_STALL;
                                    break;
                                case 0x01:
                                    R8_UEP1_CTRL = (R8_UEP1_CTRL & ~(RB_UEP_R_TOG | MASK_UEP_R_RES)) | UEP_R_RES_STALL;
                                    break;
                                default:
                                    /* 不支持的端点 */
                                    errflag = 0xFF; // 不支持的端点
                                    break;
                            }
                        }
                        else if((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
                        {
                            if(pSetupReqPak->wValue == 1)
                            {
                                /* 设置睡眠 */
                                USB_SleepStatus |= 0x01;
                            }
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                        break;

                    case USB_GET_INTERFACE:
                        pEP0_DataBuf[0] = 0x00;
                        if(SetupReqLen > 1)
                            SetupReqLen = 1;
                        break;

                    case USB_SET_INTERFACE:
                        break;

                    case USB_GET_STATUS:
                        if((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
                        {
                            /* 端点 */
                            pEP0_DataBuf[0] = 0x00;
                            switch(pSetupReqPak->wIndex)
                            {
                                case 0x83:
                                    if((R8_UEP3_CTRL & (RB_UEP_T_TOG | MASK_UEP_T_RES)) == UEP_T_RES_STALL)
                                    {
                                        pEP0_DataBuf[0] = 0x01;
                                    }
                                    break;

                                case 0x03:
                                    if((R8_UEP3_CTRL & (RB_UEP_R_TOG | MASK_UEP_R_RES)) == UEP_R_RES_STALL)
                                    {
                                        pEP0_DataBuf[0] = 0x01;
                                    }
                                    break;

                                case 0x82:
                                    if((R8_UEP2_CTRL & (RB_UEP_T_TOG | MASK_UEP_T_RES)) == UEP_T_RES_STALL)
                                    {
                                        pEP0_DataBuf[0] = 0x01;
                                    }
                                    break;

                                case 0x02:
                                    if((R8_UEP2_CTRL & (RB_UEP_R_TOG | MASK_UEP_R_RES)) == UEP_R_RES_STALL)
                                    {
                                        pEP0_DataBuf[0] = 0x01;
                                    }
                                    break;

                                case 0x81:
                                    if((R8_UEP1_CTRL & (RB_UEP_T_TOG | MASK_UEP_T_RES)) == UEP_T_RES_STALL)
                                    {
                                        pEP0_DataBuf[0] = 0x01;
                                    }
                                    break;

                                case 0x01:
                                    if((R8_UEP1_CTRL & (RB_UEP_R_TOG | MASK_UEP_R_RES)) == UEP_R_RES_STALL)
                                    {
                                        pEP0_DataBuf[0] = 0x01;
                                    }
                                    break;
                            }
                        }
                        else if((pSetupReqPak->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
                        {
                            pEP0_DataBuf[0] = 0x00;
                            if(USB_SleepStatus)
                            {
                                pEP0_DataBuf[0] = 0x02;
                            }
                            else
                            {
                                pEP0_DataBuf[0] = 0x00;
                            }
                        }
                        pEP0_DataBuf[1] = 0;
                        if(SetupReqLen >= 2)
                        {
                            SetupReqLen = 2;
                        }
                        break;

                    default:
                        errflag = 0xff;
                        break;
                }
            }
            if(errflag == 0xff) // 错误或不支持
            {
                //                  SetupReqCode = 0xFF;
                R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL; // STALL
            }
            else
            {
                if(chtype & 0x80) // 上传
                {
                    len = (SetupReqLen > DevEP0SIZE) ? DevEP0SIZE : SetupReqLen;
                    SetupReqLen -= len;
                }
                else
                    len = 0; // 下传
                R8_UEP0_T_LEN = len;
                R8_UEP0_CTRL = RB_UEP_R_TOG | RB_UEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; // 默认数据包是DATA1
            }

            R8_USB_INT_FG = RB_UIF_TRANSFER;
        }
    }
    else if(intflag & RB_UIF_BUS_RST)
    {
        R8_USB_DEV_AD = 0;
        R8_UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_UEP1_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_UEP3_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        R8_USB_INT_FG = RB_UIF_BUS_RST;
    }
    else if(intflag & RB_UIF_SUSPEND)
    {
        if(R8_USB_MIS_ST & RB_UMS_SUSPEND)
        {
            /* USB lost connected */
            PRINT("RB_UMS_SUSPEND\r\n");
            usb_connected = false;
        } // 挂起
        else
        {
            PRINT("RB_UMS_WAKEUP\r\n");
            ;
        } // 唤醒
        R8_USB_INT_FG = RB_UIF_SUSPEND;
    }
    else
    {
        R8_USB_INT_FG = intflag;
    }
}

/*********************************************************************
 * @fn      DevHIDMouseReport
 *
 * @brief   上报鼠标数据
 *
 * @return  none
 */
void DevHIDMouseReport(uint8_t mouse)
{
    HIDMouse[0] = mouse;
    memcpy(pEP2_IN_DataBuf, HIDMouse, sizeof(HIDMouse));
    DevEP2_IN_Deal(sizeof(HIDMouse));
}

/*********************************************************************
 * @fn      DevHIDKeyReport
 *
 * @brief   上报键盘数据
 *
 * @return  none
 */
void DevHIDKeyReport(uint8_t key)
{
    HIDKey[2] = key;
    memcpy(pEP1_IN_DataBuf, HIDKey, sizeof(HIDKey));
    DevEP1_IN_Deal(sizeof(HIDKey));
}

void usb_hid_kbd_report(uint8_t *data)
{
    memcpy(pEP1_IN_DataBuf, data, 8);
    DevEP1_IN_Deal(8);
}

void usb_hid_consumer_report(uint8_t data)
{
    uint8_t buf[2];
    buf[0] = data;
    buf[1] = 0x00;
    memcpy(pEP3_IN_DataBuf, &buf, 2);
    DevEP3_IN_Deal(2);
}

/*********************************************************************
 * @fn      DevWakeup
 *
 * @brief   设备模式唤醒主机
 *
 * @return  none
 */
void DevWakeup(void)
{
    R16_PIN_ANALOG_IE &= ~(RB_PIN_USB_DP_PU);
    R8_UDEV_CTRL |= RB_UD_LOW_SPEED;
    mDelaymS(2);
    R8_UDEV_CTRL &= ~RB_UD_LOW_SPEED;
    R16_PIN_ANALOG_IE |= RB_PIN_USB_DP_PU;
}


/*********************************************************************
 * @fn      user_usb_hid_init
 *
 * @brief   主函数
 *
 * @return  none
 */
void user_usb_hid_init(void)
{
    pEP0_RAM_Addr = EP0_Databuf;
    pEP1_RAM_Addr = EP1_Databuf;
    pEP2_RAM_Addr = EP2_Databuf;
    pEP3_RAM_Addr = EP3_Databuf;

    USB_DeviceInit();

    PFIC_EnableIRQ(USB_IRQn);
}

/*********************************************************************
 * @fn      DevEP1_OUT_Deal
 *
 * @brief   端点1数据处理
 *
 * @return  none
 */
void DevEP1_OUT_Deal(uint8_t l)
{ /* 用户可自定义 */
    uint8_t i;

    for(i = 0; i < l; i++)
    {
        pEP1_IN_DataBuf[i] = ~pEP1_OUT_DataBuf[i];
    }
    DevEP1_IN_Deal(l);
}

/*********************************************************************
 * @fn      DevEP2_OUT_Deal
 *
 * @brief   端点2数据处理
 *
 * @return  none
 */
void DevEP2_OUT_Deal(uint8_t l)
{ /* 用户可自定义 */
    uint8_t i;

    for(i = 0; i < l; i++)
    {
        pEP2_IN_DataBuf[i] = ~pEP2_OUT_DataBuf[i];
    }
    DevEP2_IN_Deal(l);
}

/*********************************************************************
 * @fn      DevEP3_OUT_Deal
 *
 * @brief   端点3数据处理
 *
 * @return  none
 */
void DevEP3_OUT_Deal(uint8_t l)
{ /* 用户可自定义 */
    uint8_t i;

    for(i = 0; i < l; i++)
    {
        pEP3_IN_DataBuf[i] = ~pEP3_OUT_DataBuf[i];
    }
    DevEP3_IN_Deal(l);
}

/*********************************************************************
 * @fn      DevEP4_OUT_Deal
 *
 * @brief   端点4数据处理
 *
 * @return  none
 */
void DevEP4_OUT_Deal(uint8_t l)
{ /* 用户可自定义 */
    uint8_t i;

    for(i = 0; i < l; i++)
    {
        pEP4_IN_DataBuf[i] = ~pEP4_OUT_DataBuf[i];
    }
    DevEP4_IN_Deal(l);
}

/**
 * @brief  获取 USB 连接状态
 * @note   None.
 * @param  None.
 * @retval None.
**/
bool get_usb_connected(void)
{
    return usb_connected;
}

/*********************************************************************
 * @fn      USB_IRQHandler
 *
 * @brief   USB中断函数
 *
 * @return  none
 */
__INTERRUPT
__HIGH_CODE
void USB_IRQHandler(void) /* USB中断服务程序,使用寄存器组1 */
{
    USB_DevTransProcess();
}



/******************************************* EOF *****************************************************/
