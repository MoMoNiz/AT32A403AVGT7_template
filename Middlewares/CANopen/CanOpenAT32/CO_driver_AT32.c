/*
 * CAN module object for STM32 (FD)CAN peripheral IP.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        CO_driver.c
 * @ingroup     CO_driver
 * @author      Hamed Jafarzadeh 	2022
 * 				Tilen Marjerle		2021
 * 				Janez Paternoster	2020
 * @copyright   2004 - 2020 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Implementation Author:               Tilen Majerle <tilen@majerle.eu>
 */
#include "301/CO_driver.h"
#include "CO_app_AT32.h"

#include "rtthread.h"

/* 本地 CAN 模块对象 */
static CO_CANmodule_t* CANModule_local = NULL; /* Local instance of global CAN module */

/* CAN 标识符掩码 */
#define CANID_MASK 0x07FF /*!< CAN 标准 ID 掩码 */
#define FLAG_RTR   0x8000 /*!< RTR 标志，标识符的一部分 */

/******************************************************************************/
void CO_CANsetConfigurationMode(void* CANptr) 
{
	/* 将 CAN 模块置于配置模式 */
	if (CANptr != NULL) 
	{
    // 将can 寄存器值复位到默认值
		can_reset(((CANopenNodeAT32*)CANptr)->CANHandle);
		
		// CAN 工作模式设置:冻结模式--用于 CAN 控制器初始化
		can_operating_mode_set(((CANopenNodeAT32*)CANptr)->CANHandle,CAN_OPERATINGMODE_FREEZE);
	}
}

/******************************************************************************/
void CO_CANsetNormalMode(CO_CANmodule_t* CANmodule) 
{
    /* 将 CAN 模块置于正常模式 */
    if (CANmodule->CANptr != NULL) 
    {
        // 通信模式--用于正常通信
        if (can_operating_mode_set(((CANopenNodeAT32*)CANmodule->CANptr)->CANHandle,CAN_OPERATINGMODE_COMMUNICATE) == SUCCESS)
        {
            CANmodule->CANnormal = true;
        }
    }
}

