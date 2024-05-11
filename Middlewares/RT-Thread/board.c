/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 */
 #include "at32a403a_wk_config.h"
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include "dma_uart.h"
#include "cpu_usage.h"

#define _SCB_BASE       (0xE000E010UL)
#define _SYSTICK_CTRL   (*(rt_uint32_t *)(_SCB_BASE + 0x0))
#define _SYSTICK_LOAD   (*(rt_uint32_t *)(_SCB_BASE + 0x4))
#define _SYSTICK_VAL    (*(rt_uint32_t *)(_SCB_BASE + 0x8))
#define _SYSTICK_CALIB  (*(rt_uint32_t *)(_SCB_BASE + 0xC))
#define _SYSTICK_PRI    (*(rt_uint8_t  *)(0xE000ED23UL))


#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 1024
static uint32_t rt_heap[RT_HEAP_SIZE];     // heap default size: 4K(1024 * 4)
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/**
 * 硬件初始化。
 */
void rt_hw_board_init()
{
	/*------------------------------------放入workbench生成的main函数初始化代码------------------------*/
  /* add user code begin 1 */

  /* add user code end 1 */

  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /* init debug function. */
  wk_debug_config();

  /* config systick clock source */
  systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_DIV8);
  /* system tick config */
  /**
   * use systick as time base source and configure 1ms tick.
   * users need add interrupt handler code into the below function in the at32a403a_int.c file.
   *  --void SystTick_IRQHandler(void)
   */
  systick_interrupt_config(system_core_clock / 8 / 1000);

  /* nvic config. */
  wk_nvic_config();

  /* init dma1 channel1 */
  wk_dma1_channel1_init();
  /* config dma channel transfer parameter */
  /* user need to modify parameters memory_base_addr and buffer_size */
  wk_dma_channel_config(DMA1_CHANNEL1, (uint32_t)&USART1->dt, 0, 0);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* init dma1 channel2 */
  wk_dma1_channel2_init();
  /* config dma channel transfer parameter */
  /* user need to modify parameters memory_base_addr and buffer_size */
  wk_dma_channel_config(DMA1_CHANNEL2, (uint32_t)&USART1->dt, 0, 0);
  dma_channel_enable(DMA1_CHANNEL2, TRUE);

  /* init usart1 function. */
  wk_usart1_init();

	/* init usart2 function. */
  wk_usart2_init();
	
  /* init gpio function. */
  wk_gpio_config();

  /* add user code begin 2 */
	UART_DMA_Init(&Uart1);			//DMA串口初始化
	
  /* add user code end 2 */
	/*------------------------------------直到main函数的while(1)前--------------------------------------*/
	
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

/* cortex-m 架构使用 SysTick_Handler() */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();   /* 进入中断时必须调用 */

    rt_tick_increase();     /* RT-Thread 系统时钟计数 */

    /* leave interrupt */
    rt_interrupt_leave();   /* 退出中断时必须调用 */
}

