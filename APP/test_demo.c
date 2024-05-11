#include "at32a403a_wk_config.h"
#include "rtthread.h"
#include "test_demo.h"
#include "arm_math.h"
#include "dma_uart.h"

/*-DSP测试----------------------------------------------------------------------------------------*/
#define DELTA 0.0001f   //误差值 
#define times 1000000	//times:运算次数 1百万次

/*MDK 的标准库（math.h）提供：sin、cos、sinf 和 cosf 等 4 个函数，
带 f 的表示单精度浮点型运算，即 float 型，
而不带 f 的表示双精度浮点型，即 double。*/

uint8_t dsp_test_demo(void)
{
	//sin(x)2+cos(x)2=1  测试 
	float angle  =  PI/6;	//angle:起始角度
	float sinx   =	0;
	float cosx   =	0;
	float result =	0; 
	uint32_t i 	 =  0;
	uint32_t time = 0;
	uint32_t err_num = 0;
	UART_Printf(&Uart1,"开始DSP测试......\r\n");
	rt_thread_mdelay(10);
	UART_Printf(&Uart1,"计算三角函数......\r\n");
	rt_thread_mdelay(10);
	time = rt_tick_get();//记下此时的时间
	for(i=0;i<times;i++) 
  { 
		cosx=cosf(angle);     //不使用 DSP 优化的 sin，cos 函数 
		sinx=sinf(angle); 
		result=sinx*sinx+cosx*cosx;    //计算结果应该等于 1   
		result=fabsf(result-1.0f);    //对比与 1 的差值 
		if(result>DELTA)
		{
			err_num++;
		}
		angle+=0.001f;     //角度自增 
  }
	time =	rt_tick_get()-time;//计算时间差
	UART_Printf(&Uart1,"无DSP用时:%dms,不一致次数:%d\r\n",time,err_num);
	rt_thread_mdelay(10);
	err_num = 0;
	time = rt_tick_get();//记下此时的时间
	for(i=0;i<times;i++) 
  {         
    cosx=arm_cos_f32(angle);   //使用 DSP 优化的 sin，cos 函数 
    sinx=arm_sin_f32(angle); 
    result=sinx*sinx+cosx*cosx;   //计算结果应该等于 1   
    result=fabsf(result-1.0f);    //对比与 1 的差值 
    if(result>DELTA)
		{
			err_num++;
		}
    angle+=0.001f;     //角度自增 
  }
	time =	rt_tick_get()-time;//计算时间差
	UART_Printf(&Uart1,"DSP用时:%dms,不一致次数:%d\r\n",time,err_num);
	rt_thread_mdelay(10);
  UART_Printf(&Uart1,"结束DSP测试......\r\n");
	rt_thread_mdelay(10);
	return 0;//任务完成 
}

/*-PWM占空比改变测试------------------------------------------------------------*/
//输入ABC相的高电平时间
void PWM_HighLeaveTime_test(void)
{

}

/*-FOC函数测试-------------------------------------------------------------------------------*/
void foc_test(void)
{

}


/*-SVPWM测试------------------------------------------------------------------------*/
void svpwm_test(void)
{
	
}

/*-ADC测试------------------------------------------------------------------------*/
void adc_test(void)
{

}
/*-编码器测试------------------------------------------------------------------------*/
void encoder_test(void)
{

}
/*-相电阻测量------------------------------------------------------------------------*/

void measure_phase_resistance_test(void)
{

}