/******************************************************************************/
CO_ReturnError_t
CO_CANmodule_init(CO_CANmodule_t* CANmodule, void* CANptr, CO_CANrx_t rxArray[], uint16_t rxSize, CO_CANtx_t txArray[],
                  uint16_t txSize, uint16_t CANbitRate) {

    /* 验证参数 */
    if (CANmodule == NULL || rxArray == NULL || txArray == NULL) 
    {
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* 保持 CANModule 变量 */
    CANmodule->CANptr = CANptr;

    /* 保存 CANModule 的本地副本 */
    CANModule_local = CANmodule;

    /* 配置对象变量 */
    CANmodule->rxArray = rxArray;
    CANmodule->rxSize = rxSize;
    CANmodule->txArray = txArray;
    CANmodule->txSize = txSize;
    CANmodule->CANerrorStatus = 0;
    CANmodule->CANnormal = false;
    CANmodule->useCANrxFilters = false; /* 请勿使用 HW 过滤器 */
    CANmodule->bufferInhibitFlag = false;
    CANmodule->firstCANtxMessage = true;
    CANmodule->CANtxCount = 0U;
    CANmodule->errOld = 0U;

    /* 重置所有变量 */
    for (uint16_t i = 0U; i < rxSize; i++) {
        rxArray[i].ident = 0U;
        rxArray[i].mask = 0xFFFFU;
        rxArray[i].object = NULL;
        rxArray[i].CANrx_callback = NULL;
    }
    for (uint16_t i = 0U; i < txSize; i++) {
        txArray[i].bufferFull = false;
    }

    /***************************************/
    /* AT32 相关配置 */
    /***************************************/
    ((CANopenNodeAT32*)CANptr)->HWInitFunction(); //初始化 CAN 外围设备的函数，通常为 MX_CAN_Init */

    /*
     * 配置全局过滤器，用于在信息未通过任何其他过滤器时进行最后检查：
     *
     * 在本例中，我们不依赖硬件过滤器，而是执行软件过滤器
     *
     * 接受不匹配的标准 ID 报文
     * 拒绝不匹配的扩展 ID 报文
     */

    can_filter_init_type FilterConfig;
#if defined(CAN)
    FilterConfig.filter_number = 0;
#else
    if (((can_type*)((CANopenNodeAT32*)CANmodule->CANptr)->CANHandle) == CAN1) 
    {
        FilterConfig.filter_number = 0;
    } 
    else 
    {
        FilterConfig.filter_number = 14;
    }
#endif
    FilterConfig.filter_mode = CAN_FILTER_MODE_ID_MASK;
    FilterConfig.filter_bit = CAN_FILTER_32BIT;
    FilterConfig.filter_id_high = 0x0;
    FilterConfig.filter_id_low = 0x0;
    FilterConfig.filter_mask_high = 0x0;
    FilterConfig.filter_mask_low = 0x0;
    FilterConfig.filter_fifo = CAN_FILTER_FIFO0;

    FilterConfig.filter_activate_enable = TRUE;

    can_filter_init(((CANopenNodeAT32*)CANptr)->CANHandle, &FilterConfig);

    /* 启用通知 */
    /* 激活 CAN 通知中断 */
    can_interrupt_enable(((CANopenNodeAT32*)CANptr)->CANHandle, CAN_RF0MIEN_INT, TRUE);// 接收 FIFO 0 报文接收中断使能
    can_interrupt_enable(((CANopenNodeAT32*)CANptr)->CANHandle, CAN_RF1MIEN_INT, TRUE);// 接收 FIFO 1 报文接收中断使能
    can_interrupt_enable(((CANopenNodeAT32*)CANptr)->CANHandle, CAN_TCIEN_INT, TRUE);// 发送邮箱发送完成中断使能

    return CO_ERROR_NO;
}

/******************************************************************************/
void CO_CANmodule_disable(CO_CANmodule_t* CANmodule) 
{
    if (CANmodule != NULL && CANmodule->CANptr != NULL) 
    {
        // 将can 寄存器值复位到默认值
				//can_reset(((CANopenNodeAT32*)CANmodule->CANptr)->CANHandle);
			// CAN 工作模式设置:冻结模式--用于 CAN 控制器初始化
			can_operating_mode_set(((CANopenNodeAT32*)CANmodule->CANptr)->CANHandle,CAN_OPERATINGMODE_FREEZE);
    }
}

/******************************************************************************/
CO_ReturnError_t
CO_CANrxBufferInit(CO_CANmodule_t* CANmodule, uint16_t index, uint16_t ident, uint16_t mask, bool_t rtr, void* object,
                   void (*CANrx_callback)(void* object, void* message)) {
    CO_ReturnError_t ret = CO_ERROR_NO;

    if (CANmodule != NULL && object != NULL && CANrx_callback != NULL && index < CANmodule->rxSize) {
        CO_CANrx_t* buffer = &CANmodule->rxArray[index];

        /* 配置对象变量 */
        buffer->object = object;
        buffer->CANrx_callback = CANrx_callback;

        /*
         * 配置全局标识符，包括 RTR 位
         *
         * 随后将用于 RX 操作匹配情况
         */
        buffer->ident = (ident & CANID_MASK) | (rtr ? FLAG_RTR : 0x00);
        buffer->mask = (mask & CANID_MASK) | FLAG_RTR;

        /* 设置 CAN 硬件模块过滤器和屏蔽。 */
        if (CANmodule->useCANrxFilters) {
            __NOP();
        }
    } else {
        ret = CO_ERROR_ILLEGAL_ARGUMENT;
    }

    return ret;
}

/******************************************************************************/
CO_CANtx_t*
CO_CANtxBufferInit(CO_CANmodule_t* CANmodule, uint16_t index, uint16_t ident, bool_t rtr, uint8_t noOfBytes,
                   bool_t syncFlag) {
    CO_CANtx_t* buffer = NULL;

    if (CANmodule != NULL && index < CANmodule->txSize) {
        buffer = &CANmodule->txArray[index];

        /* CAN 标识符、DLC 和 rtr，与 CAN 模块发送缓冲区的位对齐 */
        buffer->ident = ((uint32_t)ident & CANID_MASK) | ((uint32_t)(rtr ? FLAG_RTR : 0x00));
        buffer->DLC = noOfBytes;
        buffer->bufferFull = false;
        buffer->syncFlag = syncFlag;
    }
    return buffer;
}

/**
 * \brief           向网络发送 CAN 报文
 * 该函数必须以原子访问方式调用。
 *
 * \param[in]       CANmodule: CAN 模块实例
 * \param[in]       buffer: 传输缓冲区的指针
 */
static uint8_t prv_send_can_message(CO_CANmodule_t* CANmodule, CO_CANtx_t* buffer) 
{
    uint8_t transmit_mailbox = 0;  
    /* 检查 TX FIFO 是否已准备好接受更多报文 */
    static can_tx_message_type tx_hdr;
    /*
            * RTR 标志是标识符值的一部分
            * 因此需要正确解码
            */
    tx_hdr.extended_id = 0u;
    tx_hdr.id_type = CAN_ID_STANDARD; // 标准标识符
    tx_hdr.dlc = buffer->DLC;
    tx_hdr.standard_id = buffer->ident & CANID_MASK;
    tx_hdr.frame_type = (buffer->ident & FLAG_RTR) ? CAN_TFT_REMOTE : CAN_TFT_DATA;
    tx_hdr.data[0] = buffer->data[0];
    tx_hdr.data[1] = buffer->data[1];
    tx_hdr.data[2] = buffer->data[2];
    tx_hdr.data[3] = buffer->data[3];
    tx_hdr.data[4] = buffer->data[4];
    tx_hdr.data[5] = buffer->data[5];
    tx_hdr.data[6] = buffer->data[6];
    tx_hdr.data[7] = buffer->data[7];

    /* 现在将信息添加到 FIFO。不应失败 */
    transmit_mailbox = can_message_transmit(((CANopenNodeAT32*)CANmodule->CANptr)->CANHandle, &tx_hdr);
		
    if(transmit_mailbox == CAN_TX_STATUS_NO_EMPTY) // 意思是没有空邮箱，无法填写信息
    {
        return ERROR;
    }
    else
    {
        return SUCCESS;
    }
  
}

/******************************************************************************/
CO_ReturnError_t CO_CANsend(CO_CANmodule_t* CANmodule, CO_CANtx_t* buffer) 
{
    CO_ReturnError_t err = CO_ERROR_NO;

    /* 验证溢流 */
    if (buffer->bufferFull) {
        if (!CANmodule->firstCANtxMessage) 
        {
            /* 如果缓冲区中仍有启动信息，则不设置错误 */
            CANmodule->CANerrorStatus |= CO_CAN_ERRTX_OVERFLOW;
        }
        err = CO_ERROR_TX_OVERFLOW;
    }

    /*
     * 向 CAN 网络发送信息
     *
     * 锁定中断以进行原子操作
     */
    CO_LOCK_CAN_SEND(CANmodule);
    if (prv_send_can_message(CANmodule, buffer)) 
    {
        CANmodule->bufferInhibitFlag = buffer->syncFlag;
    } 
    else 
    {
        buffer->bufferFull = true;
        CANmodule->CANtxCount++;
    }
    CO_UNLOCK_CAN_SEND(CANmodule);

    return err;
}

/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t* CANmodule) 
{
    uint32_t tpdoDeleted = 0U;

    CO_LOCK_CAN_SEND(CANmodule);
    /* 如果有同步 TPDO，则 CAN 模块发出终止报文。
     * 使用此功能时要特别小心。 */
    if (/*messageIsOnCanBuffer && */ CANmodule->bufferInhibitFlag) 
    {
        /* 清除 TXREQ */
        CANmodule->bufferInhibitFlag = false;
        tpdoDeleted = 1U;
    }
    /* 同时删除 TX 缓冲区中的待处理同步 TPDO */
    if (CANmodule->CANtxCount > 0) 
    {
        for (uint16_t i = CANmodule->txSize; i > 0U; --i) 
        {
            if (CANmodule->txArray[i].bufferFull) 
            {
                if (CANmodule->txArray[i].syncFlag) 
                {
                    CANmodule->txArray[i].bufferFull = false;
                    CANmodule->CANtxCount--;
                    tpdoDeleted = 2U;
                }
            }
        }
    }
    CO_UNLOCK_CAN_SEND(CANmodule);
    if (tpdoDeleted) 
    {
        CANmodule->CANerrorStatus |= CO_CAN_ERRTX_PDO_LATE;
    }
}

