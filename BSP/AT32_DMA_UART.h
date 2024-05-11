#ifndef __AT32_DMA_UART_H
#define __AT32_DMA_UART_H
/* Includes ------------------------------------------------------------------*/
#include "at32a403a.h"
/* defines ------------------------------------------------------------------*/
#define rx_buf_size 255     //默认存放255个字节数据
#define tx_buf_size 255

typedef struct
{
	uint8_t 	UartTxBuf[tx_buf_size];	//发送数据缓存
	uint8_t  	UartRxBuf[rx_buf_size];	//接受数据缓存
	uint8_t		Uart_Rx_flag;						//串口接收标志位
  uint16_t 	Uart_Rx_len;						//串口接收的实际长度
  UART_HandleTypeDef *huart;
}DMA_Uart;

extern DMA_Uart Uart2;
/* function ------------------------------------------------------------------*/
void Uart_printf(DMA_Uart *Uart,const char *format,...);
void Uart_receive_Init(DMA_Uart *Uart);
void Uart_receive(DMA_Uart *Uart);
uint8_t Uart_scanf(DMA_Uart *Uart,uint8_t *pData,uint8_t length);


#endif
