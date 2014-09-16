// IV.c
// Source file for I-V curve point extraction
// MAH - 07/2014 - Initial Version

#include "fsm.h"
#include "fifo.h"
#include "IV.h"
#include "dynload.h"
#include <stdio.h>

#define IV_CURRENT_CURVE_STEP       (10)
#define IV_DEFAULT_POINT_DELAY      (2)
#define IV_EVENT_LIST_SIZE          (10)
#define IV_CURVE_SIZE               ((uint16_t)(DL_MAX_CURRENT/IV_CURRENT_CURVE_STEP))
#define IV_MAX_FATFS_ATTEMPT        (10)

/************* FATFS STACK INCLUDE ********************************/
#include "stm322xg_eval_sdio_sd.h"
#include "ff.h"
#include "diskio.h"
#include "string.h"
/******************************************************************/


// Event type, parameters can be added after super
typedef struct IV_EVENT_TAG
{
    FSM_Event super;
} IV_EVENT_T;

typedef struct IV_FATFS_TAG
{
  FRESULT res;
  FILINFO fno;
  FIL fil;
  DIR dir;
  FATFS fs32;  
} IV_FATFS_T;

// Event FIFO type
typedef struct IV_EVENT_LIST_TAG
{
    FIFO_T     super;
    IV_EVENT_T list[IV_EVENT_LIST_SIZE];
} IV_EVENT_LIST_T;
 
// IV Point type
typedef struct IV_POINT_TAG
{
    uint16_t v;
    uint16_t i;
    uint16_t correct_i;
} IV_POINT_T;

// IV Curve type
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
    IV_POINT_T point;
    IV_FATFS_T filesystem;
    uint32_t point_delay_ms;
    uint32_t point_delay_counter;
} IV_TRACER_T;

enum IV_SIGNALS
{ 
    IV_START_NEW_CURVE = FSM_USER_SIGNAL,
    IV_POINT_DELAY_TIMEOUT, 
    IV_SWEEP_END
};

static IV_TRACER_T iv_tracer;

FSM_State IV_HAND_INITIAL(IV_TRACER_T *me, FSM_Event *e);
FSM_State IV_HAND_IDLE(IV_TRACER_T *me, FSM_Event *e);
FSM_State IV_HAND_OPER(IV_TRACER_T *me, FSM_Event *e);
int IV_Post_Event(IV_TRACER_T *me, IV_EVENT_T *e);
void IV_Finish_Curve(void);
int IV_ExportCurve(char * filename, IV_CURVE_T *curve, IV_FATFS_T *fatfs_handle);

// Initial state. Just performs the initial transition
FSM_State IV_HAND_INITIAL(IV_TRACER_T *me, FSM_Event *e)
{
    // Goes to IDLE mode
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
            return FSM_TRAN(me,IV_HAND_OPER);
        case FSM_EXIT_SIGNAL:
            // Inits Curve FIFO
            FIFO_Init(&me->curve.super, &me->curve.points, IV_CURVE_SIZE, sizeof(IV_POINT_T));
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
            // Sets the point delay for first time
            me->point.i = DL_MIN_CURRENT;
            DL_SetCurrent(me->point.i);
            me->point_delay_counter = me->point_delay_ms;
            return FSM_HANDLED();
        case IV_POINT_DELAY_TIMEOUT:
            // Gets Panel Voltage
            me->point.v = DL_GetVoltage();
            me->point.correct_i = DL_GetCurrent();
            // Puts point on curve FIFO
            FIFO_Post(&me->curve.super, &me->point);
            // Increments current by a step
            me->point.i += IV_CURRENT_CURVE_STEP;
            if(DL_SetCurrent(me->point.i) == DL_OFFSCALE) 
                IV_Finish_Curve();
            // Sets the point delay
            me->point_delay_counter = me->point_delay_ms;
            return FSM_HANDLED();
        case IV_SWEEP_END:
            // Curve done
            IV_ExportCurve("CURVE.TXT",&me->curve,&me->filesystem);
            return FSM_TRAN(me,IV_HAND_IDLE);
        case FSM_EXIT_SIGNAL:
            // Set DAC to zero here, to reduce temperature
            me->point.i = DL_MIN_CURRENT;
            DL_SetCurrent(me->point.i);
            return FSM_HANDLED();   
    }
    // Default: Handled
    return FSM_HANDLED();
}

// Starts a new IV Curve
void IV_Perform_Curve(void)
{
    IV_EVENT_T iv_e;
    
    iv_e.super.signal = IV_START_NEW_CURVE;
    IV_Post_Event(&iv_tracer, &iv_e);  
}

// Finishes the curve
void IV_Finish_Curve(void)
{
    IV_EVENT_T iv_e;
    
    iv_e.super.signal = IV_SWEEP_END;
    IV_Post_Event(&iv_tracer, &iv_e);  
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

// TODO: typedef enum for return values
int IV_ExportCurve(char * filename, IV_CURVE_T *curve, IV_FATFS_T *fatfs_handle)
{
  uint32_t i;
  char aux_string[64];
  UINT BytesWritten;
  unsigned int n_attempt;
  
  //check if the caller is a orc
  if(filename == 0) 
      return 0;
 
  // Enable SD Interrupt 
  SD_InterruptEnable();
  
  // Avoids having previous trash on fatfs handle
  memset(&fatfs_handle->fs32, 0, sizeof(FATFS));
  
  // Mounts the file system on the selected handle
  fatfs_handle->res = f_mount(0, &fatfs_handle->fs32);
 
  // If mounting the file system failed, do not export the curve
  if(fatfs_handle->res != FR_OK)
      return -1;
    
  // Closes any open files
  fatfs_handle->res = f_close(&fatfs_handle->fil);
  
  // Number of tries for creating a new file
  n_attempt = IV_MAX_FATFS_ATTEMPT;
  
  // Tries to create a file with the filename argument. If not successful try
  // again until counter expires. 
  do
  {
      fatfs_handle->res = f_open(&fatfs_handle->fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
      n_attempt -= 1;
  } while(n_attempt > 0 && fatfs_handle->res != FR_OK);
  
  // If number of tries reach zero and still no success
  if(n_attempt == 0 && fatfs_handle->res != FR_OK)
  {
      fatfs_handle->res = f_close(&fatfs_handle->fil);
      return -2;  // very bad, check if the retarded user inserted the sd card
  }
  
  // Seeks the end of file (maybe that's irrelevant, because the file is always created)
  f_lseek(&fatfs_handle->fil, (fatfs_handle->fil.fsize));
     
  // Prints out a data header
  sprintf(aux_string, "Voltage (mV);Set Current (mA); Measured Current (mA)\n");
  
  //Log the whole curve even the empty points, improve this!
  for(i = 0; i < IV_CURVE_SIZE; i++)
  {
    sprintf(aux_string, "%d;%d;%d\n" ,curve->points[i].v, curve->points[i].i, curve->points[i].correct_i);
    fatfs_handle->res = f_write(&fatfs_handle->fil, aux_string, strlen(aux_string), &BytesWritten);
  }  
  
  // After exporting the curve, 
  fatfs_handle->res = f_close(&fatfs_handle->fil);
  
  return 1;
}
