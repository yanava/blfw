// IV.c
// Source file for I-V curve point extraction
// MAH - 07/2014 - Initial Version

#include "fsm.h"
#include "fifo.h"
#include "dac.h"
#include "IV.h"
#include "adc12.h"

#define IV_CURRENT_RESISTOR         (0.1f)
#define IV_CURRENT_CURVE_STEP       (100)
#define IV_CURRENT_DAC_STEP         ((uint16_t) (IV_CURRENT_RESISTOR*IV_CURRENT_CURVE_STEP))
#define IV_VOLTAGE_DIVIDER_FACTOR   (33.4f)
#define IV_VOLTAGE_SC_TOL           (10)
#define IV_DEFAULT_POINT_DELAY      (10)
#define IV_EVENT_LIST_SIZE          (10)
#define IV_CURVE_SIZE               (DAC_VREF_PLUS / IV_CURRENT_DAC_STEP)


// Event type, parameters can be added after super
typedef struct IV_EVENT_TAG
{
    FSM_Event super;
} IV_EVENT_T;

// Event FIFO
typedef struct IV_EVENT_LIST_TAG
{
    FIFO_T     super;
    IV_EVENT_T list[IV_EVENT_LIST_SIZE];
} IV_EVENT_LIST_T;

// IV Point, with 2 
typedef struct IV_POINT_TAG
{
    uint16_t v;
    uint16_t i;
} IV_POINT_T;

typedef struct IV_CURVE_TAG
{
    FIFO_T     super;
    IV_POINT_T points[IV_CURVE_SIZE];
} IV_CURVE_T;

typedef struct IV_TRACER_TAG
{
    FSM super;
    IV_EVENT_LIST_T events;
    IV_CURVE_T curve;
    IV_POINT_T latest_point;
    uint32_t point_delay_ms;
    uint32_t point_delay_counter;
} IV_TRACER_T;

enum IV_SIGNALS
{ 
    IV_START_NEW_CURVE = FSM_USER_SIGNAL,
    IV_POINT_DELAY_TIMEOUT, 
    IV_GET_POINT,
    IV_SHORT_CIRCUIT,
    IV_DAC_FULL_SCALE
};

static IV_TRACER_T iv_tracer;

FSM_State IV_HAND_INITIAL(IV_TRACER_T *me, FSM_Event *e);
FSM_State IV_HAND_IDLE(IV_TRACER_T *me, FSM_Event *e);
FSM_State IV_HAND_OPER(IV_TRACER_T *me, FSM_Event *e);
int IV_Post_Event(IV_TRACER_T *me, IV_EVENT_T *e);

// Initial state. Just performs the initial transition
FSM_State IV_HAND_INITIAL(IV_TRACER_T *me, FSM_Event *e)
{
    return FSM_TRAN(me,IV_HAND_IDLE);
}


// Idle, waiting timeout to get the next IV Curve
FSM_State IV_HAND_IDLE(IV_TRACER_T *me, FSM_Event *e)
{
    switch (e->signal)
    {
        case FSM_ENTRY_SIGNAL:
            return FSM_HANDLED();
        case IV_START_NEW_CURVE:
            me->latest_point.i = 0;
            FIFO_Init(&me->curve.super, &me->curve.points, IV_CURVE_SIZE, sizeof(IV_POINT_T));
            return FSM_TRAN(me,IV_HAND_OPER);
        case FSM_EXIT_SIGNAL:
            return FSM_HANDLED();   
    }
    
    // Default: Handled
    return FSM_HANDLED();
}

// Operation, getting all points until DAC Full scale or Short Circuit
FSM_State IV_HAND_OPER(IV_TRACER_T *me, FSM_Event *e)
{
    switch (e->signal)
    {
        case FSM_ENTRY_SIGNAL:
            return FSM_HANDLED();
        case IV_GET_POINT:
            // Advance DAC
            IV_SetCurrent()
            me->point_delay_counter = me->point_delay_ms;
            return FSM_HANDLED();
        case IV_POINT_DELAY_TIMEOUT:
            // Collect point
            return FSM_HANDLED();
        case IV_SHORT_CIRCUIT:
        case IV_DAC_FULL_SCALE:
            // Curve done
            return FSM_TRAN(me,IV_HAND_OPER);
        case FSM_EXIT_SIGNAL:
            // Set DAC to zero here, to reduce temperature
            return FSM_HANDLED();   
    }
    
    // Default: Handled
    return FSM_HANDLED();
}


// Gets the panel voltage
uint16_t IV_Get_Panel_Voltage(void)
{
    return ((uint16_t) (ADC12_GetOutputBufferSample(ADC12_CH1)*IV_VOLTAGE_DIVIDER_FACTOR)); 
}

// Set the current to draw from panel
void IV_SetCurrent(uint16_t current_in_ma)
{
    uint16_t dac_val = ((uint16_t) (current_in_ma * IV_CURRENT_RESISTOR));
    
    if (DAC_SetDAC1ValInMilivolts(dac_val) == DAC_VALUE_OUTSIDE_BOUNDARIES)
    {
        // POST FULL SCALE EVENT
    }
        
}


// Post an event to the IV Event list
int IV_Post_Event(IV_TRACER_T *me, IV_EVENT_T *iv_e)
{       
    FIFO_T* element_list_as_fifo = (FIFO_T *) &me->events;
    
    int result = FIFO_Post(element_list_as_fifo, iv_e);
    
    return result; 
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