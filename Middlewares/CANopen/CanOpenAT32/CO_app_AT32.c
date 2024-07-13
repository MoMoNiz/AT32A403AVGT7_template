/*
 * CANopen main program file.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        main_generic.c
 * @author      Hamed Jafarzadeh 	2022
 * 				Janez Paternoster	2021
 * @copyright   2021 Janez Paternoster
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
 */
#include "CO_app_AT32.h"
#include "CANopen.h"
#include "at32a403a_wk_config.h"
#include "rtthread.h"
#include "dma_uart.h"

#include "CO_storageBlank.h"
#include "OD.h"

CANopenNodeAT32* canopenNodeAT32; // 它将由 canopen_app_init 设置，并在整个应用程序中用于访问 CANOpen 对象。

/* CanOpen 应用程序的 rt_kprintf 函数 */
#define log_printf(macropar_message, ...)  UART_Printf(&Uart1,macropar_message, ##__VA_ARGS__)

/* CO_CANopenInit() 的默认值 */
#define NMT_CONTROL                                                                                                    \
    CO_NMT_STARTUP_TO_OPERATIONAL                                                                                      \
    | CO_NMT_ERR_ON_ERR_REG | CO_ERR_REG_GENERIC_ERR | CO_ERR_REG_COMMUNICATION
#define FIRST_HB_TIME        500
#define SDO_SRV_TIMEOUT_TIME 1000
#define SDO_CLI_TIMEOUT_TIME 500
#define SDO_CLI_BLOCK        false
#define OD_STATUS_BITS       NULL

/* 全局变量和对象 */
CO_t* CO = NULL; /* CANopen 对象 */

// 全局变量
uint32_t time_old, time_current;
CO_ReturnError_t err;

/* 该功能主要用于设置 CANopen 节点 */
int canopen_app_init(CANopenNodeAT32* _canopenNodeAT32) 
{
    // 保存 canOpenAT32 对象的全局引用副本
    canopenNodeAT32 = _canopenNodeAT32;

#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
    CO_storage_t storage;
    CO_storage_entry_t storageEntries[] = {{.addr = &OD_PERSIST_COMM,
                                            .len = sizeof(OD_PERSIST_COMM),
                                            .subIndexOD = 2,
                                            .attr = CO_storage_cmd | CO_storage_restore,
                                            .addrNV = NULL}};
    uint8_t storageEntriesCount = sizeof(storageEntries) / sizeof(storageEntries[0]);
    uint32_t storageInitError = 0;
#endif

    /* 分配内存 */
    CO_config_t* config_ptr = NULL;
#ifdef CO_MULTIPLE_OD
    /* CO_MULTIPLE_OD 的示例用法（但此处仍为单一 OD） */
    CO_config_t co_config = {0};
    OD_INIT_CONFIG(co_config); /* OD.h 中的辅助宏 */
    co_config.CNT_LEDS = 1;
    co_config.CNT_LSS_SLV = 1;
    config_ptr = &co_config;
#endif /* CO_MULTIPLE_OD */

    uint32_t heapMemoryUsed;
    CO = CO_new(config_ptr, &heapMemoryUsed);
    if (CO == NULL) {
        log_printf("Error: Can't allocate memory\n");
				rt_thread_mdelay(10);
        return 1;
    } else {
        log_printf("Allocated %u bytes for CANopen objects\n", heapMemoryUsed);
				rt_thread_mdelay(10);
    }

    canopenNodeAT32->canOpenStack = CO;

#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
    err = CO_storageBlank_init(&storage, CO->CANmodule, OD_ENTRY_H1010_storeParameters,
                               OD_ENTRY_H1011_restoreDefaultParameters, storageEntries, storageEntriesCount,
                               &storageInitError);

    if (err != CO_ERROR_NO && err != CO_ERROR_DATA_CORRUPT) {
        log_printf("Error: Storage %d\n", storageInitError);
				rt_thread_mdelay(1);
        return 2;
    }
#endif

    canopen_app_resetCommunication();
    return 0;
}

