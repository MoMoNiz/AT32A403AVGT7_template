#include "at32a403a_wk_config.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "dma_uart.h"

//���崮��1 
DMA_Uart Uart1 =
{
	.UartTxBuf 		= {0},							//�������ݻ���
	.UartRxBuf 		= {0},							//�������ݻ���
	.Uart_Rx_flag = 0,								//���ڽ��ձ�־λ
  .Uart_Rx_len 	= 0,								//���ڽ��յ�ʵ�ʳ���
	.Uart_Tx_len  = 0,								//���ڷ��͵�ʵ�ʳ���
	.dma_tx_flag  = DMA1_FDT2_FLAG,		//DMA����ͨ��������ɱ�־
	.usart_x = USART1,								//���ں�
	.dma_rx_channely = DMA1_CHANNEL1, //DMA����ͨ��
	.dma_tx_channely = DMA1_CHANNEL2, //DMA����ͨ��
};

/* code ------------------------------------------------------------------*/
/**
* @brief      DMA���ڿ����жϳ�ʼ��
* @param      DMA_Uart *Uart
* @retval     None
* @attention  ��Ҫ�Ѵ˺����ŵ�main.c�ļ��ĳ����ʼ��������,����ʹ���жϽ���
*/
void UART_DMA_Init(DMA_Uart *Uart)
{
  dma_channel_enable(Uart->dma_rx_channely, FALSE);     //ʧ��DMA����
	
	wk_dma_channel_config(Uart->dma_rx_channely, (uint32_t)&(Uart->usart_x->dt), (uint32_t)(Uart->UartRxBuf), rx_buf_size);
  dma_channel_enable(Uart->dma_rx_channely, TRUE);			//ʹ��DMA����
	
	dma_channel_enable(Uart->dma_tx_channely, FALSE);     //ʧ��DMA����
	wk_dma_channel_config(Uart->dma_tx_channely, (uint32_t)&(Uart->usart_x->dt), (uint32_t)(Uart->UartTxBuf), 0);
	
	// ʹ�ܴ��ڿ����ж�
	usart_interrupt_enable(Uart->usart_x,USART_IDLE_INT,TRUE);
}

/**
* @brief      DMA���ڿ����жϲ��������ա�
* @param      None
* @retval     None
* @attention  ��Ҫ�Ѵ˺����ŵ�at32xxxx_int.c�ļ�����Ӧ�����жϺ���UARTX_IRQHandler()��,����ͨ���жϽ���
*/
void UART_Receive_Interrupt(DMA_Uart *Uart)
{
	static uint16_t temp=0;
	
	if(usart_flag_get(Uart->usart_x,USART_IDLEF_FLAG) != RESET) //��ÿ����жϱ�־
	{   
		usart_data_receive(Uart->usart_x);  											//������б�־
		dma_channel_enable(Uart->dma_rx_channely, FALSE);     		//ʧ��DMA����
		temp = dma_data_number_get(Uart->dma_rx_channely);				//��ȡ���ճ��ȣ��ܴ�С-ʣ���С
		Uart->Uart_Rx_len = rx_buf_size - temp; 
		Uart->Uart_Rx_flag=1;                                 		//���ձ�־λ��1
		Uart->dma_rx_channely->dtcnt = rx_buf_size;								//�������ý�������
		dma_channel_enable(Uart->dma_rx_channely, TRUE);      		//����ʹ�ܽ���DMA����
	}
}

/**
* @brief      ���ڽ��ղ����ַ�����ֵ��
* @param     *pData:��Ҫ��ֵ���ַ���ָ��;length:����ַ����ĳ���
* @retval     flag:�����ݽ��յ��˾ͻ���1,û������ʱ���Զ�����
* @attention  
*/
error_status UART_Receive_DMA(DMA_Uart *Uart,uint8_t *pData,uint8_t length)
{
	if(Uart->Uart_Rx_flag)    	// �����ձ�־λ=1ʱ��˵�������ݽ��յ���
	{
		Uart->Uart_Rx_flag=0;	                    // ������ձ�־λ
		memset(pData,0, length);		          //����ս�������
    memcpy(pData,Uart->UartRxBuf,Uart->Uart_Rx_len);//�ٸ�������
		memset(Uart->UartRxBuf,0, rx_buf_size);		//��ȡ�����Ҫ��ս�������,��Ȼ��������벻���Ĵ���
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}

/**
* @brief      ����DMA���ͺ���
* @param      DMA_Uart *Uart
* @retval     None
* @attention  None
*/ 
error_status UART_Transmit_DMA(DMA_Uart *Uart)
{
	while(usart_flag_get(Uart->usart_x,USART_TDBE_FLAG) == RESET);	// �ȴ����ڷ��ͻ�����Ϊ�� 0:�ǿ� 1:�� ,Ϊ��ʱ����д������
	if (((Uart->UartTxBuf) == 0) || (Uart->Uart_Tx_len == 0))				// ��һ��������ͷ��ش���
	{
		return ERROR;
	}
	
	dma_channel_enable(Uart->dma_tx_channely, FALSE);           		// �ر�ͨ��
	Uart->dma_tx_channely->dtcnt = Uart->Uart_Tx_len;								// �������÷�������
	dma_channel_enable(Uart->dma_tx_channely, TRUE);								// ��ͨ��

	//while(dma_flag_get(Uart->dma_tx_flag) == RESET);								// �ȴ�DMAͨ���������
	//dma_flag_clear(Uart->dma_tx_flag);															// ���DMA������ɱ�־
	return SUCCESS;
}

/**
* @brief  ����DMAʽprintf�ض���
* @param  Uart:���ں�, format:��ʽ
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


/*�������´���,֧��printf����,������Ҫѡ��use MicroLIB*/
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
