#ifndef __AT32_DMA_UART_H
#define __AT32_DMA_UART_H
/* Includes ------------------------------------------------------------------*/
#include "at32a403a.h"
/* defines ------------------------------------------------------------------*/
#define rx_buf_size 255     //Ĭ�ϴ��255���ֽ�����
#define tx_buf_size 255

typedef struct
{
	uint8_t 	UartTxBuf[tx_buf_size];	//�������ݻ���
	uint8_t  	UartRxBuf[rx_buf_size];	//�������ݻ���
	uint8_t		Uart_Rx_flag;						//���ڽ��ձ�־λ
  uint16_t 	Uart_Rx_len;						//���ڽ��յ�ʵ�ʳ���
  UART_HandleTypeDef *huart;
}DMA_Uart;

extern DMA_Uart Uart2;
/* function ------------------------------------------------------------------*/
void Uart_printf(DMA_Uart *Uart,const char *format,...);
void Uart_receive_Init(DMA_Uart *Uart);
void Uart_receive(DMA_Uart *Uart);
uint8_t Uart_scanf(DMA_Uart *Uart,uint8_t *pData,uint8_t length);


#endif
