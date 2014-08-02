// dynload.c
// Source file for Dynamic Load module

#include "dac.h"
#include "adc12.h"
#include "fsm.h"
#include "fifo.h"
#include <stdint.h>
#include <math.h>

#define DL_CURRENT_RESISTOR         (0.1f)
#define DL_VOLTAGE_DIVIDER_FACTOR   (33.4f)
#define DL_ADC_VSENSE               (ADC12_CH1)
#define DL_ADC_ISENSE_LB1           (ADC12_CH2)
#define DL_EVENT_LIST_SIZE          (10)

// Event type, parameters can be added after super
typedef struct DL_EVENT_TAG
{
    FSM_Event super;
} DL_EVENT_T;

// Event FIFO type
typedef struct DL_EVENT_LIST_TAG
{
    FIFO_T     super;
    DL_EVENT_T list[DL_EVENT_LIST_SIZE];
} DL_EVENT_LIST_T;
 
// Signals
enum DL_SIGNALS
{ 
    DL_SELECT_MODE_CC = FSM_USER_SIGNAL,
    DL_SELECT_MODE_CV
};

// Forward declaration
typedef struct DL_CTRL_TAG DL_CTRL_T;

// Struct declaration
struct DL_CTRL_TAG
{
    uint16_t set_value;
    void (*control_function)(DL_CTRL_T*);
};

DL_CTRL_T dynload_control;


void DL_CcControl(DL_CTRL_T *me)
{
    uint16_t correct_current = 
        (uin16_t) (ADC12_GetOutputBufferSample(DL_ADC_ISENSE_LB1) / DL_CURRENT_RESISTOR);
    
    uint16_t error = me->set_value - correct_current;
    
    while (error > DL_CURRENT_TOL)
    {
        
    }
    
}
           
// Set the current to draw with electronic load
void DL_SetCurrent(uint16_t current_in_ma)
{
    uint16_t dac_val = ((uint16_t) (current_in_ma * IV_CURRENT_RESISTOR));   
}





