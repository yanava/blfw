// pid.c
// Source file for PID control module

#include "pid.h"
#include <math.h>

// PID Initialization
void PID_Init(PID_T *me, float Kp, float Ki, float Kd)
{
    me->Kp = Kp;
    me->Ki = Ki;
    me->Kd = Kd;
    me->integral = 0;
    me->last_error = 0;
    me->ref_value = 0;
    me->lock_count = PID_NOT_LOCKED;
    me->locked = 0;
    
}

// Sets the reference value
void PID_SetRefValue(PID_T *me, int32_t ref_value)
{
    me->ref_value = ref_value;
}

// Testing Lock Status
int PID_LockStatus(PID_T *me)
{
    switch(me->lock_count)
    {
        case PID_UNLOCK:
            if (me->locked == 1) me->locked = 0;
            break;
        case PID_LOCK:
            if (me->locked == 0) me->locked = 1;
            break;
        default:
            break;
    }
    
    return me->locked;
}

// Process PID output
int32_t PID_Process(PID_T *me, int32_t measured_value)
{
    int32_t error, derivative, integral, control_sig;
     
    // Error calculation
    error = me->ref_value - measured_value;

    // If the error is within tolerance, it's essentially null
    if (error > -PID_ERROR_TOL && error < PID_ERROR_TOL)
        error = 0;
    
    // If error is zero and not fully locked, increases lock count
    if(error == 0 && me->lock_count < PID_FULLY_LOCKED) me->lock_count++;
    
    // If error is not zero, decrements lock_count until PID_NOT_LOCKED
    else if (error != 0 && me->lock_count > PID_NOT_LOCKED) me->lock_count--;
    
    // Updates Lock Status
    PID_LockStatus(me);
    
    // Calculates the integral error
    integral = me->integral + error;
    
    // Integral anti windup, so integral term won't explode if steady state
    // error is different than zero
    if(integral > PID_MAX_INTEGRAL) integral = PID_MAX_INTEGRAL;
    if(integral < - PID_MAX_INTEGRAL) integral = -PID_MAX_INTEGRAL;
    
    // Stores the integral value
    me->integral = integral;
     
    // Calculates the derivative of the error and then stores the error value
    // on last_error
    derivative = error - me->last_error;
    me->last_error = error;
    
    // Calculates the control signal, based on the pre claculated values and
    // their predefined coefficients
    control_sig = (int32_t) (me->Kp * error + me->Ki * integral + me->Kd*derivative);
      
    return control_sig;       
}