/******************************************************************************/
/* 从模块中获取错误计数器。如有必要,函数可使用不同的方式确定错误 */
//static uint16_t rxErrors = 0, txErrors = 0, overflow = 0;

void CO_CANmodule_process(CO_CANmodule_t* CANmodule) 
{
    uint32_t err = 0;

    // CANOpen 只关心总线关闭、警告、被动和溢出。
    // 我在 AT32 中没有找到溢出错误寄存器，如果您找到了，请告诉我。

    err = ((can_type*)((CANopenNodeAT32*)CANmodule->CANptr)->CANHandle)->ests & (CAN_ESR_BOFF | CAN_ESR_EPVF | CAN_ESR_EWGF);

    if (CANmodule->errOld != err) 
    {
        uint16_t status = CANmodule->CANerrorStatus;

        CANmodule->errOld = err;

        if (err & CAN_ESR_BOFF) 
        {
            status |= CO_CAN_ERRTX_BUS_OFF;
            // 在该驱动程序中，我们假定总线自动恢复功能已激活！因此，该错误最终将自动得到处理。
        } 
        else 
        {
            /* 重新计算 CANerrorStatus，首先清除一些标记 */
            status &= 0xFFFF
                      ^ (CO_CAN_ERRTX_BUS_OFF | CO_CAN_ERRRX_WARNING | CO_CAN_ERRRX_PASSIVE | CO_CAN_ERRTX_WARNING
                         | CO_CAN_ERRTX_PASSIVE);

            if (err & CAN_ESR_EWGF) {
                status |= CO_CAN_ERRRX_WARNING | CO_CAN_ERRTX_WARNING;
            }

            if (err & CAN_ESR_EPVF) {
                status |= CO_CAN_ERRRX_PASSIVE | CO_CAN_ERRTX_PASSIVE;
            }
        }

        CANmodule->CANerrorStatus = status;
    }
}

