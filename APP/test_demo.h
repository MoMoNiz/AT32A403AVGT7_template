#ifndef __TEST_DEMO_H
#define __TEST_DEMO_H
/* includes -----------------------------------------------------------------------*/
#include "at32a403a.h"

uint8_t dsp_test_demo(void);
void encoder_test_demo(void);

//����ABC��ĸߵ�ƽʱ��
void PWM_HighLeaveTime_test(void);

/*-FOC��������-------------------------------------------------------------------------------*/
void foc_test(void);

/*-SVPWM����------------------------------------------------------------------------*/
void svpwm_test(void);

/*-ADC����------------------------------------------------------------------------*/
void adc_test(void);

/*-����������------------------------------------------------------------------------*/
void encoder_test(void);

/*-��������------------------------------------------------------------------------*/
void measure_phase_resistance_test(void);
#endif