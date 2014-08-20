// dynload.c
// Source file for Dynamic Load module

#include "dynload.h"
#include "dac.h"
#include "pid.h"
#include "adc12.h"
#include <stdint.h>

#define DL_PID_TIMER (1)

typedef struct DYNLOAD_TAG
{
    PID_T       pid;
    uint32_t    pid_timer;
    int         full_scale;
    void       (*control_function)(void);
} DYNLOAD_T;

static DYNLOAD_T dynload;

void DL_ConstantCurrentControl(void);

// Initializes the dynamic load
void DL_Init(void)
{
    PID_Init(&dynload.pid, DL_PID_KP, DL_PID_KI, DL_PID_KD);
    
    dynload.pid_timer = 0;
    dynload.full_scale = 0;
    dynload.control_function = DL_ConstantCurrentControl;
}

// Set the current to draw with electronic load
void DL_SetCurrent(uint16_t set_current_in_ma)
{        
    uint16_t target_voltage = (uint16_t) (set_current_in_ma * DL_CURRENT_RESISTOR);
    
    PID_SetRefValue(&dynload.pid, (int16_t) target_voltage); 
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

void DL_ConstantCurrentControl(void)
{
    // Gets the voltage on the current setting resistor
    uint16_t measured_voltage = DL_GetCurrent();

    // Calculates the amount of correction using the PID
    int16_t correction = PID_Process(&dynload.pid,measured_voltage);

    // Get last dac_value
    uint16_t dac_val = DAC_DacValToMilivolts(DAC_GetDataOutputValue(DAC_Channel_1));
   
    // Corrects DAC value
    dac_val+= correction;

    // Sets DAC value. If Full scale is hit, sets the flag. 
    if (DAC_SetDACValInMilivolts(DAC_Channel_1,dac_val) == DAC_VALUE_OUTSIDE_BOUNDARIES)
        dynload.full_scale = 1;
    else 
        dynload.full_scale = 0;   
        
    // Resets the timer
    dynload.pid_timer = DL_PID_TIMER;
}

// DL Process
void DL_Process()
{
    if(dynload.pid_timer == 0)
    {
        dynload.control_function();
    }
}

// Gets the DL Status
int DL_LockedStatus(void)
{
    return (PID_LockStatus(&dynload.pid));
}

// DL Timertick
void DL_Timertick()
{
    if(dynload.pid_timer > 0) dynload.pid_timer--;
}