/**
 * \brief           从 RX FIFO 读取报文
 * \param           hfdcan: 指向 FDCAN_HandleTypeDef 结构的指针，该结构包含指定 FDCAN 的配置信息。
 * \param[in]       fifo: 用于读取的 Fifo 号码
 */
static void prv_read_can_received_msg(can_type*  hcan, can_rx_fifo_num_type fifo)
{
    CO_CANrxMsg_t rcvMsg;
    CO_CANrx_t* buffer = NULL; /* 从 CO_CANmodule_t 对象接收信息缓冲区。 */
    uint16_t index;            /* 收到信息的索引 */
    uint32_t rcvMsgIdent;      /* 收到信息的标识符 */
    uint8_t messageFound = 0;

    static can_rx_message_type rx_hdr;
    /* 从 FIFO 读取收到的报文 */
    can_message_receive(hcan, fifo, &rx_hdr);
    
    /* 设置标识符（带 RTR）和长度 */
    rcvMsg.data[0] = rx_hdr.data[0];
    rcvMsg.data[1] = rx_hdr.data[1];
    rcvMsg.data[2] = rx_hdr.data[2];
    rcvMsg.data[3] = rx_hdr.data[3];
    rcvMsg.data[4] = rx_hdr.data[4];
    rcvMsg.data[5] = rx_hdr.data[5];
    rcvMsg.data[6] = rx_hdr.data[6];
    rcvMsg.data[7] = rx_hdr.data[7];
    rcvMsg.ident = rx_hdr.standard_id | (rx_hdr.frame_type == CAN_TFT_REMOTE ? FLAG_RTR : 0x00);
    rcvMsg.dlc = rx_hdr.dlc;
    rcvMsgIdent = rcvMsg.ident;
    
    /*
     * 暂时不使用硬件过滤器
     * \todo: 实施硬件过滤器...
     */
    if (CANModule_local->useCANrxFilters) 
    {
        __BKPT(0);
    } 
    else 
    {
        /*
         * 我们没有使用硬件过滤器，因此需要手动将
         * 接收到的报文 ID 与所有缓冲区进行匹配
         */
        buffer = CANModule_local->rxArray;
        for (index = CANModule_local->rxSize; index > 0U; --index, ++buffer) 
        {
            if (((rcvMsgIdent ^ buffer->ident) & buffer->mask) == 0U) 
            {
                messageFound = 1;
                break;
            }
        }
    }

    /* 调用指定函数，该函数将处理报文 */
    if (messageFound && buffer != NULL && buffer->CANrx_callback != NULL) 
    {
        buffer->CANrx_callback(buffer->object, (void*)&rcvMsg);
    }
}


/**
  * @brief  this function handles USB Low  Priority or CAN1 RX0 handler.
  * @param  none
  * @retval none
  */
