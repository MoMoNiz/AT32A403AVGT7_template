#include "AT32_DMA_UART.h"
#include "at32a403a_wk_config.h"

//���崮��2 
DMA_Uart Uart2 =
{
	.UartTxBuf 		= {0},		//�������ݻ���
	.UartRxBuf 		= {0},		//�������ݻ���
	.Uart_Rx_flag = 0,			//���ڽ��ձ�־λ
  .Uart_Rx_len 	= 0,			//���ڽ��յ�ʵ�ʳ���
  .huart 				= &huart2,//����2
};




