// IV.c
// Source file for I-V curve point extraction
// MAH - 07/2014 - Initial Version

#include "fsm.h"
#include "fifo.h"
#include "dac.h"

#define IV_DEFAULT_POINT_DELAY (10)
#define IV_DEFAULT_CUVER_DELAY (300000)
#define IV_EVENT_LIST_SIZE     (20) 

typedef struct IV_EVENT_TAG
{
    FSM_Event super;
    //expand by putting event parameters
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
    uint32_t point_delay_ms;
    uint32_t point_delay_counter;
    uint32_t curve_delay_ms;
    uint32_t curve_delay_counter;
} IV_TRACER_T;

static IV_TRACER_T iv_tracer;

enum IV_SIGNALS
{ 
    IV_POINT_DELAY_TIMEOUT = FSM_USER_SIGNAL,
    IV_CURVE_DELAY_TIMEOUT     
};

int IV_Post_Event(IV_TRACER_T *me, FSM_Event *e);

FSM_State IV_HAND_INITIAL(IV_TRACER_T *me, FSM_Event *e);
FSM_State IV_HAND_IDLE(IV_TRACER_T *me, FSM_Event *e);
FSM_State IV_HAND_DAC_SETTING(IV_TRACER_T *me, FSM_Event *e);

FSM_State IV_HAND_INITIAL(IV_TRACER_T *me, FSM_Event *e)
{
    return FSM_TRAN(me,IV_HAND_IDLE);
}

FSM_State IV_HAND_IDLE(IV_TRACER_T *me, FSM_Event *e)
{
    switch (e->signal)
    {
        case FSM_ENTRY_SIGNAL:
            return FSM_HANDLED();
        case IV_CURVE_DELAY_TIMEOUT:
            return FSM_HANDLED();
        case FSM_EXIT_SIGNAL:
            return FSM_HANDLED();   
    }
    
    return FSM_HANDLED();
}

FSM_State IV_HAND_MEASURING(IV_TRACER_T *me, FSM_Event *e)
{
    switch (e->signal)
    {
        case FSM_ENTRY_SIGNAL:
            return FSM_HANDLED();
        case IV_POINT_DELAY_TIMEOUT:
            return FSM_HANDLED();
        case FSM_EXIT_SIGNAL:
            return FSM_HANDLED();   
    }
    
    return FSM_HANDLED();
}


int IV_Post_Event(IV_TRACER_T *me, FSM_Event *e)
{       
    FIFO_T* element_list_as_fifo = (FIFO_T *) &me->events;
    
    int result = FIFO_Post(element_list_as_fifo, e);
    
    return result; 
}


void IV_Init(void) 
{
    FIFO_Init(&iv_tracer.events.super, &iv_tracer.events.list ,IV_EVENT_LIST_SIZE, sizeof(IV_EVENT_T));
    
    FSM_Ctor(&iv_tracer.super,IV_HAND_INITIAL);
    FSM_Init(&iv_tracer.super);
    
}

// Event dispatcher for IV Curve tracer
void IV_Process(void)
{
}

// IV Timertick
void IV_Timertick (void)
{
    if(iv_tracer.point_delay_counter > 0)
    {
        iv_tracer.point_delay_counter--;
        if (iv_tracer.point_delay_counter == 0);
            // POST EVENT
    }
}