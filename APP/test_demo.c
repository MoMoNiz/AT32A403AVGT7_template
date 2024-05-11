#include "at32a403a_wk_config.h"
#include "rtthread.h"
#include "test_demo.h"
#include "arm_math.h"
#include "dma_uart.h"

/*-DSP����----------------------------------------------------------------------------------------*/
#define DELTA 0.0001f   //���ֵ 
#define times 1000000	//times:������� 1�����

/*MDK �ı�׼�⣨math.h���ṩ��sin��cos��sinf �� cosf �� 4 ��������
�� f �ı�ʾ�����ȸ��������㣬�� float �ͣ�
������ f �ı�ʾ˫���ȸ����ͣ��� double��*/

uint8_t dsp_test_demo(void)
{
	//sin(x)2+cos(x)2=1  ���� 
	float angle  =  PI/6;	//angle:��ʼ�Ƕ�
	float sinx   =	0;
	float cosx   =	0;
	float result =	0; 
	uint32_t i 	 =  0;
	uint32_t time = 0;
	uint32_t err_num = 0;
	UART_Printf(&Uart1,"��ʼDSP����......\r\n");
	rt_thread_mdelay(10);
	UART_Printf(&Uart1,"�������Ǻ���......\r\n");
	rt_thread_mdelay(10);
	time = rt_tick_get();//���´�ʱ��ʱ��
	for(i=0;i<times;i++) 
  { 
		cosx=cosf(angle);     //��ʹ�� DSP �Ż��� sin��cos ���� 
		sinx=sinf(angle); 
		result=sinx*sinx+cosx*cosx;    //������Ӧ�õ��� 1   
		result=fabsf(result-1.0f);    //�Ա��� 1 �Ĳ�ֵ 
		if(result>DELTA)
		{
			err_num++;
		}
		angle+=0.001f;     //�Ƕ����� 
  }
	time =	rt_tick_get()-time;//����ʱ���
	UART_Printf(&Uart1,"��DSP��ʱ:%dms,��һ�´���:%d\r\n",time,err_num);
	rt_thread_mdelay(10);
	err_num = 0;
	time = rt_tick_get();//���´�ʱ��ʱ��
	for(i=0;i<times;i++) 
  {         
    cosx=arm_cos_f32(angle);   //ʹ�� DSP �Ż��� sin��cos ���� 
    sinx=arm_sin_f32(angle); 
    result=sinx*sinx+cosx*cosx;   //������Ӧ�õ��� 1   
    result=fabsf(result-1.0f);    //�Ա��� 1 �Ĳ�ֵ 
    if(result>DELTA)
		{
			err_num++;
		}
    angle+=0.001f;     //�Ƕ����� 
  }
	time =	rt_tick_get()-time;//����ʱ���
	UART_Printf(&Uart1,"DSP��ʱ:%dms,��һ�´���:%d\r\n",time,err_num);
	rt_thread_mdelay(10);
  UART_Printf(&Uart1,"����DSP����......\r\n");
	rt_thread_mdelay(10);
	return 0;//������� 
}

/*-PWMռ�ձȸı����------------------------------------------------------------*/
//����ABC��ĸߵ�ƽʱ��
void PWM_HighLeaveTime_test(void)
{

}

/*-FOC��������-------------------------------------------------------------------------------*/
void foc_test(void)
{

}


/*-SVPWM����------------------------------------------------------------------------*/
void svpwm_test(void)
{
	
}

/*-ADC����------------------------------------------------------------------------*/
void adc_test(void)
{

}
/*-����������------------------------------------------------------------------------*/
void encoder_test(void)
{

}
/*-��������------------------------------------------------------------------------*/

void measure_phase_resistance_test(void)
{

}
