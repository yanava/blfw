// dynload.c
// Source file for Dynamic Load module

#include "dynload.h"
#include "dac.h"
#include "pid.h"
#include "adc12.h"
#include <stdint.h>

#define DL_CURRENT_RESISTOR         (0.1f)
#define DL_VOLTAGE_DIVIDER          (33.4f)
#define DL_ADC_VSENSE               (ADC12_CH1)
#define DL_ADC_ISENSE_LB1           (ADC12_CH2)

// Set the current to draw with electronic load
int  DL_SetCurrent(uint16_t set_current_in_ma)
{        
    uint16_t target_voltage = (uint16_t) (set_current_in_ma * DL_CURRENT_RESISTOR);
    
    if(DAC_SetDACValInMilivolts(DAC_Channel_1,target_voltage) == DAC_OFFSCALE)
       return DL_OFFSCALE;
    else
       return DL_NOERROR;
}

// Gets the actual current draw from the current load
uint16_t DL_GetCurrent(void)
{
    return ((uint16_t) (ADC12_GetOutputBufferSample(DL_ADC_ISENSE_LB1) / DL_CURRENT_RESISTOR));    
}

// Gets the voltage connected to the load circuit
uint16_t DL_GetVoltage(void)
{
   return ((uint16_t) (ADC12_GetOutputBufferSample(DL_ADC_VSENSE) * DL_VOLTAGE_DIVIDER));     
}