void USBFS_L_CAN1_RX0_IRQHandler(void)
{
  /* add user code begin USBFS_L_CAN1_RX0_IRQ 0 */
    if(can_flag_get(CAN1, CAN_RF0MN_FLAG) == SET) // FIFO0 非空标志
    {
        prv_read_can_received_msg(CAN1, CAN_RX_FIFO0);
    }
  /* add user code end USBFS_L_CAN1_RX0_IRQ 0 */
  /* add user code begin USBFS_L_CAN1_RX0_IRQ 1 */

  /* add user code end USBFS_L_CAN1_RX0_IRQ 1 */
}

/**
  * @brief  this function handles CAN1 RX1 handler.
  * @param  none
  * @retval none
  */
void CAN1_RX1_IRQHandler(void)
{
  /* add user code begin CAN1_RX1_IRQ 0 */
    if(can_flag_get(CAN1, CAN_RF1MN_FLAG) == SET) // FIFO1 非空标志
    {
        prv_read_can_received_msg(CAN1, CAN_RX_FIFO1);
    }
  /* add user code end CAN1_RX1_IRQ 0 */
  /* add user code begin CAN1_RX1_IRQ 1 */

  /* add user code end CAN1_RX1_IRQ 1 */
}

/**
 * \brief           TX 缓冲区已顺利传输 回调
 * \param[in]       hcan: 指向 CAN_HandleTypeDef 结构的指针，该结构包含指定 CAN 的配置信息。
 */
void CO_CANinterrupt_TX(CO_CANmodule_t* CANmodule) 
{
    CANmodule->firstCANtxMessage = false;            /* 第一条 CAN 报文（启动）发送成功 */
    CANmodule->bufferInhibitFlag = false;            /* 清除上一条信息的标记 */
    if (CANmodule->CANtxCount > 0U)                  /* 是否有新信息等待发送 */
    {                
        CO_CANtx_t* buffer = &CANmodule->txArray[0]; /* 从第一个缓冲区句柄开始 */
        uint16_t i = 0;
        /*
		 * 尝试发送更多缓冲区，处理所有空缓冲区
		 *
		 * 该函数总是从中断中调用，但为了确保不会发生抢占，中断无论如何都会被锁定
		 * 除非您能保证没有更高优先级的中断会尝试访问 CAN 实例并发送数据，否则就没有必要锁定中断）。
		 */
        CO_LOCK_CAN_SEND(CANmodule);
        for (i = CANmodule->txSize; i > 0U; --i, ++buffer) 
        {
            /* 尝试发送信息 */
            if (buffer->bufferFull) 
            {
                if (prv_send_can_message(CANmodule, buffer)) 
                {
                    buffer->bufferFull = false;
                    CANmodule->CANtxCount--;
                    CANmodule->bufferInhibitFlag = buffer->syncFlag;
                }
            }
        }
        /* 如果不再有信息，则清除计数器 */
        if (i == 0U) {
            CANmodule->CANtxCount = 0U;
        }
        CO_UNLOCK_CAN_SEND(CANmodule);
    }
}

/**
  * @brief  this function handles USB High Priority or CAN1 TX handler.
  * @param  none
  * @retval none
  */
void USBFS_H_CAN1_TX_IRQHandler(void)
{
  /* add user code begin USBFS_H_CAN1_TX_IRQ 0 */
     if(can_flag_get(CAN1, CAN_TMEF_FLAG) == SET) // 发送邮箱空标志（三个发送邮箱任一为空）
     {
			 can_flag_clear(CAN1, CAN_TMEF_FLAG); // 清除选定的CAN 标志
       CO_CANinterrupt_TX(CANModule_local);
     }
     
  /* add user code end USBFS_H_CAN1_TX_IRQ 0 */
  /* add user code begin USBFS_H_CAN1_TX_IRQ 1 */

  /* add user code end USBFS_H_CAN1_TX_IRQ 1 */
}

/**
  * @brief  this function handles CAN1 SE handler.
  * @param  none
  * @retval none
  */
void CAN1_SE_IRQHandler(void)
{
  /* add user code begin CAN1_SE_IRQ 0 */

  /* add user code end CAN1_SE_IRQ 0 */
  /* add user code begin CAN1_SE_IRQ 1 */

  /* add user code end CAN1_SE_IRQ 1 */
}

