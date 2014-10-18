// IV.c
// Source file for I-V curve point extraction
// MAH - 07/2014 - Initial Version

#include "fsm.h"
#include "fifo.h"
#include "IV.h"
#include "dynload.h"
#include <stdio.h>

#define IV_CURRENT_CURVE_STEP       (10)
#define IV_DEFAULT_POINT_DELAY      (5)
#define IV_DEFAULT_CURVE_DELAY      (60E3) 
#define IV_EVENT_LIST_SIZE          (20)
#define IV_CURVE_SIZE               ((uint16_t)((DL_MAX_CURRENT-DL_MIN_CURRENT)/IV_CURRENT_CURVE_STEP))
#define IV_MAX_FATFS_ATTEMPT        (10)
#define IV_PWR_SCALE_FACTOR         (1E3)
#define IV_CURVE_NAME_MAX_CHARS     (32)

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
    uint32_t p;
} IV_POINT_T;

typedef struct IV_INFO_TAG
{
    IV_POINT_T mpp;
    uint32_t   mpp_index;
    uint16_t   voc;
    uint16_t   isc;
} IV_INFO_T;

typedef struct IV_TRACER_TAG
{
    FSM super;
    IV_EVENT_LIST_T events;
    IV_POINT_T curve[IV_CURVE_SIZE];
    IV_INFO_T  parameters;
    char curve_name[IV_CURVE_NAME_MAX_CHARS];
    uint16_t  index;
    uint16_t  curve_index;
    uint16_t  set_i;
    IV_FATFS_T filesystem;
    uint32_t point_delay_ms;
    uint32_t point_delay_counter;
    uint32_t curve_delay_ms;
    uint32_t curve_delay_counter;
} IV_TRACER_T;

enum IV_SIGNALS
{ 
    IV_START_NEW_CURVE = FSM_USER_SIGNAL,
    IV_POINT_DELAY_TIMEOUT,
    IV_CURVE_DELAY_TIMEOUT,
    IV_SWEEP_END
};

static IV_TRACER_T iv_tracer;

FSM_State IV_HAND_INITIAL(IV_TRACER_T *me, FSM_Event *e);
FSM_State IV_HAND_IDLE(IV_TRACER_T *me, FSM_Event *e);
FSM_State IV_HAND_OPER(IV_TRACER_T *me, FSM_Event *e);
int IV_Post_Event(IV_TRACER_T *me, IV_EVENT_T *e);
int IV_ExportCurve(IV_TRACER_T *me, char * filename);
void IV_Finish_Curve(void);
void IV_GetPointValue(IV_TRACER_T *me);

// Initial state. Just performs the initial transition
FSM_State IV_HAND_INITIAL(IV_TRACER_T *me, FSM_Event *e)
{
    me->curve_index = 0;
    
    // Goes to IDLE mode
    return FSM_TRAN(me,IV_HAND_IDLE);
}

// Idle, waiting timeout to get the next IV Curve
FSM_State IV_HAND_IDLE(IV_TRACER_T *me, FSM_Event *e)
{
    switch (e->signal)
    {
        case FSM_ENTRY_SIGNAL:
            // Sets the current to the minimum
            me->set_i = DL_MIN_CURRENT;
            DL_SetCurrent(me->set_i);
            return FSM_HANDLED();
        case IV_START_NEW_CURVE:
        case IV_CURVE_DELAY_TIMEOUT:
            // Goes to curve extraction state
            return FSM_TRAN(me,IV_HAND_OPER);
        case FSM_EXIT_SIGNAL:
            // Initilize the index
            me->index = 0;
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
            me->point_delay_counter = me->point_delay_ms;
            return FSM_HANDLED();
        case IV_POINT_DELAY_TIMEOUT:
            // Gets the point value
            IV_GetPointValue(me);
            // Increment index. If overflow, end curve. 
            me->index++;
            if(me->index == IV_CURVE_SIZE)
                IV_Finish_Curve();
            // Increments current by a step
            me->set_i += IV_CURRENT_CURVE_STEP;
            DL_SetCurrent(me->set_i);
            // Sets the point delay
            me->point_delay_counter = me->point_delay_ms;
            return FSM_HANDLED();
        case IV_SWEEP_END:
            // Curve done
            sprintf(me->curve_name, "CURVE_%04d.TXT", me->curve_index); 
            IV_ExportCurve(me,(char*) &me->curve_name);
            return FSM_TRAN(me,IV_HAND_IDLE);
        case FSM_EXIT_SIGNAL:
            me->curve_index++;
            me->curve_delay_counter = me->curve_delay_ms;
            return FSM_HANDLED();   
    }
    // Default: Handled
    return FSM_HANDLED();
}

