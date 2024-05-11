#ifndef __DMA_UART_H
#define __DMA_UART_H
/* includes -----------------------------------------------------------------------*/
#include "at32a403a.h"

/* defines ------------------------------------------------------------------*/
#define rx_buf_size 255     //最多存放255个字节数据
#define tx_buf_size 255

typedef struct
{
	uint8_t 	UartTxBuf[tx_buf_size];		//发送数据缓存
	uint8_t  	UartRxBuf[rx_buf_size];		//接受数据缓存
	uint8_t		Uart_Rx_flag;							//串口接收标志位
  uint16_t 	Uart_Rx_len;							//串口接收的实际长度
	uint16_t  Uart_Tx_len;							//串口发送的实际长度
	uint32_t  dma_tx_flag;							//DMA发送通道传输完成标志
	usart_type *usart_x;								//串口号
	dma_channel_type *dma_rx_channely;  //DMA接收通道
	dma_channel_type *dma_tx_channely;  //DMA发送通道
}DMA_Uart;

extern DMA_Uart Uart1;
/* function ------------------------------------------------------------------*/

void UART_DMA_Init(DMA_Uart *Uart);
void UART_Receive_Interrupt(DMA_Uart *Uart);
error_status UART_Receive_DMA(DMA_Uart *Uart,uint8_t *pData,uint8_t length);
error_status UART_Transmit_DMA(DMA_Uart *Uart);
void UART_Printf(DMA_Uart *Uart,const char *format,...);
#endif
