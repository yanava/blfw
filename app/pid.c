// pid.c
// Source file for PID control module

// PID Initialization
void PID_Init(PID_T *me, float Kp, float Ki, float Kd)
{
    me->Kp = Kp;
    me->Ki = Ki;
    me->Kd = Kd;
    me->integral = 0;
    me->last_error = 0;
    me->ref_value = 0;
    
}

// Sets the reference value
void PID_SetRefValue(PID_T *me, uint32_t ref_value)
{
    me->ref_value = ref_value;
}


// Process PID output
// Todo: anti-windup for the integral term (needs experimentation first)
uint32_t PID_Process(PID_T *me, int32_t measured_value)
{
    int32_t error, derivative, integral, control_sig;
    
    error = me->ref_value - measured_value;
    
    integral = me->integral + error;    
    me->integral = integral;
     
    derivative = error - me->last_error;
    me->last_error = error;
    
    control_sig = me->Kp * error + me->Ki * integral + me->Kd*derivative;
    
    return control_sig;       
}