int canopen_app_resetCommunication() {
    /* CANopen 通信复位 - 初始化 CANopen 对象 *******************/
    log_printf("CANopenNode - Reset communication...\n");
		rt_thread_mdelay(10);
    /* 等待 rt_thread。 */
    CO->CANmodule->CANnormal = false;

    /* 进入 CAN 配置。 */
    CO_CANsetConfigurationMode((void*)canopenNodeAT32);
    CO_CANmodule_disable(CO->CANmodule);

    /* 初始化 CANopen */
    err = CO_CANinit(CO, canopenNodeAT32, 0); // 正在 WorkBench 设置中设置 AT32 微控制器的比特率
    if (err != CO_ERROR_NO) {
        log_printf("Error: CAN initialization failed: %d\n", err);
				rt_thread_mdelay(10);
        return 1;
    }

    CO_LSS_address_t lssAddress = {.identity = {.vendorID = OD_PERSIST_COMM.x1018_identity.vendor_ID,
                                                .productCode = OD_PERSIST_COMM.x1018_identity.productCode,
                                                .revisionNumber = OD_PERSIST_COMM.x1018_identity.revisionNumber,
                                                .serialNumber = OD_PERSIST_COMM.x1018_identity.serialNumber}};
    err = CO_LSSinit(CO, &lssAddress, &canopenNodeAT32->desiredNodeID, &canopenNodeAT32->baudrate);
    if (err != CO_ERROR_NO) {
        log_printf("Error: LSS slave initialization failed: %d\n", err);
				rt_thread_mdelay(10);
        return 2;
    }

    canopenNodeAT32->activeNodeID = canopenNodeAT32->desiredNodeID;
    uint32_t errInfo = 0;

    err = CO_CANopenInit(CO,                   /* CANopen object */
                         NULL,                 /* alternate NMT */
                         NULL,                 /* alternate em */
                         OD,                   /* Object dictionary */
                         OD_STATUS_BITS,       /* Optional OD_statusBits */
                         NMT_CONTROL,          /* CO_NMT_control_t */
                         FIRST_HB_TIME,        /* firstHBTime_ms */
                         SDO_SRV_TIMEOUT_TIME, /* SDOserverTimeoutTime_ms */
                         SDO_CLI_TIMEOUT_TIME, /* SDOclientTimeoutTime_ms */
                         SDO_CLI_BLOCK,        /* SDOclientBlockTransfer */
                         canopenNodeAT32->activeNodeID, &errInfo);
    if (err != CO_ERROR_NO && err != CO_ERROR_NODE_ID_UNCONFIGURED_LSS) {
        if (err == CO_ERROR_OD_PARAMETERS) {
            log_printf("Error: Object Dictionary entry 0x%X\n", errInfo);
						rt_thread_mdelay(10);
        } else {
            log_printf("Error: CANopen initialization failed: %d\n", err);
						rt_thread_mdelay(10);
        }
        return 3;
    }

    err = CO_CANopenInitPDO(CO, CO->em, OD, canopenNodeAT32->activeNodeID, &errInfo);
    if (err != CO_ERROR_NO) {
        if (err == CO_ERROR_OD_PARAMETERS) {
            log_printf("Error: Object Dictionary entry 0x%X\n", errInfo);
						rt_thread_mdelay(10);
        } else {
            log_printf("Error: PDO initialization failed: %d\n", err);
						rt_thread_mdelay(10);
        }
        return 4;
    }

    /* 配置定时器中断功能，每 1 毫秒执行一次 */
    tmr_interrupt_enable(canopenNodeAT32->timerHandle,TMR_OVF_INT,TRUE);//1ms 中断
    /* 配置 CAN 发送和接收中断 */

    /* 配置 CANopen 回调等 */
    if (!CO->nodeIdUnconfigured) {

#if (CO_CONFIG_STORAGE) & CO_CONFIG_STORAGE_ENABLE
        if (storageInitError != 0) {
            CO_errorReport(CO->em, CO_EM_NON_VOLATILE_MEMORY, CO_EMC_HARDWARE, storageInitError);
        }
#endif
    } else {
        log_printf("CANopenNode - Node-id not initialized\n");
				rt_thread_mdelay(10);
    }

    /* 启动 CAN */
    CO_CANsetNormalMode(CO->CANmodule);

    log_printf("CANopenNode - Running...\n");
		rt_thread_mdelay(10);
    time_old = time_current = rt_tick_get();//获取系统时间
    return 0;
}

void canopen_app_process() 
{
    /* 正常程序执行循环 ******************************************/
    /* 获取上次函数调用后的时间差 */
    time_current = rt_tick_get();//获取系统时间

    if ((time_current - time_old) > 0) { // 确保超过 1 毫秒
        /* CANopen process */
        CO_NMT_reset_cmd_t reset_status;
        uint32_t timeDifference_us = (time_current - time_old) * 1000;
        time_old = time_current;
        reset_status = CO_process(CO, false, timeDifference_us, NULL);
        canopenNodeAT32->outStatusLEDRed = CO_LED_RED(CO->LEDs, CO_LED_CANopen);
        canopenNodeAT32->outStatusLEDGreen = CO_LED_GREEN(CO->LEDs, CO_LED_CANopen);

        if (reset_status == CO_RESET_COMM) {
            /* 从内存中删除对象 */
        	tmr_interrupt_enable(canopenNodeAT32->timerHandle,TMR_OVF_INT,FALSE);//1ms 中断
            CO_CANsetConfigurationMode((void*)canopenNodeAT32);
            CO_delete(CO);
            log_printf("CANopenNode Reset Communication request\n");
						rt_thread_mdelay(10);
            canopen_app_init(canopenNodeAT32); // 重置通信程序
        } else if (reset_status == CO_RESET_APP) {
            log_printf("CANopenNode Device Reset\n");
						rt_thread_mdelay(10);
            nvic_system_reset(); // 复位 AT32 微控制器
        }
    }
}

/* 线程函数以恒定的时间间隔执行，可从 RTOS 任务或计时器调用该函数 ********/
void canopen_app_interrupt(void) 
{
    CO_LOCK_OD(CO->CANmodule);
    if (!CO->nodeIdUnconfigured && CO->CANmodule->CANnormal) {
        bool_t syncWas = false;
        /* 获取上次函数调用后的时间差 */
        uint32_t timeDifference_us = 1000; // 1ms second

#if (CO_CONFIG_SYNC) & CO_CONFIG_SYNC_ENABLE
        syncWas = CO_process_SYNC(CO, timeDifference_us, NULL);
#endif
#if (CO_CONFIG_PDO) & CO_CONFIG_RPDO_ENABLE
        CO_process_RPDO(CO, syncWas, timeDifference_us, NULL);
#endif
#if (CO_CONFIG_PDO) & CO_CONFIG_TPDO_ENABLE
        CO_process_TPDO(CO, syncWas, timeDifference_us, NULL);
#endif

        /* 更多的 I/O 或非阻塞应用代码可以放在这里。 */
    }
    CO_UNLOCK_OD(CO->CANmodule);
}