// Gets the point value
void IV_GetPointValue(IV_TRACER_T *me)
{   
    // Gets voltage (mV), current (mA) and calculates power (mW)
    me->curve[me->index].v = DL_GetVoltage();
    me->curve[me->index].i = DL_GetCurrent();
    me->curve[me->index].p = (uint32_t) ((me->curve[me->index].i * me->curve[me->index].v) / IV_PWR_SCALE_FACTOR);  
    
    // PMax analysis
    if (me->curve[me->index].p > me->parameters.mpp.p)
    {
        me->parameters.mpp.p = me->curve[me->index].p;
        me->parameters.mpp.v = me->curve[me->index].v;
        me->parameters.mpp.i = me->curve[me->index].i;
    }
    
    // Voc Analysis (make this better)
    if (me->index == 0)
        me->parameters.voc = me->curve[me->index].v;
    
    // Isc Analysis
    if (me->parameters.isc < me->curve[me->index].i)
        me->parameters.isc = me->curve[me->index].i;
    
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
    iv_tracer.curve_delay_ms = IV_DEFAULT_CURVE_DELAY;
    
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
    
    // Point Delay
    if(iv_tracer.point_delay_counter > 0)
    {
        iv_tracer.point_delay_counter--;
        if (iv_tracer.point_delay_counter == 0)
        {
            iv_e.super.signal = IV_POINT_DELAY_TIMEOUT;
            IV_Post_Event(&iv_tracer, &iv_e);
        }
    }
    
    // Curve Delay
    if(iv_tracer.curve_delay_counter > 0)
    {
        iv_tracer.curve_delay_counter--;
        if (iv_tracer.curve_delay_counter == 0)
        {
            iv_e.super.signal = IV_CURVE_DELAY_TIMEOUT;
            IV_Post_Event(&iv_tracer, &iv_e);
        }
    }
}

// TODO: typedef enum for return values
int IV_ExportCurve(IV_TRACER_T *me, char *filename)
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
  memset(&me->filesystem.fs32, 0, sizeof(FATFS));
  
  // Mounts the file system on the selected handle
  me->filesystem.res = f_mount(0, &me->filesystem.fs32);
 
  // If mounting the file system failed, do not export the curve
  if(me->filesystem.res != FR_OK)
      return -1;
    
  // Closes any open files
  me->filesystem.res = f_close(&me->filesystem.fil);
  
  // Number of tries for creating a new file
  n_attempt = IV_MAX_FATFS_ATTEMPT;
  
  // Tries to create a file with the filename argument. If not successful try
  // again until counter expires. 
  do
  {
      me->filesystem.res = f_open(&me->filesystem.fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
      n_attempt -= 1;
  } while(n_attempt > 0 && me->filesystem.res != FR_OK);
  
  // If number of tries reach zero and still no success
  if(n_attempt == 0 && me->filesystem.res != FR_OK)
  {
      me->filesystem.res = f_close(&me->filesystem.fil);
      return -2;  // very bad, check if the retarded user inserted the sd card
  }
  
  // Seeks the end of file (maybe that's irrelevant, because the file is always created)
  f_lseek(&me->filesystem.fil, (me->filesystem.fil.fsize));
  
  // Puts out the entire Solar Cell and MPP Analysis first  
  sprintf(aux_string, "Solar Cell Analysis\r\n");
  me->filesystem.res = f_write(&me->filesystem.fil, aux_string, strlen(aux_string), &BytesWritten);
  sprintf(aux_string, "------------------- \r\n");
  me->filesystem.res = f_write(&me->filesystem.fil, aux_string, strlen(aux_string), &BytesWritten);
  sprintf(aux_string, "Voc= %d mV \r\n" , me->parameters.voc);
  me->filesystem.res = f_write(&me->filesystem.fil, aux_string, strlen(aux_string), &BytesWritten);
  sprintf(aux_string, "Isc= %d mA \r\n" , me->parameters.isc);
  me->filesystem.res = f_write(&me->filesystem.fil, aux_string, strlen(aux_string), &BytesWritten);
  sprintf(aux_string, "Pmax= %d mW \r\n" , me->parameters.mpp.p);
  me->filesystem.res = f_write(&me->filesystem.fil, aux_string, strlen(aux_string), &BytesWritten);
  sprintf(aux_string, "Vmax= %d mV \r\n" , me->parameters.mpp.v);
  me->filesystem.res = f_write(&me->filesystem.fil, aux_string, strlen(aux_string), &BytesWritten);
  sprintf(aux_string, "Imax= %d mA \r\n" , me->parameters.mpp.i);
  me->filesystem.res = f_write(&me->filesystem.fil, aux_string, strlen(aux_string), &BytesWritten);
  sprintf(aux_string, "------------------- \r\n");
  me->filesystem.res = f_write(&me->filesystem.fil, aux_string, strlen(aux_string), &BytesWritten);
          
  
  // Prints out a data header
  sprintf(aux_string, "Current (mA); Voltage (mV); Power (mW) \n");
  
  //Log the whole curve even the empty points, improve this!
  for(i = 0; i < me->index; i++)
  {
    sprintf(aux_string, "%d;%d;%d \r\n" , me->curve[i].i, me->curve[i].v, me->curve[i].p);
    me->filesystem.res = f_write(&me->filesystem.fil, aux_string, strlen(aux_string), &BytesWritten);
  }  
  
  // After exporting the curve, 
  me->filesystem.res = f_close(&me->filesystem.fil);
  
  return 1;
}
