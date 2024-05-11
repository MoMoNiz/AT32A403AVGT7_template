#ifndef __TEST_DEMO_H
#define __TEST_DEMO_H
/* includes -----------------------------------------------------------------------*/
#include "at32a403a.h"

uint8_t dsp_test_demo(void);
void encoder_test_demo(void);

//输入ABC相的高电平时间
void PWM_HighLeaveTime_test(void);

/*-FOC函数测试-------------------------------------------------------------------------------*/
void foc_test(void);

/*-SVPWM测试------------------------------------------------------------------------*/
void svpwm_test(void);

/*-ADC测试------------------------------------------------------------------------*/
void adc_test(void);

/*-编码器测试------------------------------------------------------------------------*/
void encoder_test(void);

/*-相电阻测量------------------------------------------------------------------------*/
void measure_phase_resistance_test(void);
#endif
