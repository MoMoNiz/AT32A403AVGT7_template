/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32a403a_wk_config.h"
#include "rtthread.h"
#include "rt_thread.h"
#include "cpu_usage.h"
#include "dma_uart.h"
#include "test_demo.h"
/* private includes ----------------------------------------------------------*/

/* private typedef -----------------------------------------------------------*/


/* private define ------------------------------------------------------------*/


/* private macro -------------------------------------------------------------*/


/* private variables ---------------------------------------------------------*/


/* private function prototypes --------------------------------------------*/


/* private user code ---------------------------------------------------------*/

uint8_t char_buf_1[255] = {0};
/**
  * @brief main function.
  * @param  none
  * @retval none
  * @note 当添加 RT-Thread 之后,裸机中的 main() 函数会自动变成 RT-Thread 系统中 main 线程 的入口函数。
  * 由于线程不能一直独占 CPU,所以此时在 main() 中使用 while(1) 时,需要有让出 CPU 的动作,
  * 比如使用 rt_thread_mdelay() 系列的函数让出 CPU。
  */
int main(void)
{
  /*在 void rt_hw_board_init() 中已完成初始化*/

  thread_init();
	
  while(1)
  {
    /* add user code begin 3 */
    //rt_kprintf("hello\r\n");
		//UART_Printf(&Uart1,"hellohellohello\r\n");
		//UART_Printf(&Uart1,"worldworldworld\r\n");
		//UART_Printf(&Uart1,"cpu load average: %.1f\r\n",cpu_load_average());
		if(UART_Receive_DMA(&Uart1,char_buf_1,sizeof(char_buf_1)))
		{
			UART_Printf(&Uart1,"%s\r\n",char_buf_1);//把接收到的字符再发送出去
			rt_thread_mdelay(10);
			UART_Printf(&Uart1,"cpu load average: %.1f\r\n",cpu_load_average());
			rt_thread_mdelay(10);
		}
    gpio_bits_write(LED3_GPIO_PORT,LED3_PIN,TRUE);
    rt_thread_mdelay(50);
    gpio_bits_write(LED3_GPIO_PORT,LED3_PIN,FALSE);
    rt_thread_mdelay(50);
    /* add user code end 3 */
  }
}
