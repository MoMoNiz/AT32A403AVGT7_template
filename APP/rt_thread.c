#include "rt_thread.h"
#include "rtthread.h"
#include "at32a403a_wk_config.h"
#include "dma_uart.h"
#include "test_demo.h"

#include "CO_app_AT32.h"
#include "OD.h"

#define THREAD_PRIORITY 3			// 优先级
#define THREAD_TIMESLICE 10  	// 时间片

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1096];
static struct rt_thread thread1;

static char thread2_stack[1024];
static struct rt_thread thread2;

/* 线程1 的入口函数*/
static void thread1_entry(void *parameter)
{
	CANopenNodeAT32 canOpenNodeAT32;
  canOpenNodeAT32.CANHandle = CAN1;
  canOpenNodeAT32.HWInitFunction = wk_can1_init;
  canOpenNodeAT32.timerHandle = TMR14;
  canOpenNodeAT32.desiredNodeID = 1;
  canOpenNodeAT32.baudrate = 125;
  canopen_app_init(&canOpenNodeAT32);
  while(1)
  {
		canopen_app_process();// CANopen进程
		rt_thread_mdelay(1);
  }
}

/* 线程2 入口*/
static void thread2_entry(void *param)
{
	while(1)
	{
		gpio_bits_write(LED2_GPIO_PORT,LED2_PIN,TRUE);
		rt_thread_mdelay(200);
		gpio_bits_write(LED2_GPIO_PORT,LED2_PIN,FALSE);
		rt_thread_mdelay(200);
	}
}

/* 线程初始化*/
int thread_init(void)
{
	/* 初始化线程1， 名称是thread1， 入口是thread1_entry */
	rt_thread_init(&thread1,
									"thread1",
									thread1_entry,
									RT_NULL,
									&thread1_stack[0],
									sizeof(thread1_stack),
									THREAD_PRIORITY, THREAD_TIMESLICE);
	rt_thread_startup(&thread1);

	/* 初始化线程2， 名称是thread2， 入口是thread2_entry */
	rt_thread_init(&thread2,
									"thread2",
									thread2_entry,
									RT_NULL,
									&thread2_stack[0],
									sizeof(thread2_stack),
									THREAD_PRIORITY - 1, THREAD_TIMESLICE);
	rt_thread_startup(&thread2);

	return 0;
}

/* finsh组件用的串口收发函数
***********************************************************************************************************/
/**
* @brief      finsh 输出一个字符,系统函数,函数名不可更改
* @param      
* @retval     None
* @attention  RT-Thread 系统中已有的打印均以 \n 结尾，而并非 \r\n,
*             所以在字符输出时，需要在输出 \n 之前输出 \r，完成回车与换行,
*             否则系统打印出来的信息将只有换行。
*/
void rt_hw_console_output(const char *str)
{
	static rt_size_t i = 0; 
	static rt_size_t size = 0;

	size = rt_strlen(str);
	
	for (i = 0; i < size; i++)
	{
	  if (*(str + i) == '\n')
		{
			while(usart_flag_get(USART2, USART_TDBE_FLAG) == RESET);
			usart_data_transmit(USART2, '\r');
		}
		while(usart_flag_get(USART2, USART_TDBE_FLAG) == RESET);
		usart_data_transmit(USART2, (uint16_t)*(str + i));
	}
	
}

/**
* @brief      finsh 获取一个字符,系统函数,函数名不可更改
* @param      None
* @retval     
* @attention  
*/
char rt_hw_console_getchar(void)
{
  int ch = -1;

	if (usart_flag_get(USART2, USART_RDBF_FLAG) == SET)	//如果接收到数据
	{
		ch = usart_data_receive(USART2);
	}
	else
	{
		if(usart_flag_get(USART2, USART_ROERR_FLAG) == SET) //如果接收溢出
		{
			usart_flag_clear(USART2,USART_ROERR_FLAG);
		}
		rt_thread_mdelay(10);
	}

  return ch;
}
