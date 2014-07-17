// IV.c
// Source file for I-V curve point extraction
// MAH - 07/2014 - Initial Version

#include "fsm.h"
#include "fifo.h"
#include "dac.h"
#include "IV.h"
#include "adc12.h"

#define IV_SENSE_RESISTOR  (0.1f)
#define IV_VOLTAGE_DIVIDER (33.4f)

typedef struct IV_EVENT_TAG
{
    FSM_Event super;
    uint16_t  dac_val;
} IV_EVENT_T;

typedef struct IV_EVENT_LIST_TAG
{
    FIFO_T super;
    IV_EVENT_T list[IV_EVENT_LIST_SIZE];
} IV_EVENT_LIST_T;

typedef struct IV_TRACER_TAG
{
    FSM super;
    IV_EVENT_LIST_T events;
    float voltage;
    uint32_t point_delay_ms;
    uint32_t point_delay_counter;
} IV_TRACER_T;

enum IV_SIGNALS
{ 
    IV_POINT_DELAY_TIMEOUT = FSM_USER_SIGNAL,
    IV_GET_POINT,
};

static IV_TRACER_T iv_tracer;

FSM_State IV_HAND_INITIAL(IV_TRACER_T *me, FSM_Event *e);
FSM_State IV_HAND_OPER(IV_TRACER_T *me, FSM_Event *e);
int IV_Post_Event(IV_TRACER_T *me, IV_EVENT_T *e);

// Initial state. Just performs the initial transition
FSM_State IV_HAND_INITIAL(IV_TRACER_T *me, FSM_Event *e)
{
    return FSM_TRAN(me,IV_HAND_OPER);
}

// Idle, waiting timeout to get the next IV Curve
FSM_State IV_HAND_OPER(IV_TRACER_T *me, FSM_Event *e)
{
    IV_EVENT_T *iv_e = (IV_EVENT_T *) e;
    
    switch (e->signal)
    {
        case FSM_ENTRY_SIGNAL:
            return FSM_HANDLED();
        case IV_GET_POINT:
            DAC_SetDAC1ValInMilivolts(iv_e->dac_val);
            me->point_delay_counter = me->point_delay_ms;
            return FSM_HANDLED();
        case IV_POINT_DELAY_TIMEOUT:
            me->voltage = ADC12_GetOutputBufferSample(ADC12_CH1)*IV_VOLTAGE_DIVIDER;
            return FSM_HANDLED();
        case FSM_EXIT_SIGNAL:
            return FSM_HANDLED();   
    }
    
    // Default: Handled
    return FSM_HANDLED();
}

// Post an event to the IV Event list
int IV_Post_Event(IV_TRACER_T *me, IV_EVENT_T *iv_e)
{       
    FIFO_T* element_list_as_fifo = (FIFO_T *) &me->events;
    
    int result = FIFO_Post(element_list_as_fifo, iv_e);
    
    return result; 
}

// Set the current to draw from panel
void IV_Test(uint16_t current_in_ma)
{
    IV_EVENT_T iv_e;
    
    iv_e.super.signal = IV_GET_POINT;
    iv_e.dac_val = current_in_ma / 10;
    
    IV_Post_Event(&iv_tracer, &iv_e);    
}


// Initialization function
void IV_Init(void) 
{
    // Initializes point delay in ms
    iv_tracer.point_delay_ms = IV_DEFAULT_POINT_DELAY;
    
    // Initializes FIFO
    FIFO_Init(&iv_tracer.events.super, 
              &iv_tracer.events.list ,
              IV_EVENT_LIST_SIZE, 
              sizeof(IV_EVENT_T));
    
    // FSM Constructor
    FSM_Ctor(&iv_tracer.super,IV_HAND_INITIAL);
    
    // FSM Init
    FSM_Init(&iv_tracer.super);
}

// Event dispatcher for IV Curve tracer
void IV_Process(void)
{
   IV_EVENT_T evt;
  
   if (FIFO_Get(&iv_tracer.events.super, &evt) == FIFO_NOERROR)
       FSM_Dispatch (&iv_tracer.super, &evt.super);  
}

// IV Timertick
void IV_Timertick (void)
{
    IV_EVENT_T iv_e; 

    if(iv_tracer.point_delay_counter > 0)
    {
        iv_tracer.point_delay_counter--;
        if (iv_tracer.point_delay_counter == 0)
        {
            iv_e.super.signal = IV_POINT_DELAY_TIMEOUT;
            IV_Post_Event(&iv_tracer, &iv_e);
        }
    }
}