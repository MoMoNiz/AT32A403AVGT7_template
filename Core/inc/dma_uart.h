#ifndef __DMA_UART_H
#define __DMA_UART_H
/* includes -----------------------------------------------------------------------*/
#include "at32a403a.h"

/* defines ------------------------------------------------------------------*/
#define rx_buf_size 255     //�����255���ֽ�����
#define tx_buf_size 255

typedef struct
{
	uint8_t 	UartTxBuf[tx_buf_size];		//�������ݻ���
	uint8_t  	UartRxBuf[rx_buf_size];		//�������ݻ���
	uint8_t		Uart_Rx_flag;							//���ڽ��ձ�־λ
  uint16_t 	Uart_Rx_len;							//���ڽ��յ�ʵ�ʳ���
	uint16_t  Uart_Tx_len;							//���ڷ��͵�ʵ�ʳ���
	uint32_t  dma_tx_flag;							//DMA����ͨ��������ɱ�־
	usart_type *usart_x;								//���ں�
	dma_channel_type *dma_rx_channely;  //DMA����ͨ��
	dma_channel_type *dma_tx_channely;  //DMA����ͨ��
}DMA_Uart;

extern DMA_Uart Uart1;
/* function ------------------------------------------------------------------*/

void UART_DMA_Init(DMA_Uart *Uart);
void UART_Receive_Interrupt(DMA_Uart *Uart);
error_status UART_Receive_DMA(DMA_Uart *Uart,uint8_t *pData,uint8_t length);
error_status UART_Transmit_DMA(DMA_Uart *Uart);
void UART_Printf(DMA_Uart *Uart,const char *format,...);
#endif
