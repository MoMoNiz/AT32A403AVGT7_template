#include "AT32_DMA_UART.h"
#include "at32a403a_wk_config.h"

//定义串口2 
DMA_Uart Uart2 =
{
	.UartTxBuf 		= {0},		//发送数据缓存
	.UartRxBuf 		= {0},		//接受数据缓存
	.Uart_Rx_flag = 0,			//串口接收标志位
  .Uart_Rx_len 	= 0,			//串口接收的实际长度
  .huart 				= &huart2,//串口2
};




