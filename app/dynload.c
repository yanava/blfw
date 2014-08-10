// dynload.c
// Source file for Dynamic Load module

#include "dynload.h"
#include "dac.h"
#include "pid.h"
#include "adc12.h"
#include <stdint.h>


PID_T pid;
 
void DL_Init(void)
{
    PID_Init(&pid, DL_PID_KP, DL_PID_KI, DL_PID_KD);
}

// Set the current to draw with electronic load
void DL_SetCurrent(uint16_t set_current_in_ma)
{    
    uint16_t target_voltage = (uint16_t) (set_current_in_ma * DL_CURRENT_RESISTOR);
    
    PID_SetRefValue(&pid, (int16_t) target_voltage); 
}

uint16_t probe_measured_voltage;
int16_t  probe_correction;
uint16_t  probe_dac_val;

void DL_Process()
{
    uint16_t measured_voltage = probe_measured_voltage = ADC12_GetOutputBufferSample(DL_ADC_ISENSE_LB1);
    
    int16_t correction = probe_correction = PID_Process(&pid,measured_voltage);
    
    uint16_t dac_val = DAC_DacValToMilivolts(DAC_GetDataOutputValue(DAC_Channel_1));
       
    dac_val += correction;

    probe_dac_val = dac_val;  
    
    DAC_SetDACValInMilivolts(DAC_Channel_1,dac_val);
}




