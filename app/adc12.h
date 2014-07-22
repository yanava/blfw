// ADC12.H
// Header file for ADC routines
// EB - 07/2014 - Initial Version

#ifndef __ADC12_H
#define __ADC12_H

#include "stm32f2xx.h"

enum ADC12_CHANNELS
{
    ADC12_CH1 = 0,      // PC0 -> ADC123_IN10 - VSENSE
    ADC12_CH2,          // PC2 -> ADC123_IN12 - ISENSE for LB1
    ADC12_CH3,          // PC3 -> ADC123_IN13 - ISENSE for LB2
    ADC12_CH4,
    ADC12_CH5,
    ADC12_CH6
};

void ADC12_Init(void);
void ADC12_Timertic(void);
void ADC12_FilterDMASamples(void);
uint16_t ADC12_GetOutputBufferSample(enum ADC12_CHANNELS const ch);

#endif /* __ADC12_H */