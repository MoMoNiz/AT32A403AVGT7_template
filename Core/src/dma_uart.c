#include "at32a403a_wk_config.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "dma_uart.h"

//定义串口1 
DMA_Uart Uart1 =
{
	.UartTxBuf 		= {0},							//发送数据缓存
	.UartRxBuf 		= {0},							//接受数据缓存
	.Uart_Rx_flag = 0,								//串口接收标志位
  .Uart_Rx_len 	= 0,								//串口接收的实际长度
	.Uart_Tx_len  = 0,								//串口发送的实际长度
	.dma_tx_flag  = DMA1_FDT2_FLAG,		//DMA发送通道传输完成标志
	.usart_x = USART1,								//串口号
	.dma_rx_channely = DMA1_CHANNEL1, //DMA接收通道
	.dma_tx_channely = DMA1_CHANNEL2, //DMA发送通道
};

/* code ------------------------------------------------------------------*/
/**
* @brief      DMA串口空闲中断初始化
* @param      DMA_Uart *Uart
* @retval     None
* @attention  需要把此函数放到main.c文件的程序初始化操作里,才能使能中断接收
*/
void UART_DMA_Init(DMA_Uart *Uart)
{
  dma_channel_enable(Uart->dma_rx_channely, FALSE);     //失能DMA接收
	
	wk_dma_channel_config(Uart->dma_rx_channely, (uint32_t)&(Uart->usart_x->dt), (uint32_t)(Uart->UartRxBuf), rx_buf_size);
  dma_channel_enable(Uart->dma_rx_channely, TRUE);			//使能DMA接收
	
	dma_channel_enable(Uart->dma_tx_channely, FALSE);     //失能DMA发送
	wk_dma_channel_config(Uart->dma_tx_channely, (uint32_t)&(Uart->usart_x->dt), (uint32_t)(Uart->UartTxBuf), 0);
	
	// 使能串口空闲中断
	usart_interrupt_enable(Uart->usart_x,USART_IDLE_INT,TRUE);
}

/**
* @brief      DMA串口空闲中断不定长接收。
* @param      None
* @retval     None
* @attention  需要把此函数放到at32xxxx_int.c文件的相应串口中断函数UARTX_IRQHandler()里,才能通过中断接收
*/
void UART_Receive_Interrupt(DMA_Uart *Uart)
{
	static uint16_t temp=0;
	
	if(usart_flag_get(Uart->usart_x,USART_IDLEF_FLAG) != RESET) //获得空闲中断标志
	{   
		usart_data_receive(Uart->usart_x);  											//清除空闲标志
		dma_channel_enable(Uart->dma_rx_channely, FALSE);     		//失能DMA接收
		temp = dma_data_number_get(Uart->dma_rx_channely);				//读取接收长度，总大小-剩余大小
		Uart->Uart_Rx_len = rx_buf_size - temp; 
		Uart->Uart_Rx_flag=1;                                 		//接收标志位置1
		Uart->dma_rx_channely->dtcnt = rx_buf_size;								//重新配置接收数量
		dma_channel_enable(Uart->dma_rx_channely, TRUE);      		//重新使能接收DMA接收
	}
}

/**
* @brief      串口接收并给字符串赋值。
* @param     *pData:需要赋值的字符串指针;length:这个字符串的长度
* @retval     flag:有数据接收到了就会置1,没有数据时会自动置零
* @attention  
*/
error_status UART_Receive_DMA(DMA_Uart *Uart,uint8_t *pData,uint8_t length)
{
	if(Uart->Uart_Rx_flag)    	// 当接收标志位=1时，说明有数据接收到了
	{
		Uart->Uart_Rx_flag=0;	                    // 清零接收标志位
		memset(pData,0, length);		          //先清空接收数组
    memcpy(pData,Uart->UartRxBuf,Uart->Uart_Rx_len);//再复制数组
		memset(Uart->UartRxBuf,0, rx_buf_size);		//读取完后需要清空接收数组,不然会出现意想不到的错误
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}

/**
* @brief      串口DMA发送函数
* @param      DMA_Uart *Uart
* @retval     None
* @attention  None
*/ 
error_status UART_Transmit_DMA(DMA_Uart *Uart)
{
	while(usart_flag_get(Uart->usart_x,USART_TDBE_FLAG) == RESET);	// 等待串口发送缓冲器为空 0:非空 1:空 ,为空时才能写入数据
	if (((Uart->UartTxBuf) == 0) || (Uart->Uart_Tx_len == 0))				// 有一个不满足就返回错误
	{
		return ERROR;
	}
	
	dma_channel_enable(Uart->dma_tx_channely, FALSE);           		// 关闭通道
	Uart->dma_tx_channely->dtcnt = Uart->Uart_Tx_len;								// 重新配置发送数量
	dma_channel_enable(Uart->dma_tx_channely, TRUE);								// 打开通道

	//while(dma_flag_get(Uart->dma_tx_flag) == RESET);								// 等待DMA通道传输完成
	//dma_flag_clear(Uart->dma_tx_flag);															// 清除DMA传输完成标志
	return SUCCESS;
}

/**
* @brief  串口DMA式printf重定向。
* @param  Uart:串口号, format:格式
* @retval 
* @attention
*/
void UART_Printf(DMA_Uart *Uart,const char *format,...)
{
	va_list args;	
	va_start(args,format);
	Uart->Uart_Tx_len = vsnprintf((char*)Uart->UartTxBuf,sizeof(Uart->UartTxBuf)+1,(char*)format,args);
	va_end(args);
	UART_Transmit_DMA(Uart);
}


/*加入以下代码,支持printf函数,而不需要选择use MicroLIB*/
#ifdef __CC_ARM
#pragma import(__use_no_semihosting)  
int _ttywrch(int ch)    
{
	ch=ch;
	return ch;
}           
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
FILE __stdout;       
void _sys_exit(int x) 
{ 
	x = x; 
} 

#endif
