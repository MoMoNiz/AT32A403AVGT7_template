/*
 * CO_app_AT32.h
 *
 *  Created on: 2024.7.12
 *      Author: MoMoNiz
 */

#ifndef CANOPENSTM32_CO_APP_STM32_H_
#define CANOPENSTM32_CO_APP_STM32_H_

#include "CANopen.h"
#include "at32a403a_wk_config.h"

/* CANHandle : 为该功能输入 CAN 句柄，它将用于所有 CAN 通信。CANOpenAT32 驱动程序将负责处理它们。
 * HWInitFunction : 输入用于初始化 CAN 外围设备的函数，通常为 MX_CAN_Init
 * timerHandle : 输入用于为 tmrThread 函数产生 1ms 中断的定时器，
 * 请注意 CANOpenAT32 库将覆盖 HAL_TIM_PeriodElapsedCallback 函数，
 * 如果您的代码中也需要该函数，请采取必要的步骤。

 */

#ifdef __cplusplus
extern "C" {
#endif

/********************  CAN_ESR 寄存器的位定义  *******************/
#define CAN_ESR_EWGF_Pos                     (0U)                              
#define CAN_ESR_EWGF_Msk                     (0x1UL << CAN_ESR_EWGF_Pos)        /*!< 0x00000001 */
#define CAN_ESR_EWGF                         CAN_ESR_EWGF_Msk                  /*!< 错误主动标志 */
#define CAN_ESR_EPVF_Pos                     (1U)                              
#define CAN_ESR_EPVF_Msk                     (0x1UL << CAN_ESR_EPVF_Pos)        /*!< 0x00000002 */
#define CAN_ESR_EPVF                         CAN_ESR_EPVF_Msk                  /*!< 错误被动标志 */
#define CAN_ESR_BOFF_Pos                     (2U)                              
#define CAN_ESR_BOFF_Msk                     (0x1UL << CAN_ESR_BOFF_Pos)        /*!< 0x00000004 */
#define CAN_ESR_BOFF                         CAN_ESR_BOFF_Msk                  /*!< 总线关闭标志 */


typedef struct 
{
    uint8_t desiredNodeID; /*这是您要求 CANOpen 栈分配给您的设备的节点 ID，
	 * 尽管它可能并不总是最终的节点 ID，在调用 canopen_app_init() 后，
	 * 您应该检查 CANopenNodeAT32 结构的 ActiveNodeID 以获得分配的节点 ID。
	 */
    uint8_t activeNodeID; /* 分配的节点 ID */
    uint16_t baudrate;     /* 这是您在 AT32 WorkBench 配置中设置的波特率。 */
    tmr_type* timerHandle; /*请注意，CANOpenAT32 库将覆盖 HAL_TIM_PeriodElapsedCallback 函数，
	 * 如果您的代码中也需要该函数，请执行必要的步骤
	 */

    /* 为该功能输入 CAN 句柄，它将用于所有 CAN 通信。
	 * CANOpenAT32 驱动程序将负责处理它们。*/

    can_type* CANHandle;

    void (*HWInitFunction)(); /* 输入用于初始化 CAN 外围设备的函数，通常为 MX_CAN_Init */

    uint8_t outStatusLEDGreen; // 这将由堆栈更新 - 使用它们进行 LED 管理
    uint8_t outStatusLEDRed;   // 这将由堆栈更新 - 使用它们进行 LED 管理
    CO_t* canOpenStack;

} CANopenNodeAT32;


// 要使用 CANOpenAT32，您必须在代码中的某个地方设置 canopenNodeAT32 结构体，它通常位于 CO_app_AT32.c 中。
extern CANopenNodeAT32* canopenNodeAT32;


/* 该函数将初始化所需的 CANOpen 堆栈对象，分配内存并为通信重置准备堆栈。*/
int canopen_app_init(CANopenNodeAT32* canopenAT32);
/* 该功能将重置 CAN 通信 periperhal 和 CANOpen 堆栈变量 */
int canopen_app_resetCommunication(void);
/* 该函数将检查输入缓冲区和任何未完成的非关键任务，应定期从代码中调用该函数（即从 while(1) 中调用）。*/
void canopen_app_process(void);
/* 线程函数以恒定的时间间隔执行，可从 RTOS 任务或计时器中断调用该函数 ********/
void canopen_app_interrupt(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CANOPENAT32_CO_APP_AT32_H_ */
